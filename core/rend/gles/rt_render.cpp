// Render threads (4.131, docs/render_threads_plan.md): N renderers em paralelo.
//
// O frame do jogo vai para uma thread de render livre. Cada thread tem um contexto
// EGL COMPARTILHADO com o do frontend (texturas/buffers/programas compartilhados),
// SEM superficie (EGL_KHR_surfaceless_context -- pbuffer no GBM derrubou o kernel
// 4.4 do R36), e desenha num dos seus DOIS FBOs. A thread principal (retro_run)
// copia (blit) o frame pronto MAIS NOVO para o framebuffer do frontend e apresenta;
// frames mais velhos que ninguem mostrou sao descartados (como o render antigo faz
// quando nao da conta, so que com menos atraso). Enquanto uma thread desenha o
// frame k, a outra ja faz o Process do k+1: a CPU que o driver gasta montando
// comandos se divide entre dois nucleos (teste isolado: 1,75x,
// tools/gl_two_threads.c).
//
// A thread de render pega o proximo frame assim que termina o anterior, sem esperar
// a apresentacao: a emulacao espera o Process de cada frame (g_emuWaitRe, para o
// render nao ler VRAM ja sobrescrita), entao prender o Process ao ritmo da
// apresentacao prendia a emulacao junto (DOA2 90,6% -> 67,2% na 1a versao).
//
// Estado por thread: glcache, gl (shaders/VBOs/FBO de RTT), ShaderUniforms,
// buffers do gldraw/sorter, texturas de fog/paleta/VMU, estado do glsm, _pvrrc
// (todos thread_local). Compartilhado: o cache de texturas do jogo (TexCache).
//  - O Process (TA -> listas + upload de textura) roda um de cada vez (a fila do
//    core tem 1 slot e so libera depois do Process; g_procM por garantia).
//  - Textura usada por um frame em voo que o Process do proximo precisa atualizar
//    ganha um NOME GL NOVO; o antigo so e apagado quando aquele frame termina
//    (versionamento, gltex.cpp + rt_defer_delete).
//  - Parametros de textura (filtro/wrap) sao estado do OBJETO compartilhado: as
//    threads de render usam sampler objects proprios em vez de glTexParameter.
//  - Frames de RTT e de framebuffer rodam no caminho antigo, na thread principal,
//    depois que as threads esvaziam (Renderer_if.cpp, rt_single_frame).
//
// Opt-in: FC_RT=<N> com N >= 2 (FC_RT=1 e o prototipo rt_proto.cpp). Funcoes GLES3
// e EGL resolvidas por dlsym (o build e GLES2: os wrappers do glsm para fence,
// sampler e blit nao fazem nada).
#include "gles.h"
#include "hw/pvr/Renderer_if.h"
#include "hw/pvr/ta_ctx.h"
#include <dirent.h>
#include <dlfcn.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>
#include <atomic>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

extern "C" void glsm_thread_state_init(void);	// glsm.c
bool gl_rt_thread_init();		// gles.cpp
void gl_rt_after_process();		// gles.cpp
void gl_rt_thread_term();		// gles.cpp
void rt_release_after_process(TA_context *ctx, TA_context *shadow);	// Renderer_if.cpp
void rt_wake_main();			// Renderer_if.cpp
void rt_dispatch_pending();		// Renderer_if.cpp
extern std::atomic<u64> g_rendBusyUntilUs;	// Renderer_if.cpp (gate da fila)
extern thread_local u32 t_rtFrameId;	// Renderer_if.cpp (FC_RT_HASH)

void rt_collect_deferred();

thread_local bool t_rtWorker;		// esta thread e uma thread de render
thread_local u64 t_rtCurSeq;		// seq do frame que esta thread esta processando (0 = nenhum)

namespace {

typedef void *EGLDisplay, *EGLContext, *EGLSurface, *EGLConfig;
typedef int EGLint;
typedef struct __GLsync *RtSync;

const EGLint R_EGL_NONE = 0x3038, R_EGL_CONFIG_ID = 0x3028, R_EGL_EXTENSIONS = 0x3055,
		R_EGL_CONTEXT_CLIENT_VERSION = 0x3098;
const GLenum R_SYNC_GPU_COMMANDS_COMPLETE = 0x9117, R_ALREADY_SIGNALED = 0x911A,
		R_CONDITION_SATISFIED = 0x911C, R_READ_FRAMEBUFFER = 0x8CA8, R_DRAW_FRAMEBUFFER = 0x8CA9,
		R_DEPTH24_STENCIL8 = 0x88F0, R_DEPTH_STENCIL_ATTACHMENT = 0x821A;
const GLbitfield R_SYNC_FLUSH_COMMANDS_BIT = 1;
const uint64_t R_TIMEOUT_IGNORED = 0xFFFFFFFFFFFFFFFFull;

struct Egl {
	EGLDisplay (*GetCurrentDisplay)();
	EGLContext (*GetCurrentContext)();
	int (*QueryContext)(EGLDisplay, EGLContext, EGLint, EGLint *);
	int (*ChooseConfig)(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *);
	EGLContext (*CreateContext)(EGLDisplay, EGLConfig, EGLContext, const EGLint *);
	int (*DestroyContext)(EGLDisplay, EGLContext);
	int (*MakeCurrent)(EGLDisplay, EGLSurface, EGLSurface, EGLContext);
	const char *(*QueryString)(EGLDisplay, EGLint);
	EGLint (*GetError)();
} egl;

struct Gl3 {
	RtSync (*FenceSync)(GLenum, GLbitfield);
	void (*WaitSync)(RtSync, GLbitfield, uint64_t);
	GLenum (*ClientWaitSync)(RtSync, GLbitfield, uint64_t);
	void (*DeleteSync)(RtSync);
	void (*BlitFramebuffer)(GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum);
	void (*BindFramebuffer)(GLenum, GLuint);
	void (*GenSamplers)(GLsizei, GLuint *);
	void (*DeleteSamplers)(GLsizei, const GLuint *);
	void (*BindSampler)(GLuint, GLuint);
	void (*SamplerParameteri)(GLuint, GLenum, GLint);
	void (*SamplerParameterf)(GLuint, GLenum, GLfloat);
} gl3;

template <typename T> bool sym(void *h, T &fn, const char *name)
{
	fn = (T)dlsym(h, name);
	if (fn == nullptr)
		fprintf(stderr, "rt: simbolo %s nao encontrado\n", name);
	return fn != nullptr;
}

// Uma saida (FBO) de uma thread de render. `seq` != 0: tem um frame que a main
// ainda nao mostrou nem descartou. `blitFence`: o blit da main que leu esta saida
// (a thread espera antes de desenhar de novo nela).
struct Out
{
	GLuint fbo = 0, tex = 0;
	u64 seq = 0;
	RtSync blitFence = nullptr;
	GLuint readFbo = 0, readTex = 0;	// GL da main: FBO de leitura do blit
};

struct Worker
{
	int id = 0;
	std::thread th;
	EGLContext ctx = nullptr;
	TA_context *shadow = nullptr;	// guarda as listas do frame depois que o slot da fila e liberado
	// pedido (g_m)
	TA_context *job = nullptr;
	u64 jobSeq = 0;
	u32 jobFrameId = 0;
	bool quit = false;
	// GL da thread
	Out out[2];
	GLuint depth = 0;
	int w = 0, h = 0;
};

struct Result
{
	u64 seq = 0;
	int worker = -1, out = -1;
	bool ready = false, ok = false, swap = false;
	bool consumed = false;		// a main ja mostrou ou descartou
	bool done = false;		// a GPU ja terminou o desenho
	RtSync fence = nullptr;		// fim do desenho (contexto da thread)
	GLuint tex = 0;
	int w = 0, h = 0;
};

const int RING = 64;
std::mutex g_m;
std::condition_variable g_cv;		// pedidos para as threads
std::condition_variable g_outCv;	// saida liberada pela main
std::vector<Worker *> g_workers;
Result g_res[RING];
u64 g_doneSeq[RING];
u64 g_nextSeq = 1;			// proximo seq a entregar
u64 g_lastShown = 0;			// ultimo seq mostrado (ou descartado por ser velho)
int g_rendering = 0;			// frames entregues cujo resultado ainda nao saiu
std::atomic<bool> g_slotOwned{false};
std::atomic<u64> g_completed{0};
// Corrente de visibilidade entre contextos: mudanca num objeto compartilhado
// (upload de textura) so e garantida em outro contexto depois de esperar um fence
// do contexto que mudou. Todo Process (thread de render) e todo frame da main
// (RTT/framebuffer) espera este fence ao comecar e o troca pelo seu ao terminar;
// como o Process e serial, a corrente e linear.
RtSync g_chainFence = nullptr;

// Gate da fila (QueueRender, ta_ctx.cpp): com todas as threads desenhando, o proximo
// frame ficaria na fila esperando uma delas e a emulacao, no fim do render emulado,
// esperaria o Process dele (re.Wait): DOA2 90,6 -> 73,5% de VEL. O g_rendBusyUntilUs
// (o mesmo da liberacao antecipada do render antigo) fica "ocupado para sempre"
// enquanto nao houver thread livre e o QueueRender descarta o frame. Estimar o fim do
// desenho (como o render antigo) deixava passar frames quando o desenho atrasava:
// emulacao esperando p99 12 ms. (g_m)
void update_busy_locked()
{
	u64 busy = ~0ull;
	for (Worker *w : g_workers)
		if (w->job == nullptr)
			busy = 0;
	g_rendBusyUntilUs.store(busy, std::memory_order_relaxed);
}

std::mutex g_texM;
std::vector<std::pair<GLuint, u64>> g_deferred;	// texturas para apagar quando o frame terminar
std::mutex g_procM;

EGLDisplay g_dpy = nullptr;
bool g_started = false, g_failed = false;
int g_want = -1;

// marca o frame seq como concluido na GPU e avanca g_completed em ordem (g_m)
void mark_done_locked(u64 seq)
{
	g_doneSeq[seq % RING] = seq;
	u64 c = g_completed.load(std::memory_order_relaxed);
	while (g_doneSeq[(c + 1) % RING] == c + 1)
		c++;
	g_completed.store(c, std::memory_order_release);
}

// o fence do resultado so e apagado quando a GPU terminou E a main ja o usou (g_m)
void maybe_free_fence_locked(Result &r)
{
	if (r.fence != nullptr && r.done && r.consumed)
	{
		gl3.DeleteSync(r.fence);
		r.fence = nullptr;
	}
}

// libera a saida de uma thread (frame mostrado ou descartado) (g_m)
void release_out_locked(const Result &r, RtSync blitFence)
{
	if (r.worker < 0 || r.out < 0)
		return;
	Out &o = g_workers[r.worker]->out[r.out];
	if (o.seq == r.seq)
	{
		o.seq = 0;
		o.blitFence = blitFence;
	}
	g_outCv.notify_all();
}

// espera (na GPU) o fim do ultimo trecho da corrente
void chain_wait()
{
	std::lock_guard<std::mutex> lk(g_m);
	if (g_chainFence != nullptr)
		gl3.WaitSync(g_chainFence, 0, R_TIMEOUT_IGNORED);
}

// poe o fim dos comandos ja emitidos por este contexto na corrente
void chain_signal()
{
	RtSync f = gl3.FenceSync(R_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glFlush();
	std::lock_guard<std::mutex> lk(g_m);
	if (g_chainFence != nullptr)
		gl3.DeleteSync(g_chainFence);
	g_chainFence = f;
}

// Afinidade (FC_RT_PIN=0 desliga): com 4 nucleos, a emulacao fica sozinha no 3,
// as threads de render no 1 e no 2 e a principal no 0-2. Sem isso a emulacao
// migrava de nucleo quando as threads de render acordavam (DOA2, sem o wait do
// Process: 83,5% -> 87,8% de VEL com os nucleos fixos).
bool pin_enabled()
{
	static int en = -1;
	if (en < 0)
	{
		const char *e = getenv("FC_RT_PIN");
		en = (e == nullptr || atoi(e) != 0) && get_nprocs() == 4 ? 1 : 0;
	}
	return en == 1;
}

void pin_self(int first, int last)
{
	cpu_set_t set;
	CPU_ZERO(&set);
	for (int c = first; c <= last; c++)
		CPU_SET(c, &set);
	sched_setaffinity(0, sizeof(set), &set);
}

bool load_syms()
{
	void *e = dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL);
	void *g = dlopen("libGLESv2.so", RTLD_NOW | RTLD_GLOBAL);
	if (e == nullptr || g == nullptr)
	{
		fprintf(stderr, "rt: sem libEGL/libGLESv2\n");
		return false;
	}
	return sym(e, egl.GetCurrentDisplay, "eglGetCurrentDisplay")
		&& sym(e, egl.GetCurrentContext, "eglGetCurrentContext")
		&& sym(e, egl.QueryContext, "eglQueryContext")
		&& sym(e, egl.ChooseConfig, "eglChooseConfig")
		&& sym(e, egl.CreateContext, "eglCreateContext")
		&& sym(e, egl.DestroyContext, "eglDestroyContext")
		&& sym(e, egl.MakeCurrent, "eglMakeCurrent")
		&& sym(e, egl.QueryString, "eglQueryString")
		&& sym(e, egl.GetError, "eglGetError")
		&& sym(g, gl3.FenceSync, "glFenceSync")
		&& sym(g, gl3.WaitSync, "glWaitSync")
		&& sym(g, gl3.ClientWaitSync, "glClientWaitSync")
		&& sym(g, gl3.DeleteSync, "glDeleteSync")
		&& sym(g, gl3.BlitFramebuffer, "glBlitFramebuffer")
		&& sym(g, gl3.BindFramebuffer, "glBindFramebuffer")
		&& sym(g, gl3.GenSamplers, "glGenSamplers")
		&& sym(g, gl3.DeleteSamplers, "glDeleteSamplers")
		&& sym(g, gl3.BindSampler, "glBindSampler")
		&& sym(g, gl3.SamplerParameteri, "glSamplerParameteri")
		&& sym(g, gl3.SamplerParameterf, "glSamplerParameterf");
}

// ---------------------------------------------------------------- samplers (por thread)

thread_local std::unordered_map<u64, GLuint> t_samplers;

void delete_samplers()
{
	for (auto &it : t_samplers)
		gl3.DeleteSamplers(1, &it.second);
	t_samplers.clear();
	gl3.BindSampler(0, 0);
}

// ---------------------------------------------------------------- thread de render

void free_fbos(Worker *w)
{
	for (Out &o : w->out)
	{
		if (o.fbo != 0)
			glDeleteFramebuffers(1, &o.fbo);
		if (o.tex != 0)
			glcache.DeleteTextures(1, &o.tex);
		o.fbo = o.tex = 0;
	}
	if (w->depth != 0)
		glDeleteRenderbuffers(1, &w->depth);
	w->depth = 0;
}

void ensure_fbos(Worker *w, int width, int height)
{
	if (w->depth != 0 && w->w == width && w->h == height)
		return;
	free_fbos(w);
	w->w = width;
	w->h = height;
	glGenRenderbuffers(1, &w->depth);
	glBindRenderbuffer(GL_RENDERBUFFER, w->depth);
	glRenderbufferStorage(GL_RENDERBUFFER, R_DEPTH24_STENCIL8, width, height);
	for (Out &o : w->out)
	{
		glGenTextures(1, &o.tex);
		glcache.BindTexture(GL_TEXTURE_2D, o.tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenFramebuffers(1, &o.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, o.fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, o.tex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, R_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, w->depth);
		GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (st != GL_FRAMEBUFFER_COMPLETE)
			fprintf(stderr, "rt: FBO da thread %d incompleto (0x%x)\n", w->id, st);
	}
}

// Reserva uma saida livre para o frame seq; espera a main liberar uma se as duas
// guardam frames ainda nao mostrados. Devolve -1 se a thread deve sair.
int acquire_out(Worker *w, u64 seq)
{
	RtSync f = nullptr;
	int s;
	{
		std::unique_lock<std::mutex> lk(g_m);
		g_outCv.wait(lk, [&] { return w->quit || w->out[0].seq == 0 || w->out[1].seq == 0; });
		if (w->out[0].seq != 0 && w->out[1].seq != 0)
			return -1;
		s = w->out[0].seq == 0 ? 0 : 1;
		w->out[s].seq = seq;
		f = w->out[s].blitFence;
		w->out[s].blitFence = nullptr;
	}
	if (f != nullptr)
	{
		// o blit que leu esta saida precisa ter terminado antes de desenhar nela
		gl3.ClientWaitSync(f, R_SYNC_FLUSH_COMMANDS_BIT, 500000000ull);
		gl3.DeleteSync(f);
	}
	return s;
}

void worker_main(Worker *w)
{
	if (!egl.MakeCurrent(g_dpy, nullptr, nullptr, w->ctx))
	{
		fprintf(stderr, "rt: eglMakeCurrent da thread %d falhou (0x%x)\n", w->id, egl.GetError());
		return;
	}
	t_rtWorker = true;
	if (pin_enabled())
		pin_self(1 + (w->id % 2), 1 + (w->id % 2));
	glsm_thread_state_init();
	gl_rt_thread_init();
	for (;;)
	{
		TA_context *ctx;
		u64 seq;
		u32 fid;
		{
			std::unique_lock<std::mutex> lk(g_m);
			g_cv.wait(lk, [&] { return w->job != nullptr || w->quit; });
			if (w->job == nullptr)
				break;
			ctx = w->job;
			seq = w->jobSeq;
			fid = w->jobFrameId;
		}
		t_rtCurSeq = seq;
		t_rtFrameId = fid;
		_pvrrc = ctx;
		bool proc;
		{
			std::lock_guard<std::mutex> pl(g_procM);
			chain_wait();
			rt_collect_deferred();
			glcache.InvalidateTextures();
			proc = renderer->Process(ctx);
			gl_rt_after_process();	// registradores/paleta/fog deste frame, antes de soltar a emulacao
			chain_signal();		// os uploads deste Process ficam visiveis a quem vier depois
		}
		{
			std::lock_guard<std::mutex> lk(g_m);
			update_busy_locked();	// antes de soltar o slot
		}
		rt_release_after_process(ctx, w->shadow);	// re.Set + listas para o shadow + libera o slot
		_pvrrc = w->shadow;
		g_slotOwned.store(false, std::memory_order_release);
		rt_dispatch_pending();	// frame que entrou entre o FinishRender e a linha acima
		rt_wake_main();

		bool swap = false;
		int s = -1;
		if (proc)
		{
			ensure_fbos(w, screen_width, screen_height);
			s = acquire_out(w, seq);
			if (s >= 0)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, w->out[s].fbo);
				glcache.InvalidateTextures();
				swap = renderer->Render();
				gl3.BindSampler(0, 0);
			}
		}
		RtSync f = gl3.FenceSync(R_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glFlush();
		{
			std::lock_guard<std::mutex> lk(g_m);
			Result &r = g_res[seq % RING];
			r = Result();
			r.seq = seq;
			r.worker = w->id;
			r.out = s;
			r.ready = true;
			r.ok = proc && s >= 0;
			r.swap = swap;
			r.fence = f;
			if (s >= 0)
			{
				r.tex = w->out[s].tex;
				r.w = w->w;
				r.h = w->h;
			}
			w->job = nullptr;
			g_rendering--;
			g_rendBusyUntilUs.store(0, std::memory_order_relaxed);	// esta thread esta livre
		}
		_pvrrc = nullptr;
		t_rtCurSeq = 0;
		rt_dispatch_pending();
		rt_wake_main();
	}
	free_fbos(w);
	delete_samplers();
	gl_rt_thread_term();
	glFinish();
	egl.MakeCurrent(g_dpy, nullptr, nullptr, nullptr);
}

} // namespace

// ---------------------------------------------------------------- API

// Padrao: 2 threads, se o modo de render permite (sem filtro PowerVR2, sem troca de
// frame atrasada, sem a textura Morton na GPU). FC_RT=<N> muda: 0/1 = render antigo
// (A/B), 2-4 threads.
bool rt_requested()
{
	static const char *e = getenv("FC_RT");
	int want = e != nullptr ? atoi(e) : 2;
	if (want > 4)
		want = 4;
	g_want = want;
	return g_want >= 2 && !g_failed && settings.rend.ThreadedRendering && !settings.rend.PowerVR2Filter
			&& !settings.rend.DelayFrameSwapping && getenv("FC_TEX_GPU_MORTON") == nullptr;
}

// Thread principal, contexto do frontend corrente.
bool rt_start()
{
	if (g_started)
		return true;
	if (g_failed)
		return false;
	g_failed = true;	// so tenta uma vez
	if (!load_syms())
		return false;
	g_dpy = egl.GetCurrentDisplay();
	EGLContext front = egl.GetCurrentContext();
	if (g_dpy == nullptr || front == nullptr)
	{
		fprintf(stderr, "rt: sem display/contexto do frontend\n");
		return false;
	}
	const char *ext = egl.QueryString(g_dpy, R_EGL_EXTENSIONS);
	if (ext == nullptr || strstr(ext, "EGL_KHR_surfaceless_context") == nullptr)
	{
		fprintf(stderr, "rt: EGL sem EGL_KHR_surfaceless_context\n");
		return false;
	}
	// mesma config e mesma versao do contexto do frontend (compartilhamento compativel)
	EGLint cfgId = 0, ver = 3;
	egl.QueryContext(g_dpy, front, R_EGL_CONFIG_ID, &cfgId);
	egl.QueryContext(g_dpy, front, R_EGL_CONTEXT_CLIENT_VERSION, &ver);
	const EGLint cfgAttr[] = { R_EGL_CONFIG_ID, cfgId, R_EGL_NONE };
	EGLConfig cfg = nullptr;
	EGLint n = 0;
	if (!egl.ChooseConfig(g_dpy, cfgAttr, &cfg, 1, &n) || n < 1)
	{
		fprintf(stderr, "rt: config %d do frontend nao encontrada (0x%x)\n", cfgId, egl.GetError());
		return false;
	}
	const EGLint ctxAttr[] = { R_EGL_CONTEXT_CLIENT_VERSION, ver, R_EGL_NONE };
	for (int i = 0; i < g_want; i++)
	{
		Worker *w = new Worker();
		w->id = i;
		w->ctx = egl.CreateContext(g_dpy, cfg, front, ctxAttr);
		if (w->ctx == nullptr)
		{
			fprintf(stderr, "rt: eglCreateContext (thread %d) falhou (0x%x)\n", i, egl.GetError());
			delete w;
			break;
		}
		w->shadow = new TA_context();
		w->shadow->Alloc();
		g_workers.push_back(w);
	}
	if ((int)g_workers.size() < 2)
	{
		for (Worker *w : g_workers)
		{
			egl.DestroyContext(g_dpy, w->ctx);
			delete w;
		}
		g_workers.clear();
		return false;
	}
	memset(g_doneSeq, 0, sizeof(g_doneSeq));
	for (auto &r : g_res)
		r = Result();
	g_nextSeq = 1;
	g_lastShown = 0;
	g_rendering = 0;
	g_completed.store(0);
	g_slotOwned.store(false);
	for (Worker *w : g_workers)
		w->th = std::thread(worker_main, w);
	if (pin_enabled())
		pin_self(0, 2);		// thread principal (retro_run)
	g_started = true;
	g_failed = false;
	fprintf(stderr, "rt: %zu threads de render (contexto GLES %d, config %d)\n", g_workers.size(), ver, cfgId);
	return true;
}

// Thread principal (Term do renderer, contexto do frontend corrente).
void rt_stop()
{
	if (!g_started)
		return;
	{
		std::lock_guard<std::mutex> lk(g_m);
		for (Worker *w : g_workers)
			w->quit = true;
	}
	g_cv.notify_all();
	g_outCv.notify_all();
	for (Worker *w : g_workers)
		if (w->th.joinable())
			w->th.join();
	std::lock_guard<std::mutex> lk(g_m);
	for (auto &r : g_res)
	{
		if (r.fence != nullptr)
			gl3.DeleteSync(r.fence);
		r = Result();
	}
	if (g_chainFence != nullptr)
		gl3.DeleteSync(g_chainFence);
	g_chainFence = nullptr;
	for (Worker *w : g_workers)
	{
		for (Out &o : w->out)
		{
			if (o.blitFence != nullptr)
				gl3.DeleteSync(o.blitFence);
			if (o.readFbo != 0)
				glDeleteFramebuffers(1, &o.readFbo);
		}
		egl.DestroyContext(g_dpy, w->ctx);
		delete w->shadow;
		delete w;
	}
	g_workers.clear();
	{
		std::lock_guard<std::mutex> tl(g_texM);
		for (auto &d : g_deferred)
			glcache.DeleteTextures(1, &d.first);
		g_deferred.clear();
	}
	g_completed.store(0);
	g_slotOwned.store(false);
	g_rendBusyUntilUs.store(0, std::memory_order_relaxed);
	g_started = false;
}

bool rt_slot_owned() { return g_started && g_slotOwned.load(std::memory_order_acquire); }

int rt_rendering()
{
	std::lock_guard<std::mutex> lk(g_m);
	return g_rendering;
}

// Ha frame pronto, com imagem, ainda nao mostrado?
bool rt_has_unpresented()
{
	if (!g_started)
		return false;
	std::lock_guard<std::mutex> lk(g_m);
	for (u64 s = g_lastShown + 1; s < g_nextSeq; s++)
	{
		const Result &r = g_res[s % RING];
		if (r.seq == s && r.ready && !r.consumed && r.ok && r.swap)
			return true;
	}
	return false;
}

bool rt_can_submit()
{
	if (!g_started)
		return false;
	std::lock_guard<std::mutex> lk(g_m);
	for (Worker *w : g_workers)
		if (w->job == nullptr)
			return true;
	return false;
}

// Entrega o frame (que esta no slot da fila) a uma thread de render livre. Chamada
// pela emulacao (logo depois de enfileirar), pelas threads (ao ficarem livres) e pela
// main: quem pegar o slot primeiro entrega. false = slot ja entregue ou nenhuma livre.
bool rt_submit(TA_context *ctx, u32 frameId)
{
	if (!g_started)
		return false;
	bool expect = false;
	if (!g_slotOwned.compare_exchange_strong(expect, true, std::memory_order_acq_rel))
		return false;
	{
		std::lock_guard<std::mutex> lk(g_m);
		Worker *w = nullptr;
		for (Worker *x : g_workers)
			if (x->job == nullptr)
			{
				w = x;
				break;
			}
		if (w == nullptr)
		{
			g_slotOwned.store(false, std::memory_order_release);
			return false;
		}
		w->job = ctx;
		w->jobSeq = g_nextSeq++;
		w->jobFrameId = frameId;
		g_res[w->jobSeq % RING] = Result();
		g_rendering++;
	}
	g_cv.notify_all();
	return true;
}

// Main: marca os frames cuja GPU ja terminou (versionamento de textura).
void rt_poll_completion()
{
	if (!g_started)
		return;
	std::lock_guard<std::mutex> lk(g_m);
	const u64 from = g_nextSeq > (u64)RING ? g_nextSeq - RING : 1;
	for (u64 s = from; s < g_nextSeq; s++)
	{
		Result &r = g_res[s % RING];
		if (r.seq != s || !r.ready || r.done || r.fence == nullptr)
			continue;
		GLenum st = gl3.ClientWaitSync(r.fence, 0, 0);
		if (st == R_ALREADY_SIGNALED || st == R_CONDITION_SATISFIED)
		{
			r.done = true;
			mark_done_locked(s);
			maybe_free_fence_locked(r);
		}
	}
}

// Main: mostra o frame pronto mais novo (blit no framebuffer do frontend); os
// prontos mais velhos que ele, que ninguem mostrou, sao descartados.
// 1 = mostrou, 0 = nada novo pronto.
int rt_try_present()
{
	if (!g_started)
		return 0;
	Result r;
	Worker *w = nullptr;
	{
		std::lock_guard<std::mutex> lk(g_m);
		// atrasados: ficaram prontos depois que um frame mais novo ja foi mostrado
		const u64 from = g_nextSeq > (u64)RING ? g_nextSeq - RING : 1;
		for (u64 s = from; s <= g_lastShown && s < g_nextSeq; s++)
		{
			Result &x = g_res[s % RING];
			if (x.seq == s && x.ready && !x.consumed)
			{
				x.consumed = true;
				release_out_locked(x, nullptr);
				maybe_free_fence_locked(x);
			}
		}
		u64 best = 0;
		for (u64 s = g_lastShown + 1; s < g_nextSeq; s++)
		{
			Result &x = g_res[s % RING];
			if (x.seq == s && x.ready && !x.consumed && x.ok && x.swap)
				best = s;
		}
		// consome o que ja nao vai aparecer: prontos sem imagem e prontos mais velhos
		// que o escolhido
		for (u64 s = g_lastShown + 1; s < g_nextSeq; s++)
		{
			Result &x = g_res[s % RING];
			if (x.seq != s || !x.ready || x.consumed)
				continue;
			if (s == best)
				continue;
			if (s < best || !x.ok || !x.swap)
			{
				x.consumed = true;
				release_out_locked(x, nullptr);
				maybe_free_fence_locked(x);
			}
		}
		if (best == 0)
		{
			// avanca o ponteiro sobre os ja consumidos no comeco da fila
			while (g_lastShown + 1 < g_nextSeq)
			{
				const Result &x = g_res[(g_lastShown + 1) % RING];
				if (x.seq == g_lastShown + 1 && x.consumed)
					g_lastShown++;
				else
					break;
			}
			return 0;
		}
		Result &b = g_res[best % RING];
		b.consumed = true;
		r = b;
		w = g_workers[r.worker];
		g_lastShown = best;
	}
	// espera (na GPU) o desenho da thread e copia para o framebuffer do frontend
	gl3.WaitSync(r.fence, 0, R_TIMEOUT_IGNORED);
	Out &o = w->out[r.out];
	if (o.readFbo == 0 || o.readTex != r.tex)
	{
		if (o.readFbo != 0)
			glDeleteFramebuffers(1, &o.readFbo);
		glGenFramebuffers(1, &o.readFbo);
		gl3.BindFramebuffer(GL_FRAMEBUFFER, o.readFbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, r.tex, 0);
		o.readTex = r.tex;
	}
	const GLuint front = (GLuint)hw_render.get_current_framebuffer();
	gl3.BindFramebuffer(R_READ_FRAMEBUFFER, o.readFbo);
	gl3.BindFramebuffer(R_DRAW_FRAMEBUFFER, front);
	const bool same = r.w == screen_width && r.h == screen_height;
	gl3.BlitFramebuffer(0, 0, r.w, r.h, 0, 0, screen_width, screen_height, GL_COLOR_BUFFER_BIT,
			same ? GL_NEAREST : GL_LINEAR);
	glBindFramebuffer(GL_FRAMEBUFFER, front);	// glsm anota o framebuffer de volta
	RtSync bf = gl3.FenceSync(R_SYNC_GPU_COMMANDS_COMPLETE, 0);
	glFlush();
	std::lock_guard<std::mutex> lk(g_m);
	release_out_locked(r, bf);
	maybe_free_fence_locked(g_res[r.seq % RING]);
	return 1;
}

// Main, antes do caminho antigo (RTT/framebuffer): texturas subidas pelos Process
// das threads ficam visiveis, e o cache de textura desta thread pode lembrar nomes
// que outra thread reciclou.
void rt_main_before_fallback()
{
	if (g_started)
		chain_wait();
	glcache.InvalidateTextures();
	rt_collect_deferred();
}

// Main, depois do caminho antigo: o proximo Process de uma thread espera este fim.
void rt_main_after_fallback()
{
	if (g_started)
		chain_signal();
}

// ---------------------------------------------------------------- texturas versionadas

bool rt_active() { return g_started; }

// Thread da emulacao (rend_start_render): sozinha no nucleo 3.
void rt_pin_emu_thread()
{
	static thread_local bool pinned;
	if (pinned || !g_started || !pin_enabled())
		return;
	pinned = true;
	pin_self(3, 3);
	// O nucleo 3 fica so com a emulacao: as outras threads do processo que podiam ir
	// para ele (apresentacao e audio do frontend, criadas antes) vao para o 0-2.
	const pid_t self = (pid_t)syscall(SYS_gettid);
	if (DIR *d = opendir("/proc/self/task"))
	{
		while (dirent *e = readdir(d))
		{
			const pid_t tid = (pid_t)atoi(e->d_name);
			cpu_set_t cur;
			if (tid <= 0 || tid == self || sched_getaffinity(tid, sizeof(cur), &cur) != 0 || !CPU_ISSET(3, &cur))
				continue;
			cpu_set_t set;
			CPU_ZERO(&set);
			for (int c = 0; c <= 2; c++)
				CPU_SET(c, &set);
			sched_setaffinity(tid, sizeof(set), &set);
		}
		closedir(d);
	}
}

// A textura foi referenciada por um frame que ainda nao terminou na GPU (e nao e
// o frame que esta thread esta processando agora).
bool rt_tex_in_flight(u64 lastUse)
{
	return g_started && lastUse != 0 && lastUse != t_rtCurSeq
			&& lastUse > g_completed.load(std::memory_order_acquire);
}

void rt_mark_tex_use(u64 &lastUse)
{
	if (t_rtCurSeq != 0 && t_rtCurSeq > lastUse)
		lastUse = t_rtCurSeq;
}

void rt_defer_delete(GLuint tex, u64 lastUse)
{
	std::lock_guard<std::mutex> lk(g_texM);
	g_deferred.emplace_back(tex, lastUse);
}

// Apaga as texturas aposentadas cujos frames ja terminaram (thread com contexto GL).
void rt_collect_deferred()
{
	const u64 c = g_completed.load(std::memory_order_acquire);
	std::vector<GLuint> del;
	{
		std::lock_guard<std::mutex> lk(g_texM);
		for (size_t i = 0; i < g_deferred.size();)
		{
			if (!g_started || g_deferred[i].second <= c)
			{
				del.push_back(g_deferred[i].first);
				g_deferred[i] = g_deferred.back();
				g_deferred.pop_back();
			}
			else
				i++;
		}
	}
	if (!del.empty())
		glcache.DeleteTextures((GLsizei)del.size(), del.data());
}

void rt_unbind_sampler()
{
	gl3.BindSampler(0, 0);
}

// Sampler da thread de render para o estado de textura deste poligono.
void rt_bind_sampler(GLint wrapS, GLint wrapT, GLint minF, GLint magF, float aniso)
{
	u64 key = ((u64)(wrapS & 0xFFFF) << 48) | ((u64)(wrapT & 0xFFFF) << 32) | ((u64)(minF & 0xFFFF) << 16)
			| (u64)(magF & 0xFFFF);
	key ^= (u64)(aniso * 16.f) << 4;
	auto it = t_samplers.find(key);
	GLuint s;
	if (it == t_samplers.end())
	{
		gl3.GenSamplers(1, &s);
		gl3.SamplerParameteri(s, GL_TEXTURE_WRAP_S, wrapS);
		gl3.SamplerParameteri(s, GL_TEXTURE_WRAP_T, wrapT);
		gl3.SamplerParameteri(s, GL_TEXTURE_MIN_FILTER, minF);
		gl3.SamplerParameteri(s, GL_TEXTURE_MAG_FILTER, magF);
		if (aniso > 1.f)
			gl3.SamplerParameterf(s, TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		t_samplers[key] = s;
	}
	else
		s = it->second;
	gl3.BindSampler(0, s);
}
