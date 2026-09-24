#include "Renderer_if.h"
#include <dlfcn.h>
#include <atomic>
#include <chrono>	// FC_REND_SPLIT timing below
#include "ta.h"
#include "hw/pvr/pvr_mem.h"
#include "rend/TexCache.h"
#include "hw/mem/_vmem.h"
#include "cheats.h"
#include "spg.h"
#include "hw/sh4/sh4_sched.h"
#include "hw/sh4/sh4_mem.h"
#include "hw/aica/aica_if.h"
#include "deps/xxhash/xxhash.h"

/*

	rendv3 ideas
	- multiple backends
	  - ESish
	    - OpenGL ES2.0
	    - OpenGL ES3.0
	    - OpenGL 3.1
	  - OpenGL 4.x
	  - Direct3D 10+ ?
	- correct memory ordering model
	- resource pools
	- threaded TA
	- threaded rendering
	- RTTs
	- framebuffers
	- overlays


	PHASES
	- TA submission (memops, dma)

	- TA parsing (defered, rend thread)

	- CORE render (in-order, defered, rend thread)


	submission is done in-order
	- Partial handling of TA values
	- Gotchas with TA contexts

	parsing is done on demand and out-of-order, and might be skipped
	- output is only consumed by renderer

	render is queued on RENDER_START, and won't stall the emulation or might be skipped
	- VRAM integrity is an issue with out-of-order or delayed rendering.
	- selective vram snapshots require TA parsing to complete in order with REND_START / REND_END


	Complications
	- For some apis (gles2, maybe gl31) texture allocation needs to happen on the gpu thread
	- multiple versions of different time snapshots of the same texture are required
	- TA parsing vs frameskip logic


	Texture versioning and staging
	 A memory copy of the texture can be used to temporary store the texture before upload to vram
	 This can be moved to another thread
	 If the api supports async resource creation, we don't need the extra copy
	 Texcache lookups need to be versioned


	rendv2x hacks
	- Only a single pending render. Any renders while still pending are dropped (before parsing)
	- wait and block for parse/texcache. Render is async
*/

extern int screen_width;
extern int screen_height;

u32 VertexCount=0;
u32 FrameCount=1;

Renderer* renderer;
static Renderer* fallback_renderer;
bool renderer_changed = false;	// Signals the renderer interface to switch renderer

#if !defined(TARGET_NO_THREADS)
cResetEvent rs;
cResetEvent re;
#endif
extern cResetEvent frame_finished;
static bool swap_pending;
static bool do_swap;
u32 fb_w_cur = 1;

int max_idx,max_mvo,max_op,max_pt,max_tr,max_vtx,max_modt, ovrn;
bool pend_rend = false;

static bool render_called = false;
u32 fb_watch_addr_start;
u32 fb_watch_addr_end;
bool fb_dirty;

TA_context* _pvrrc;

void rend_create_renderer()
{
#ifdef NO_REND
	renderer	 = rend_norend();
#else
	switch (settings.pvr.rend)
	{
	default:
	case 0:
		NOTICE_LOG(PVR, "Creating Open GL per-triangle/strip renderer");
		renderer = rend_GLES2();
		break;
#if defined(HAVE_OIT)
	case 3:
		NOTICE_LOG(PVR, "Creating Open GL per-pixel renderer");
		renderer = rend_GL4();
		fallback_renderer = rend_GLES2();
		break;
#endif
#ifdef HAVE_VULKAN
	case 4:
		NOTICE_LOG(PVR, "Creating Vulkan per-triangle/strip renderer");
		renderer = rend_Vulkan();
		break;
	case 5:
		NOTICE_LOG(PVR, "Creating Vulkan per-pixel renderer");
		renderer = rend_OITVulkan();
		break;
#endif
	}
#endif
}

void rend_init_renderer()
{
	if (!renderer->Init())
    {
		delete renderer;
    	if (fallback_renderer == NULL || !fallback_renderer->Init())
    	{
         delete fallback_renderer;
    		die("Renderer initialization failed\n");
    	}
    	INFO_LOG(PVR, "Selected renderer initialization failed. Falling back to default renderer.");
    	renderer  = fallback_renderer;
    	fallback_renderer = NULL;	// avoid double-free
    }
}

void rend_term_renderer()
{
	if (renderer != NULL)
	{
		renderer->Term();
		delete renderer;
		renderer = NULL;
	}
	if (fallback_renderer != NULL)
	{
		delete fallback_renderer;
		fallback_renderer = NULL;
	}
}

// FC_REND_SPLIT (opt-in): split the main thread's per-frame cost into the part
// spent WAITING for the emu_thread (rsWait) and the part doing actual work
// (Process = TA parsing, Render = GL submission). Without this, `core_average`
// from the benchmark lumps the two together, and a core time of e.g. 59ms says
// nothing about whether the main thread is busy or just blocked -- which is
// exactly the question when cutting JIT work in half doesn't move the frame
// rate. Accumulated here, dumped by rend_dump_split() from retro_run().
bool g_rendSplitEnabled;
u64 g_rendWaitUs, g_rendProcUs, g_rendRenderUs;
u64 g_glFinishUs;	// FC_GL_FINISH, see gles.cpp
u32 g_rendSplitFrames;

// FC_REND_SPLIT (DOA2 2026-09-22): intervalo real (wall clock) entre quadros
// produzidos pela emu_thread, medido em rend_start_render (que roda nela).
// Responde direto "quem limita": a emu_thread (SH4) ou a main thread (GL).
u64 g_emuFrameIntervalUs;
u32 g_emuFrameCount;
// Taxa de frames de render PEDIDOS pelo jogo, em tempo EMULADO. Imune aos
// drops da fila (nao depende de quantos chegam ao render thread). ~60 => jogo
// e 60fps; ~30 => o jogo so renderiza a cada 2 vblanks.
u64 g_reqEmuCycles;
u32 g_reqCount;
u64 g_lastReqCycles;
// Taxa de swap NATIVA do jogo, em tempo EMULADO (independente de quao lento
// roda no device): media de ciclos SH4 entre frames que realmente trocam
// (do_swp). SH4_MAIN_CLOCK / ciclos = fps nativo. Decide 30 vs 60.
u64 g_swapEmuCycles;
u32 g_swapCount;
u64 g_lastSwapCycles;
// Quantos pedidos de render sao RTT/render-framebuffer (nao vao pra tela) vs
// frames de tela de verdade -- decide se o jogo apresenta a 60 ou 30Hz.
u32 g_reqRtt, g_reqDisplay;

static inline u64 rend_now_us()
{
	return (u64)std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
}

extern u64 g_emuReWaitUs;
extern u32 g_emuReWaits;
u32 g_earlyReleases;

void rend_dump_split(const char *path)
{
	FILE *f = fopen(path, "w");
	if (f == nullptr)
		return;
	u32 n = g_rendSplitFrames ? g_rendSplitFrames : 1;
	fprintf(f, "frames\t%u\n", g_rendSplitFrames);
	fprintf(f, "rsWait_ms_avg\t%.3f\n", g_rendWaitUs / 1000.0 / n);
	fprintf(f, "process_ms_avg\t%.3f\n", g_rendProcUs / 1000.0 / n);
	fprintf(f, "render_ms_avg\t%.3f\n", g_rendRenderUs / 1000.0 / n);
	fprintf(f, "emu_frame_interval_ms_avg\t%.3f\n", g_emuFrameCount ? g_emuFrameIntervalUs / 1000.0 / g_emuFrameCount : 0.0);
	fprintf(f, "emu_frames\t%u\n", g_emuFrameCount);
	fprintf(f, "early_releases\t%u\n", g_earlyReleases);
	{
		extern u64 g_schedTicks[32];
		extern u32 g_schedCalls[32];
		extern std::vector<sched_list> sch_list;
		u64 freq = 0;
#if defined(__aarch64__)
		asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
#endif
		if (freq == 0) freq = 1000000;
		Dl_info info;
		for (size_t i = 0; i < sch_list.size() && i < 32; i++)
		{
			uintptr_t off = 0;
			if (dladdr((void *)sch_list[i].cb, &info) != 0)
				off = (uintptr_t)sch_list[i].cb - (uintptr_t)info.dli_fbase;
			fprintf(f, "sched_%zu_cb_off\t%zx\tcalls_per_frame\t%.1f\tms_per_frame\t%.3f\n", i, (size_t)off,
					(double)g_schedCalls[i] / n, g_schedTicks[i] * 1000.0 / freq / n);
		}
		extern u64 g_aicaArmTicks, g_aicaCtrlTicks, g_aicaRenderTicks, g_aicaVoiceSteps, g_aicaQueueFullWaits;
		fprintf(f, "aica_arm7_ms_per_frame\t%.3f\n", g_aicaArmTicks * 1000.0 / freq / n);
		fprintf(f, "aica_ctrl_ms_per_frame\t%.3f\n", g_aicaCtrlTicks * 1000.0 / freq / n);
		fprintf(f, "aica_render_ms_per_frame\t%.3f\n", g_aicaRenderTicks * 1000.0 / freq / n);
		fprintf(f, "aica_voice_steps_per_frame\t%.0f\n", (double)g_aicaVoiceSteps / n);
		fprintf(f, "aica_queue_full_waits_per_frame\t%.2f\n", (double)g_aicaQueueFullWaits / n);
		extern u64 g_aicaFastBlocks, g_aicaSlowBlocks;
		fprintf(f, "aica_fast_blocks_per_frame\t%.1f\naica_slow_blocks_per_frame\t%.1f\n", (double)g_aicaFastBlocks / n, (double)g_aicaSlowBlocks / n);
	}
	{
		extern u64 g_taSqUs;
		extern u32 g_taSqCalls;
		fprintf(f, "ta_sq_calls\t%u\n", g_taSqCalls);
		fprintf(f, "ta_sq_ms_total\t%.1f\n", g_taSqUs / 1000.0);
		fprintf(f, "ta_sq_us_avg\t%.3f\n", g_taSqCalls ? (double)g_taSqUs / g_taSqCalls : 0.0);
	}
	{
		extern u64 g_rsUs[12];
		extern u32 g_rsShaders, g_rsSingleDraws, g_rsBatchDraws, g_rsProgramSwitches, g_rsTexBinds;
		static const char *nm[8] = { "setup", "uniform_loop", "upload", "opaque", "punch", "modvol", "transl", "post" };
		for (int i = 0; i < 8; i++)
			fprintf(f, "rs_%s_ms\t%.3f\n", nm[i], g_rsUs[i] / 1000.0 / n);
		fprintf(f, "rs_sort_ms\t%.3f\n", g_rsUs[8] / 1000.0 / n);
		{
			extern u32 g_rsBreakSameTex, g_rsBreakTcwOnly;
			extern u64 g_rsStateTicks, g_rsDrawTicks;
			u64 freq = 0;
#if defined(__aarch64__)
			asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
#endif
			if (freq == 0) freq = 1000000;
			fprintf(f, "rs_break_tcw_only_per_frame\t%.1f\n", (double)g_rsBreakTcwOnly / n);
			fprintf(f, "rs_break_same_texid_per_frame\t%.1f\n", (double)g_rsBreakSameTex / n);
			fprintf(f, "rs_setgpstate_ms\t%.3f\n", g_rsStateTicks * 1000.0 / freq / n);
			fprintf(f, "rs_drawcall_ms\t%.3f\n", g_rsDrawTicks * 1000.0 / freq / n);
		}
		fprintf(f, "rs_batch_upload_ms\t%.3f\n", g_rsUs[10] / 1000.0 / n);
		fprintf(f, "rs_batch_draw_ms\t%.3f\n", g_rsUs[11] / 1000.0 / n);
		fprintf(f, "rs_tr_strips\t%.1f\n", (double)g_rsUs[9] / n);
		fprintf(f, "rs_shaders\t%u\n", g_rsShaders);
		fprintf(f, "rs_single_draws\t%.1f\n", (double)g_rsSingleDraws / n);
		fprintf(f, "rs_batch_draws\t%.1f\n", (double)g_rsBatchDraws / n);
		fprintf(f, "rs_program_switches\t%.1f\n", (double)g_rsProgramSwitches / n);
		fprintf(f, "rs_tex_binds\t%.1f\n", (double)g_rsTexBinds / n);
	}
	fprintf(f, "emu_re_waits\t%u\n", g_emuReWaits);
	fprintf(f, "emu_re_wait_ms_avg\t%.3f\n", g_emuReWaits ? g_emuReWaitUs / 1000.0 / g_emuReWaits : 0.0);
	fprintf(f, "emu_re_wait_ms_total\t%.1f\n", g_emuReWaitUs / 1000.0);
	// Taxa de swap nativa do jogo em tempo emulado (30 vs 60).
	fprintf(f, "swap_native_fps\t%.2f\n", (g_swapCount && g_swapEmuCycles) ? (double)SH4_MAIN_CLOCK * g_swapCount / g_swapEmuCycles : 0.0);
	fprintf(f, "swaps\t%u\n", g_swapCount);
	// Taxa de render PEDIDA (imune a drops): decisao 30 vs 60 limpa.
	fprintf(f, "req_native_fps\t%.2f\n", (g_reqCount && g_reqEmuCycles) ? (double)SH4_MAIN_CLOCK * g_reqCount / g_reqEmuCycles : 0.0);
	fprintf(f, "reqs\t%u\n", g_reqCount);
	fprintf(f, "req_rtt\t%u\n", g_reqRtt);
	fprintf(f, "req_display\t%u\n", g_reqDisplay);
	extern u32 g_glDrawCalls;
	fprintf(f, "draw_calls_avg\t%.1f\n", (double)g_glDrawCalls / n);
	extern u32 g_batchBreakPcw, g_batchBreakIsp, g_batchBreakTcw, g_batchBreakTsp, g_batchBreakTileclip, g_batchBreakTotal;
	fprintf(f, "batch_breaks_total\t%u\n", g_batchBreakTotal);
	extern u32 g_opaqueRuns, g_opaqueDistinctSum, g_opaqueFramesCounted;
	fprintf(f, "opaque_runs\t%u\n", g_opaqueRuns);
	fprintf(f, "opaque_distinct_states_per_frame\t%.1f\n",
			g_opaqueFramesCounted ? (double)g_opaqueDistinctSum / g_opaqueFramesCounted : 0.0);
	fprintf(f, "batch_break_pcw\t%u\n", g_batchBreakPcw);
	fprintf(f, "batch_break_isp\t%u\n", g_batchBreakIsp);
	fprintf(f, "batch_break_tcw\t%u\n", g_batchBreakTcw);
	fprintf(f, "batch_break_tsp\t%u\n", g_batchBreakTsp);
	fprintf(f, "batch_break_tileclip\t%u\n", g_batchBreakTileclip);
	fprintf(f, "batch_breaks_per_frame\t%.1f\n", (double)g_batchBreakTotal / n);
	fprintf(f, "gl_finish_ms_avg\t%.3f\n", g_glFinishUs / 1000.0 / n);
	fclose(f);
}

// Core-side render work per frame in real time (Process + Render), smoothed.
// QueueRender() compares it with the game's own render interval to decide
// whether waiting for this thread would slow the game down (docs/
// tech_debits.md 4.14). A first version measured "got frame -> started
// waiting for the next one", which also includes the frontend's present --
// and that blocks on vsync, so it read ~16.7ms (the display period) in every
// game, the same as a 60Hz game's interval, and the comparison became a coin
// flip (kofnw dropped 610 frames). Process + Render never waits on vsync.
std::atomic<u32> g_rendWorkUsEma(0);

// Liberacao antecipada do slot da fila (docs/tech_debits.md 4.21/4.29).
// A fila tem UM slot, e o contexto so saia dele depois do Render inteiro:
// com o render quase do tamanho do intervalo do jogo (KOF Evolution na
// chuva: 16,2 x 16,8ms), o frame seguinte chegava com o slot ocupado, era
// descartado, e o render ficava ~10ms parado esperando o proximo (36-43 fps
// com a emulacao a 100%). Agora, logo apos o Process, os dados ja processados
// passam para um contexto-sombra do render (troca de ponteiros, sem copia) e o
// slot e liberado: o proximo frame espera na fila enquanto este e desenhado.
// So quando o render cabe no intervalo do jogo -- senao o Process do frame
// seguinte atrasaria e a emu thread esperaria por ele no fim do render
// (re.Wait), roubando velocidade (DOA2, render 21ms). FC_NO_EARLY_RELEASE=1
// desliga.
static TA_context* rend_shadow;
static bool rend_early_released;
extern u32 g_earlyReleases;
// Previsao de quando o Render em andamento (apos liberacao antecipada)
// termina, em us de steady_clock. QueueRender (emu thread) so enfileira o
// proximo frame se faltar pouco -- senao a emu thread esperaria o Process dele
// no fim do render (re.Wait) e a emulacao cairia para o ritmo do render
// (medido: KOF Evolution 100% -> 90,8%, 2,7ms de espera por frame).
std::atomic<u64> g_rendBusyUntilUs(0);
static u32 g_rendRenderUsEma;

static void rend_fix_overrun_ptrs(rend_context& r)
{
	r.verts.overrun = &r.Overrun;
	r.idx.overrun = &r.Overrun;
	r.modtrig.overrun = &r.Overrun;
	r.global_param_mvo.overrun = &r.Overrun;
	r.global_param_mvo_tr.overrun = &r.Overrun;
	r.global_param_op.overrun = &r.Overrun;
	r.global_param_pt.overrun = &r.Overrun;
	r.global_param_tr.overrun = &r.Overrun;
	r.render_passes.overrun = &r.Overrun;
}

bool rend_frame(TA_context* ctx, bool draw_osd)
{
   if (renderer_changed || renderer == NULL)
   {
	  renderer_changed = false;
	  if (renderer != NULL)
		 rend_term_renderer();
	  rend_create_renderer();
	  rend_init_renderer();
   }
   u64 t0 = rend_now_us();
   bool proc = renderer->Process(ctx);
   if (g_rendSplitEnabled)
      g_rendProcUs += rend_now_us() - t0;
#if !defined(TARGET_NO_THREADS)
   if (settings.rend.ThreadedRendering && (!proc || (!ctx->rend.isRenderFramebuffer && !ctx->rend.isRTT)))
	   // If rendering to texture, continue locking until the frame is rendered
      re.Set();

   {
      static int earlyEnabled = -1;
      if (earlyEnabled == -1)
         earlyEnabled = getenv("FC_NO_EARLY_RELEASE") != nullptr ? 0 : 1;
      extern u32 g_rendIntervalCyclesEma;
      u64 workCycles = (u64)g_rendWorkUsEma.load(std::memory_order_relaxed) * (SH4_MAIN_CLOCK / 1000000);
      bool renderFits = workCycles != 0 && g_rendIntervalCyclesEma != 0 && workCycles <= g_rendIntervalCyclesEma;
      if (earlyEnabled && settings.rend.ThreadedRendering && proc && renderFits
            && !ctx->rend.isRenderFramebuffer && !ctx->rend.isRTT && ctx == _pvrrc)
      {
         if (rend_shadow == nullptr)
         {
            rend_shadow = new TA_context();
            rend_shadow->Alloc();
         }
         std::swap(rend_shadow->rend, ctx->rend);
         rend_fix_overrun_ptrs(rend_shadow->rend);
         rend_fix_overrun_ptrs(ctx->rend);
         _pvrrc = rend_shadow;
         // ~1ms a mais pela apresentacao do frame, que vem logo depois.
         g_rendBusyUntilUs.store(rend_now_us() + g_rendRenderUsEma + 1000, std::memory_order_relaxed);
         FinishRender(ctx);	// libera o slot: a emu thread ja pode enfileirar o proximo
         rend_early_released = true;
         if (g_rendSplitEnabled)
            g_earlyReleases++;
      }
   }
#endif
   
   u64 t1 = rend_now_us();
   bool do_swp = proc && renderer->Render();
   u64 t2 = rend_now_us();
   g_rendBusyUntilUs.store(0, std::memory_order_relaxed);
   if (t2 - t1 < 500000)
      g_rendRenderUsEma = g_rendRenderUsEma == 0 ? (u32)(t2 - t1) : (u32)(((u64)g_rendRenderUsEma * 15 + (t2 - t1)) / 16);
   if (g_rendSplitEnabled)
   {
      g_rendRenderUs += t2 - t1;
      g_rendSplitFrames++;
      if (do_swp)
      {
         u64 nowC = sh4_sched_now64();
         if (g_lastSwapCycles != 0)
         {
            g_swapEmuCycles += nowC - g_lastSwapCycles;
            g_swapCount++;
         }
         g_lastSwapCycles = nowC;
      }
   }
   // Core-side work for this frame (Process + Render), smoothed. The time
   // spent waiting on re.Set() above is the emu thread's, not ours, and it's
   // not in here: t0..t2 is only this thread's own work.
   u64 work = t2 - t0;
   if (work < 500000)
   {
      u32 ema = g_rendWorkUsEma.load(std::memory_order_relaxed);
      ema = ema == 0 ? (u32)work : (u32)((ema * 15 + work) / 16);
      g_rendWorkUsEma.store(ema, std::memory_order_relaxed);
   }

   return do_swp;
}

bool rend_single_frame(void)
{
	while (true)
	{
		//wait render start only if no frame pending
		if (_pvrrc == NULL)
		{
			do
			{
#if !defined(TARGET_NO_THREADS)
				if (settings.rend.ThreadedRendering)
				{
					u64 tw = g_rendSplitEnabled ? rend_now_us() : 0;
					bool got = rs.Wait(100);
					if (g_rendSplitEnabled)
						g_rendWaitUs += rend_now_us() - tw;
					if (!got)
						return false;
					if (do_swap)
					{
						do_swap = false;
						rs.Set();	// set the semaphore in case a render is pending
						return true;
					}
				}
#endif
				_pvrrc = DequeueRender();

				if (!settings.rend.ThreadedRendering && _pvrrc == NULL)
					return false;
			}
			while (!_pvrrc);
		}
		if ((_pvrrc->rend.isRTT || _pvrrc->rend.isRenderFramebuffer) && swap_pending)
		{
			// If there is a frame swap pending, we want to do it now.
			// The current frame "swapping" detection mechanism (using FB_R_SOF1) doesn't work
			// if a RTT frame is rendered in between.
			swap_pending = false;
			return true;
		}

		bool do_swp = rend_frame(_pvrrc, true);
		swap_pending = do_swp && !_pvrrc->rend.isRenderFramebuffer && FB_R_SOF1 != FB_W_SOF1
				 && settings.rend.ThreadedRendering && settings.rend.DelayFrameSwapping;

		if (settings.rend.ThreadedRendering && _pvrrc->rend.isRTT)
			re.Set();

		//clear up & free data ..
		if (!rend_early_released)
			FinishRender(_pvrrc);
		rend_early_released = false;
		_pvrrc=0;

		if (do_swp && !swap_pending)
			return true;
	}
}

void rend_resize(int width, int height)
{
	renderer->Resize(width, height);
}

// FC_STATE_HASH: estado compartilhado com o savestate (state_hash_mark_load)
FILE *g_stateHashFile;
int g_stateHashState = -1;
u32 g_stateHashFrame;
// Carga de savestate: marca no arquivo e zera o contador de pedidos, para
// comparar rodadas a partir do mesmo ponto (o boot antes da carga varia).
void state_hash_mark_load()
{
   if (g_stateHashState == 1)
   {
      fprintf(g_stateHashFile, "L\n");
      fflush(g_stateHashFile);
   }
   g_stateHashFrame = 0;
}

void rend_start_render(void)
{
   render_called = true;
   pend_rend = false;
   if (g_rendSplitEnabled)
   {
      static u64 lastEmuFrame = 0;
      u64 now = rend_now_us();
      if (lastEmuFrame != 0)
      {
         g_emuFrameIntervalUs += now - lastEmuFrame;
         g_emuFrameCount++;
      }
      lastEmuFrame = now;
      // Taxa de pedidos de render do jogo em tempo emulado (imune a drops).
      u64 nowC = sh4_sched_now64();
      if (g_lastReqCycles != 0 && nowC - g_lastReqCycles < SH4_MAIN_CLOCK / 2)
      {
         g_reqEmuCycles += nowC - g_lastReqCycles;
         g_reqCount++;
      }
      g_lastReqCycles = nowC;
   }
   TA_context* ctx = tactx_Pop(CORE_CURRENT_CTX);

   // FC_STATE_HASH=<arquivo> (diagnostico, 2026-09-25): hash por pedido de
   // render dos dados que o SH4 mandou ao TA no frame, e a cada 60 pedidos
   // da RAM principal, VRAM e RAM de som. Duas rodadas do mesmo savestate
   // devem bater linha a linha se a emulacao for deterministica -- base para
   // validar um JIT novo contra o atual.
   {
      extern FILE *g_stateHashFile;
      extern int g_stateHashState;
      extern u32 g_stateHashFrame;
      FILE *&hashFile = g_stateHashFile;
      int &hashState = g_stateHashState;
      u32 &hashFrame = g_stateHashFrame;
      if (hashState < 0)
      {
         const char *p = getenv("FC_STATE_HASH");
         hashFile = p != nullptr ? fopen(p, "w") : nullptr;
         hashState = hashFile != nullptr ? 1 : 0;
      }
      if (hashState == 1 && ctx != nullptr)
      {
         u8 *end = ctx->tad.End();
         size_t n = end - ctx->tad.thd_root;
         fprintf(hashFile, "F %u %llu %zu %016llx", hashFrame, (unsigned long long)sh4_sched_now64(), n,
               (unsigned long long)XXH64(ctx->tad.thd_root, n, 0));
         // FC_STATE_HASH_EVERY=N: RAM/VRAM/ARAM/contexto a cada N pedidos (padrao 60)
         static int every = -1;
         if (every < 0)
         {
            const char *e = getenv("FC_STATE_HASH_EVERY");
            every = e != nullptr ? std::max(1, atoi(e)) : 60;
         }
         if (hashFrame % every == 0)
            fprintf(hashFile, " ram %016llx vram %016llx aram %016llx ctx %016llx",
                  (unsigned long long)XXH64(mem_b.data, mem_b.size, 0),
                  (unsigned long long)XXH64(vram.data, vram.size, 0),
                  (unsigned long long)XXH64(aica_ram.data, aica_ram.size, 0),
                  (unsigned long long)XXH64(&p_sh4rcb->cntx, sizeof(Sh4Context), 0));
         // FC_STATE_HASH_RAMDUMP=F: grava a RAM principal inteira no pedido F
         static int dumpAt = -2;
         if (dumpAt == -2)
         {
            const char *e = getenv("FC_STATE_HASH_RAMDUMP");
            dumpAt = e != nullptr ? atoi(e) : -1;
         }
         if ((int)hashFrame == dumpAt)
         {
            char path[600];
            snprintf(path, sizeof(path), "%s.ram%u", getenv("FC_STATE_HASH"), hashFrame);
            FILE *rf = fopen(path, "wb");
            if (rf != nullptr)
            {
               fwrite(mem_b.data, 1, mem_b.size, rf);
               fclose(rf);
            }
         }
         fputc('\n', hashFile);
         if (hashFrame % 30 == 0)
            fflush(hashFile);
         hashFrame++;
      }
   }

   // No end of render interrupt when rendering the framebuffer
	if (!ctx || !ctx->rend.isRenderFramebuffer)
		SetREP(ctx);

   if (ctx)
   {
      bool is_rtt=(FB_W_SOF1& 0x1000000)!=0 && !ctx->rend.isRenderFramebuffer;
      if (g_rendSplitEnabled)
      {
         if (ctx->rend.isRenderFramebuffer || is_rtt) g_reqRtt++;
         else g_reqDisplay++;
      }

      if (!ctx->rend.Overrun)
      {
         //printf("REP: %.2f ms\n",render_end_pending_cycles/200000.0);
         if (!ctx->rend.isRenderFramebuffer)
            FillBGP(ctx);

         ctx->rend.isRTT      = is_rtt;

         ctx->rend.fb_X_CLIP  = FB_X_CLIP;
         ctx->rend.fb_Y_CLIP  = FB_Y_CLIP;

         ctx->rend.fog_clamp_min = FOG_CLAMP_MIN;
			ctx->rend.fog_clamp_max = FOG_CLAMP_MAX;

         max_idx              = std::max(max_idx,  ctx->rend.idx.used());
         max_vtx              = std::max(max_vtx,  ctx->rend.verts.used());
         max_op               = std::max(max_op,   ctx->rend.global_param_op.used());
         max_pt               = std::max(max_pt,   ctx->rend.global_param_pt.used());
         max_tr               = std::max(max_tr,   ctx->rend.global_param_tr.used());

         max_mvo              = std::max(max_mvo,  ctx->rend.global_param_mvo.used());
         max_modt             = std::max(max_modt, ctx->rend.modtrig.used());

         if (QueueRender(ctx))
         {
            palette_update();
#if !defined(TARGET_NO_THREADS)
            if (settings.rend.ThreadedRendering)
            	rs.Set();
            else
#endif
            	rend_single_frame();
            pend_rend = true;
         }
      }
      else
      {
         ovrn++;
         INFO_LOG(PVR, "WARNING: Rendering context is overrun (%d), aborting frame", ovrn);
         tactx_Recycle(ctx);
      }
   }
}

// FC_REND_SPLIT: tempo que a EMU thread passa bloqueada no fim do render
// esperando o Process() da render thread (re.Wait). E tempo roubado da
// emulacao a cada frame renderizado -- o que decide a velocidade em jogos
// limitados pelo SH4 (DOA2, Zombie Revenge).
u64 g_emuReWaitUs;
u32 g_emuReWaits;

void rend_end_render(void)
{
   if (pend_rend)
   {
#if !defined(TARGET_NO_THREADS)
	   if (settings.rend.ThreadedRendering)
	   {
		   u64 tw = g_rendSplitEnabled ? rend_now_us() : 0;
		   re.Wait();
		   if (g_rendSplitEnabled)
		   {
			   g_emuReWaitUs += rend_now_us() - tw;
			   g_emuReWaits++;
		   }
	   }
	   else
#endif
		  if(renderer != NULL)
			 renderer->Present();
   }
}

void rend_cancel_emu_wait()
{
#if !defined(TARGET_NO_THREADS)
	if (settings.rend.ThreadedRendering)
	{
		rs.Set();
		re.Set();
	}
#endif
	frame_finished.Set();
}

bool rend_init(void)
{
   rend_create_renderer();

#if !defined(TARGET_NO_THREADS)
	if (!settings.rend.ThreadedRendering)
#endif
	{
	   rend_init_renderer();

	   renderer->Resize(screen_width, screen_height);
	}

#if SET_AFNT
	cpu_set_t mask;

	/* CPU_ZERO initializes all the bits in the mask to zero. */
	CPU_ZERO( &mask );
	/* CPU_SET sets only the bit corresponding to cpu. */
	CPU_SET( 0, &mask );

	/* sched_setaffinity returns 0 in success */

	if( sched_setaffinity( 0, sizeof(mask), &mask ) == -1 )
		WARN_LOG(PVR, "WARNING: Could not set CPU Affinity, continuing...");
#endif

	return true;
}

void rend_term(void)
{
}

void rend_vblank()
{
   if (!render_called && fb_dirty && FB_R_CTRL.fb_enable)
	{
		DEBUG_LOG(PVR, "Direct framebuffer write detected");
		u32 saved_ctx_addr = PARAM_BASE;
		bool restore_ctx = ta_ctx != NULL;
		PARAM_BASE = 0xF00000;
		SetCurrentTARC(CORE_CURRENT_CTX);
		ta_ctx->Reset();
		ta_ctx->rend.isRenderFramebuffer = true;
		ta_ctx->rend.isRTT = false;
		rend_start_render();
		PARAM_BASE = saved_ctx_addr;
		if (restore_ctx)
			SetCurrentTARC(CORE_CURRENT_CTX);
		fb_dirty = false;
	}
	render_called = false;
	check_framebuffer_write();
	cheatManager.Apply();

   os_DoEvents();
}

void check_framebuffer_write()
{
   u32 fb_size = (FB_R_SIZE.fb_y_size + 1) * (FB_R_SIZE.fb_x_size + FB_R_SIZE.fb_modulus) * 4;
	fb_watch_addr_start = (SPG_CONTROL.interlace ? FB_R_SOF2 : FB_R_SOF1) & VRAM_MASK;
	fb_watch_addr_end = fb_watch_addr_start + fb_size;
}

void rend_swap_frame()
{
	if (swap_pending)
	{
		swap_pending = false;
		do_swap = true;
		rs.Set();
	}
}
