// Render threads (4.131, docs/render_threads_plan.md) -- etapa 2 (prototipo).
//
// Prova a fundacao: o core consegue pegar o display/contexto EGL do frontend
// (corrente na thread principal) e criar um SEGUNDO contexto EGL COMPARTILHADO
// com ele (texturas/buffers/programas compartilhados; FBO/VAO/estado NAO), fazer
// ele corrente e desenhar/ler num FBO proprio. Opt-in: FC_RT=1. Nao toca o
// caminho de render normal (roda uma vez, no init do renderer).
//
// O core nao linka EGL; resolvemos por dlopen (como o hle_gpu.cpp). Sem pbuffer
// (pbuffer no GBM derrubou o kernel 4.4 do R36): contexto SEM superficie
// (EGL_KHR_surfaceless_context) e desenho so em FBO.
#include "gles.h"		// glsm/glsmsym.h: as funcoes GL
#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

namespace rt_proto {

typedef void *EGLDisplay, *EGLContext, *EGLSurface, *EGLConfig;
typedef int EGLint;

#ifndef EGL_NONE
#define EGL_NONE 0x3038
#endif
#define RT_NO_CONTEXT ((EGLContext)0)
#define RT_NO_SURFACE ((EGLSurface)0)
#define RT_RENDERABLE_TYPE 0x3040
#define RT_OPENGL_ES3_BIT 0x0040
#define RT_CONTEXT_CLIENT_VERSION 0x3098
#define RT_CONTEXT_MINOR_VERSION 0x30FB

struct Egl {
	EGLDisplay (*GetCurrentDisplay)();
	EGLContext (*GetCurrentContext)();
	int (*Initialize)(EGLDisplay, EGLint *, EGLint *);
	int (*ChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
	EGLContext (*CreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
	int (*MakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
	EGLint (*GetError)();
};
static Egl egl;
static bool ready;
static int en = -1;

template <typename T> static bool sym(void *h, T &fn, const char *name)
{
	fn = (T)dlsym(h, name);
	if (fn == nullptr)
		fprintf(stderr, "rt: simbolo %s nao encontrado\n", name);
	return fn != nullptr;
}

bool rt_enabled()
{
	if (en < 0)
		en = getenv("FC_RT") != nullptr && atoi(getenv("FC_RT")) != 0 ? 1 : 0;
	return en == 1;
}

// Chamado uma vez, na thread principal, com o contexto do frontend corrente.
void rt_init()
{
	if (ready || !rt_enabled())
		return;
	ready = true;		// tenta uma vez so

	void *h = dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL);
	if (h == nullptr)
	{
		fprintf(stderr, "rt: sem libEGL\n");
		return;
	}
	bool ok = sym(h, egl.GetCurrentDisplay, "eglGetCurrentDisplay")
			&& sym(h, egl.GetCurrentContext, "eglGetCurrentContext")
			&& sym(h, egl.Initialize, "eglInitialize")
			&& sym(h, egl.ChooseConfig, "eglChooseConfig")
			&& sym(h, egl.CreateContext, "eglCreateContext")
			&& sym(h, egl.MakeCurrent, "eglMakeCurrent")
			&& sym(h, egl.GetError, "eglGetError");
	if (!ok)
		return;

	EGLDisplay dpy = egl.GetCurrentDisplay();
	EGLContext front = egl.GetCurrentContext();
	if (dpy == nullptr || front == RT_NO_CONTEXT)
	{
		fprintf(stderr, "rt: sem display (%p) ou contexto (%p) do frontend\n", dpy, front);
		return;
	}
	EGLint maj = 0, min = 0;
	egl.Initialize(dpy, &maj, &min);

	const EGLint cfgAttr[] = { RT_RENDERABLE_TYPE, RT_OPENGL_ES3_BIT, EGL_NONE };
	EGLConfig cfg;
	EGLint n = 0;
	if (!egl.ChooseConfig(dpy, cfgAttr, &cfg, 1, &n) || n < 1)
	{
		fprintf(stderr, "rt: eglChooseConfig falhou (0x%x)\n", egl.GetError());
		return;
	}
	// Contexto COMPARTILHADO com o do frontend.
	const EGLint ctxAttr[] = { RT_CONTEXT_CLIENT_VERSION, 3, RT_CONTEXT_MINOR_VERSION, 0, EGL_NONE };
	EGLContext shared = egl.CreateContext(dpy, cfg, front, ctxAttr);
	if (shared == RT_NO_CONTEXT)
	{
		fprintf(stderr, "rt: eglCreateContext(share) falhou (0x%x)\n", egl.GetError());
		return;
	}
	// Sem superficie (surfaceless): so desenhamos em FBO.
	if (!egl.MakeCurrent(dpy, RT_NO_SURFACE, RT_NO_SURFACE, shared))
	{
		fprintf(stderr, "rt: eglMakeCurrent(surfaceless) falhou (0x%x) -- frontend tem EGL_KHR_surfaceless_context?\n",
				egl.GetError());
		return;
	}
	// FBO proprio + prova de desenho/leitura.
	GLuint tex = 0, fbo = 0;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
	GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glClearColor(0.25f, 0.5f, 0.75f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	unsigned char px[4] = { 0, 0, 0, 0 };
	glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);
	fprintf(stderr, "rt: 2o contexto EGL compartilhado ok -- FBO %s (0x%x), clear/readback = %u,%u,%u,%u\n",
			st == GL_FRAMEBUFFER_COMPLETE ? "completo" : "INCOMPLETO", st, px[0], px[1], px[2], px[3]);

	// Devolve o contexto do frontend a thread principal.
	egl.MakeCurrent(dpy, RT_NO_SURFACE, RT_NO_SURFACE, front);
}

} // namespace rt_proto
