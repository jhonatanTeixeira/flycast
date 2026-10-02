// EXPERIMENTO (FC_HLE_GPU=1, docs/tech_debits.md 4.102): a mesma conta da
// lightxf (transformacao + iluminacao + tela) num compute shader GLES 3.1 da
// Mali, chamado da thread de emulacao a cada chamada da funcao. O resultado do
// jogo e usado logo em seguida, entao todo caminho por GPU paga, por chamada:
// enviar a entrada -> dispatch -> esperar -> ler de volta. E isso que se mede.
// A versao nativa em CPU continua alimentando a emulacao (o jogo nao quebra) e
// serve de referencia para comparar o resultado bit a bit.
//
// Contexto EGL proprio na thread de emulacao (pbuffer 1x1), no mesmo display
// do frontend (capturado na main thread em retro_run).

#include "types.h"
#include <dlfcn.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <vector>

namespace {

typedef void *EGLDisplay, *EGLContext, *EGLSurface, *EGLConfig;
typedef int EGLint;
typedef unsigned int GLuint, GLenum, GLbitfield;
typedef int GLint, GLsizei;
typedef long GLintptr, GLsizeiptr;
typedef char GLchar;

#define EGL_NONE 0x3038
#define EGL_RENDERABLE_TYPE 0x3040
#define EGL_SURFACE_TYPE 0x3033
#define EGL_PBUFFER_BIT 0x0001
#define EGL_OPENGL_ES3_BIT 0x0040
#define EGL_WIDTH 0x3057
#define EGL_HEIGHT 0x3056
#define EGL_CONTEXT_CLIENT_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define GL_COMPUTE_SHADER 0x91B9
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_MAP_READ_BIT 0x0001
#define GL_BUFFER_UPDATE_BARRIER_BIT 0x00000200
#define GL_SHADER_STORAGE_BARRIER_BIT 0x00002000

struct Api
{
	EGLDisplay (*GetCurrentDisplay)();
	int (*Initialize)(EGLDisplay, EGLint *, EGLint *);
	int (*ChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
	EGLSurface (*CreatePbufferSurface)(EGLDisplay, EGLConfig, const EGLint *);
	EGLContext (*CreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
	int (*MakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
	int (*BindAPI)(unsigned);
	GLuint (*CreateShader)(GLenum);
	void (*ShaderSource)(GLuint, GLsizei, const GLchar *const *, const GLint *);
	void (*CompileShader)(GLuint);
	void (*GetShaderiv)(GLuint, GLenum, GLint *);
	void (*GetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
	GLuint (*CreateProgram)();
	void (*AttachShader)(GLuint, GLuint);
	void (*LinkProgram)(GLuint);
	void (*GetProgramiv)(GLuint, GLenum, GLint *);
	void (*GetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
	void (*UseProgram)(GLuint);
	void (*GenBuffers)(GLsizei, GLuint *);
	void (*BindBuffer)(GLenum, GLuint);
	void (*BindBufferBase)(GLenum, GLuint, GLuint);
	void (*BufferData)(GLenum, GLsizeiptr, const void *, GLenum);
	void (*BufferSubData)(GLenum, GLintptr, GLsizeiptr, const void *);
	void (*DispatchCompute)(GLuint, GLuint, GLuint);
	void (*MemoryBarrier)(GLbitfield);
	void *(*MapBufferRange)(GLenum, GLintptr, GLsizeiptr, GLbitfield);
	unsigned char (*UnmapBuffer)(GLenum);
} gl;

EGLDisplay frontendDisplay;
int state = 0;	// 0 nao iniciado, 1 pronto, -1 falhou
GLuint prog, bufIn, bufPar, bufOut;
size_t capIn, capOut;

const char *kShader = R"(#version 310 es
layout(local_size_x = 64) in;
layout(std430, binding = 0) readonly buffer In { float src[]; };
layout(std430, binding = 1) readonly buffer Par {
	mat4 m;			// XMTRX: coluna c = xf[4c..4c+3]
	vec4 scr;		// sx, sy, ox, oy
	ivec4 cnt;		// n, nlights
	vec4 ldir[16];
	vec4 lcol[16];
};
layout(std430, binding = 2) writeonly buffer Out { float rec[]; };
void main()
{
	int i = int(gl_GlobalInvocationID.x);
	if (i >= cnt.x)
		return;
	int s = i * 6;
	vec4 P = m * vec4(src[s], src[s + 1], src[s + 2], 1.0);
	vec4 N = m * vec4(src[s + 3], src[s + 4], src[s + 5], 0.0);
	vec3 col = vec3(0.0);
	for (int l = 0; l < cnt.y; l++)
	{
		float d = dot(N.xyz, ldir[l].xyz);
		if (0.0 > d)
			col += (-d) * lcol[l].xyz;
	}
	float iz = 1.0 / P.z;
	int o = i * 8;
	rec[o] = P.z;
	rec[o + 1] = scr.z + (scr.x * P.x) * iz;
	rec[o + 2] = scr.w + (scr.y * P.y) * iz;
	rec[o + 3] = iz;
	rec[o + 4] = 1.0;
	rec[o + 5] = col.x;
	rec[o + 6] = col.y;
	rec[o + 7] = col.z;
}
)";

template <typename T> bool sym(void *h, T &fn, const char *name)
{
	fn = (T)dlsym(h, name);
	if (fn == nullptr)
		fprintf(stderr, "hle-gpu: simbolo %s nao encontrado\n", name);
	return fn != nullptr;
}

bool init()
{
	void *h = dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL);
	if (h == nullptr || frontendDisplay == nullptr)
	{
		fprintf(stderr, "hle-gpu: sem libEGL (%p) ou sem display (%p)\n", h, frontendDisplay);
		return false;
	}
	bool ok = sym(h, gl.Initialize, "eglInitialize") && sym(h, gl.ChooseConfig, "eglChooseConfig")
		&& sym(h, gl.CreatePbufferSurface, "eglCreatePbufferSurface") && sym(h, gl.CreateContext, "eglCreateContext")
		&& sym(h, gl.MakeCurrent, "eglMakeCurrent") && sym(h, gl.BindAPI, "eglBindAPI")
		&& sym(h, gl.CreateShader, "glCreateShader") && sym(h, gl.ShaderSource, "glShaderSource")
		&& sym(h, gl.CompileShader, "glCompileShader") && sym(h, gl.GetShaderiv, "glGetShaderiv")
		&& sym(h, gl.GetShaderInfoLog, "glGetShaderInfoLog") && sym(h, gl.CreateProgram, "glCreateProgram")
		&& sym(h, gl.AttachShader, "glAttachShader") && sym(h, gl.LinkProgram, "glLinkProgram")
		&& sym(h, gl.GetProgramiv, "glGetProgramiv") && sym(h, gl.GetProgramInfoLog, "glGetProgramInfoLog")
		&& sym(h, gl.UseProgram, "glUseProgram") && sym(h, gl.GenBuffers, "glGenBuffers")
		&& sym(h, gl.BindBuffer, "glBindBuffer") && sym(h, gl.BindBufferBase, "glBindBufferBase")
		&& sym(h, gl.BufferData, "glBufferData") && sym(h, gl.BufferSubData, "glBufferSubData")
		&& sym(h, gl.DispatchCompute, "glDispatchCompute") && sym(h, gl.MemoryBarrier, "glMemoryBarrier")
		&& sym(h, gl.MapBufferRange, "glMapBufferRange") && sym(h, gl.UnmapBuffer, "glUnmapBuffer");
	if (!ok)
		return false;
	EGLint maj, min;
	gl.Initialize(frontendDisplay, &maj, &min);
	gl.BindAPI(0x30A0);	// EGL_OPENGL_ES_API
	const EGLint cfgAttr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT, EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, EGL_NONE };
	EGLConfig cfg;
	EGLint n = 0;
	if (!gl.ChooseConfig(frontendDisplay, cfgAttr, &cfg, 1, &n) || n < 1)
	{
		fprintf(stderr, "hle-gpu: eglChooseConfig falhou\n");
		return false;
	}
	const EGLint pbAttr[] = { EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE };
	EGLSurface surf = gl.CreatePbufferSurface(frontendDisplay, cfg, pbAttr);
	const EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 1, EGL_NONE };
	EGLContext ctx = gl.CreateContext(frontendDisplay, cfg, nullptr, ctxAttr);
	if (surf == nullptr || ctx == nullptr || !gl.MakeCurrent(frontendDisplay, surf, surf, ctx))
	{
		fprintf(stderr, "hle-gpu: contexto EGL falhou (surf %p ctx %p)\n", surf, ctx);
		return false;
	}
	GLuint sh = gl.CreateShader(GL_COMPUTE_SHADER);
	gl.ShaderSource(sh, 1, &kShader, nullptr);
	gl.CompileShader(sh);
	GLint st = 0;
	gl.GetShaderiv(sh, GL_COMPILE_STATUS, &st);
	if (!st)
	{
		char log[1024];
		gl.GetShaderInfoLog(sh, sizeof(log), nullptr, log);
		fprintf(stderr, "hle-gpu: shader nao compilou: %s\n", log);
		return false;
	}
	prog = gl.CreateProgram();
	gl.AttachShader(prog, sh);
	gl.LinkProgram(prog);
	gl.GetProgramiv(prog, GL_LINK_STATUS, &st);
	if (!st)
	{
		char log[1024];
		gl.GetProgramInfoLog(prog, sizeof(log), nullptr, log);
		fprintf(stderr, "hle-gpu: link falhou: %s\n", log);
		return false;
	}
	GLuint b[3];
	gl.GenBuffers(3, b);
	bufIn = b[0]; bufPar = b[1]; bufOut = b[2];
	gl.BindBuffer(GL_SHADER_STORAGE_BUFFER, bufPar);
	gl.BufferData(GL_SHADER_STORAGE_BUFFER, 16 * 4 + 16 + 16 + 16 * 16 * 2, nullptr, GL_DYNAMIC_DRAW);
	fprintf(stderr, "hle-gpu: compute shader pronto (EGL %d.%d)\n", maj, min);
	return true;
}

} // namespace

// main thread (retro_run), com o contexto do frontend corrente
void hle_gpu_capture_display()
{
	if (frontendDisplay != nullptr)
		return;
	void *h = dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL);
	if (h == nullptr)
		return;
	EGLDisplay (*cur)() = (EGLDisplay (*)())dlsym(h, "eglGetCurrentDisplay");
	if (cur != nullptr)
		frontendDisplay = cur();
}

bool hle_gpu_enabled()
{
	static int e = -1;
	if (e < 0)
		e = getenv("FC_HLE_GPU") != nullptr && atoi(getenv("FC_HLE_GPU")) != 0;
	return e == 1;
}

// thread de emulacao: roda a lightxf na GPU; out = n*8 floats (registro de 32
// bytes por vertice, mesmo layout da SQ). Devolve false se a GPU nao esta pronta.
bool hle_gpu_lightxf(const float *src, u32 n, const float *m16, const float *scr4,
		int nl, const float *ldir4, const float *lcol4, float *out)
{
	if (state == 0)
		state = init() ? 1 : -1;
	if (state != 1 || nl > 16)
		return false;
	const size_t inBytes = (size_t)n * 6 * 4, outBytes = (size_t)n * 8 * 4;
	gl.UseProgram(prog);
	gl.BindBuffer(GL_SHADER_STORAGE_BUFFER, bufIn);
	if (inBytes > capIn)
	{
		capIn = inBytes * 2;
		gl.BufferData(GL_SHADER_STORAGE_BUFFER, capIn, nullptr, GL_DYNAMIC_DRAW);
	}
	gl.BufferSubData(GL_SHADER_STORAGE_BUFFER, 0, inBytes, src);
	struct { float m[16]; float scr[4]; int cnt[4]; float ldir[16][4]; float lcol[16][4]; } par;
	memcpy(par.m, m16, sizeof(par.m));
	memcpy(par.scr, scr4, sizeof(par.scr));
	par.cnt[0] = (int)n; par.cnt[1] = nl; par.cnt[2] = par.cnt[3] = 0;
	memcpy(par.ldir, ldir4, nl * 16);
	memcpy(par.lcol, lcol4, nl * 16);
	gl.BindBuffer(GL_SHADER_STORAGE_BUFFER, bufPar);
	gl.BufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(par), &par);
	gl.BindBuffer(GL_SHADER_STORAGE_BUFFER, bufOut);
	if (outBytes > capOut)
	{
		capOut = outBytes * 2;
		gl.BufferData(GL_SHADER_STORAGE_BUFFER, capOut, nullptr, GL_DYNAMIC_READ);
	}
	gl.BindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, bufIn);
	gl.BindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bufPar);
	gl.BindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bufOut);
	gl.DispatchCompute((n + 63) / 64, 1, 1);
	gl.MemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
	gl.BindBuffer(GL_SHADER_STORAGE_BUFFER, bufOut);
	void *p = gl.MapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, outBytes, GL_MAP_READ_BIT);	// espera a GPU
	if (p == nullptr)
		return false;
	memcpy(out, p, outBytes);
	gl.UnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	return true;
}
