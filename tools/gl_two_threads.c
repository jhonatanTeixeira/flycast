// rr2: dá para renderizar em 2 threads na Mali ao mesmo tempo?
// Mede o custo de CPU por draw (padrão do flycast: muitos glDrawElements pequenos com
// troca de textura/uniform/blend) com 1 thread e com 2 threads (contextos EGL
// compartilhados = round robin real; e independentes = sem grupo de compartilhamento).
// Offscreen: GBM + EGL, cada thread desenha num FBO 640x480 próprio, 2 frames em voo.
//
//   (no device) gcc -O2 gl_two_threads.c -o rr2 -lEGL -lGLESv2 -lgbm -lpthread
//   Superfície: janela GBM. Com pbuffer no GBM o kernel 4.4 do R36 deu oops (NULL
//   pointer) e travou a GPU até o reboot (2026-10-10) — não usar pbuffer.
//   ./rr2 [draws_por_frame=640] [frames=600] [init|1|2s|2i|all]
#define _GNU_SOURCE
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <gbm.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static EGLDisplay dpy;
static EGLConfig cfg;
static EGLContext root;           // contexto dono dos recursos compartilhados
static int DRAWS = 640, FRAMES = 600;
static const int NTEX = 64, W = 640, H = 480;

static double now(clockid_t c)
{
	struct timespec t;
	clock_gettime(c, &t);
	return t.tv_sec + t.tv_nsec * 1e-9;
}

struct Res { GLuint prog, vbo, ibo, tex[64]; GLint ucol; };
static struct Res shared;

static GLuint shader(GLenum type, const char *src)
{
	GLuint s = glCreateShader(type);
	glShaderSource(s, 1, &src, NULL);
	glCompileShader(s);
	GLint ok;
	glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
	if (!ok) { char log[512]; glGetShaderInfoLog(s, 512, NULL, log); fprintf(stderr, "shader: %s\n", log); exit(1); }
	return s;
}

static void make_res(struct Res *r)
{
	const char *vs = "#version 300 es\nlayout(location=0) in vec2 p; layout(location=1) in vec2 uv;"
	                 "out vec2 t; void main(){ t=uv; gl_Position=vec4(p,0.0,1.0); }";
	const char *fs = "#version 300 es\nprecision mediump float; in vec2 t; uniform sampler2D s; uniform vec4 col;"
	                 "out vec4 o; void main(){ o=texture(s,t)*col; }";
	r->prog = glCreateProgram();
	glAttachShader(r->prog, shader(GL_VERTEX_SHADER, vs));
	glAttachShader(r->prog, shader(GL_FRAGMENT_SHADER, fs));
	glLinkProgram(r->prog);
	r->ucol = glGetUniformLocation(r->prog, "col");
	// 256 strips pequenos de 12 vértices (triângulos minúsculos: o custo é CPU, não GPU)
	enum { NV = 256 * 12 };
	static float v[NV * 4];
	for (int i = 0; i < NV; i++)
	{
		float x = -1.f + (i % 97) * 0.02f, y = -1.f + (i % 89) * 0.022f;
		v[i * 4 + 0] = x + (i & 1) * 0.01f;
		v[i * 4 + 1] = y + ((i >> 1) & 1) * 0.01f;
		v[i * 4 + 2] = (i & 1);
		v[i * 4 + 3] = ((i >> 1) & 1);
	}
	static unsigned short idx[NV];
	for (int i = 0; i < NV; i++) idx[i] = i;
	glGenBuffers(1, &r->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);
	glGenBuffers(1, &r->ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
	static unsigned char px[64 * 64 * 4];
	glGenTextures(NTEX, r->tex);
	for (int t = 0; t < NTEX; t++)
	{
		memset(px, t * 4, sizeof(px));
		glBindTexture(GL_TEXTURE_2D, r->tex[t]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	glFinish();
}

static EGLContext make_ctx(EGLContext share)
{
	const EGLint attr[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
	EGLContext c = eglCreateContext(dpy, cfg, share, attr);
	if (c == EGL_NO_CONTEXT) { fprintf(stderr, "eglCreateContext falhou 0x%x\n", eglGetError()); exit(1); }
	return c;
}

static struct gbm_device *gbm;
static EGLSurface make_surf(void)
{
	// janela GBM fora da tela (o mesmo caminho do SDL kmsdrm do retrorun); cada contexto
	// tem a sua. Desenhamos em FBO, então ela só serve para o eglMakeCurrent.
	struct gbm_surface *gs = gbm_surface_create(gbm, 64, 64, GBM_FORMAT_XRGB8888,
	                                            GBM_BO_USE_RENDERING);
	if (!gs) { fprintf(stderr, "gbm_surface_create falhou\n"); exit(1); }
	EGLSurface s = eglCreateWindowSurface(dpy, cfg, (EGLNativeWindowType)gs, NULL);
	if (s == EGL_NO_SURFACE) { fprintf(stderr, "eglCreateWindowSurface falhou 0x%x\n", eglGetError()); exit(1); }
	return s;
}

struct Job { int id, frames, shared_ctx; double wall, cpu; };

static void *worker(void *arg)
{
	struct Job *j = arg;
	EGLContext c = make_ctx(j->shared_ctx ? root : EGL_NO_CONTEXT);
	EGLSurface s = make_surf();
	if (!eglMakeCurrent(dpy, s, s, c)) { fprintf(stderr, "makecurrent 0x%x\n", eglGetError()); exit(1); }
	struct Res own, *r = &shared;
	if (!j->shared_ctx) { make_res(&own); r = &own; }
	// FBO e VAO são por contexto (não se compartilham)
	GLuint fbo, col, dep, vao;
	glGenTextures(1, &col);
	glBindTexture(GL_TEXTURE_2D, col);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glGenRenderbuffers(1, &dep);
	glBindRenderbuffer(GL_RENDERBUFFER, dep);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, W, H);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, col, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dep);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r->ibo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 16, (void *)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 16, (void *)8);
	glUseProgram(r->prog);
	glViewport(0, 0, W, H);
	glEnable(GL_DEPTH_TEST);
	glFinish();

	GLsync fence[2] = { 0, 0 };
	const double w0 = now(CLOCK_MONOTONIC), c0 = now(CLOCK_THREAD_CPUTIME_ID);
	for (int f = 0; f < j->frames; f++)
	{
		if (fence[f & 1])      // no máximo 2 frames em voo, como a apresentação real
		{
			glClientWaitSync(fence[f & 1], GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000ull);
			glDeleteSync(fence[f & 1]);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int d = 0; d < DRAWS; d++)
		{
			glBindTexture(GL_TEXTURE_2D, r->tex[(d * 7 + f) % NTEX]);
			if ((d & 7) == 0)
			{
				if (d & 8) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
				else glDisable(GL_BLEND);
				glDepthMask((d & 16) ? GL_FALSE : GL_TRUE);
			}
			glUniform4f(r->ucol, (d & 3) * 0.3f, 0.5f, 0.7f, 1.f);
			glDrawElements(GL_TRIANGLE_STRIP, 12, GL_UNSIGNED_SHORT, (void *)(size_t)((d % 256) * 24));
		}
		fence[f & 1] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glFlush();
	}
	glFinish();
	j->wall = now(CLOCK_MONOTONIC) - w0;
	j->cpu = now(CLOCK_THREAD_CPUTIME_ID) - c0;
	eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	return NULL;
}

static void run(const char *name, int nthreads, int shared_ctx)
{
	struct Job j[2];
	pthread_t t[2];
	const double w0 = now(CLOCK_MONOTONIC);
	for (int i = 0; i < nthreads; i++)
	{
		j[i] = (struct Job){ i, FRAMES / nthreads, shared_ctx, 0, 0 };
		pthread_create(&t[i], NULL, worker, &j[i]);
	}
	for (int i = 0; i < nthreads; i++) pthread_join(t[i], NULL);
	const double wall = now(CLOCK_MONOTONIC) - w0;
	printf("%-34s frames=%d  parede=%.2fs  %.1f frames/s", name, FRAMES, wall, FRAMES / wall);
	for (int i = 0; i < nthreads; i++)
		printf(" | t%d: cpu %.2fs (%.0f%% da parede) %.2f us/draw", i, j[i].cpu, 100 * j[i].cpu / j[i].wall,
		       1e6 * j[i].cpu / ((double)j[i].frames * DRAWS));
	printf("\n");
	fflush(stdout);
}

int main(int argc, char **argv)
{
	if (argc > 1) DRAWS = atoi(argv[1]);
	if (argc > 2) FRAMES = atoi(argv[2]);
	setvbuf(stdout, NULL, _IONBF, 0);
	const char *mode = argc > 3 ? argv[3] : "all";
	int fd = open("/dev/dri/card0", O_RDWR);
	gbm = gbm_create_device(fd);
	dpy = eglGetDisplay((EGLNativeDisplayType)gbm);
	if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, NULL, NULL)) { fprintf(stderr, "EGL init falhou 0x%x\n", eglGetError()); return 1; }
	eglBindAPI(EGL_OPENGL_ES_API);
	const char *ext = eglQueryString(dpy, EGL_EXTENSIONS);
	printf("EGL %s | surfaceless=%s\n", eglQueryString(dpy, EGL_VERSION),
	       ext && strstr(ext, "EGL_KHR_surfaceless_context") ? "sim" : "nao");
	const EGLint ca[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR, EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	                      EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_NONE };
	EGLint n = 0;
	if (!eglChooseConfig(dpy, ca, &cfg, 1, &n) || n == 0)
	{
		const EGLint cb[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR, EGL_NONE };
		eglChooseConfig(dpy, cb, &cfg, 1, &n);
		printf("sem config de janela (%d)\n", n);
	}
	root = make_ctx(EGL_NO_CONTEXT);
	EGLSurface rs = make_surf();
	if (!eglMakeCurrent(dpy, rs, rs, root)) { fprintf(stderr, "makecurrent root 0x%x\n", eglGetError()); return 1; }
	printf("GL %s | %s | draws/frame=%d\n", glGetString(GL_VERSION), glGetString(GL_RENDERER), DRAWS);
	make_res(&shared);
	eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	printf("init ok\n");
	if (!strcmp(mode, "init")) return 0;
	if (!strcmp(mode, "1") || !strcmp(mode, "all")) run("1 thread", 1, 1);
	if (!strcmp(mode, "2s") || !strcmp(mode, "all")) run("2 threads, contextos compartilhados", 2, 1);
	if (!strcmp(mode, "2i") || !strcmp(mode, "all")) run("2 threads, contextos independentes", 2, 0);
	return 0;
}
