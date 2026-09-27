#include "ta_ctx.h"
#include <chrono>
#include <atomic>
#include "spg.h"
#include "oslib/oslib.h"
#include <cstdlib>
#include <atomic>

#include "hw/sh4/sh4_sched.h"

#if defined(HAVE_LIBNX)
#include <malloc.h>
#endif

extern u32 fskip;
extern u32 FrameCount;

int frameskip=0;
bool FrameSkipping=false;		// global switch to enable/disable frameskip
u32 g_queueDrops, g_queueOk, g_queueWaits;	// QueueRender: frames dropped / queued / waited for (see there)
u32 g_queueBusyDrops;	// descartados por render ainda ocupado apos liberacao antecipada
u32 g_rendIntervalCyclesEma;	// game's render interval, emulated cycles (QueueRender)

// ---- Frame pacer (docs/frame_pacing_plan.md) -------------------------------
// Apresenta 1 a cada `g_pacerDiv` frames do jogo, para os frames apresentados
// ficarem IGUALMENTE espacados quando a main thread (Process+Render+present)
// nao sustenta a taxa nativa. Em vez do descarte reativo irregular, pula
// determinísticamente. Adapta devagar (janela de ~120 frames) pelo descarte
// residual: ainda descarta -> aperta (div++); sem descarte e div>1 -> afrouxa.
// Medido 2026-09-27 (MvC2): pular frames PIORA (div 2 -> p50 13,9ms/423 frames;
// div 3 -> 33,7ms/286; off -> 10,4ms/517). A main thread fica ociosa (nao e
// CPU-bound): pular so faz ela esperar mais no rs.Wait, sem reduzir o custo por
// frame apresentado. Fica opt-in (FC_PACER=1) para nao perder o mecanismo.
u32 g_pacerDiv = 1;
u32 g_pacerSkips;		// frames pulados pelo pacer (diagnostico)
int g_pacerEnabled = 0;		// FC_PACER=1 liga (medido pior; ver plano)
static u32 pacerTick, pacerWinDrops, pacerWinFrames;

// ---- Orcamento de render por TEMPO (docs/frame_pacing_plan.md) --------------
// Alvo de frame time: se um frame chega ANTES de `g_renderBudgetUs` desde o
// ultimo renderizado, descarta a renderizacao e SEGUE (nao espera). 0 = off.
// Substitui o "esperar o render": o emulador nunca bloqueia, so descarta o que
// chegou cedo demais. FC_RENDER_BUDGET_MS=<ms> (A/B).
u64 g_renderBudgetUs = 0;
static u64 lastRenderUs = 0;

// Teto de frameskip (opcao do core, padrao 33%): quantos % dos frames o core
// pode descartar para manter o jogo a 100% de velocidade. Acima do teto ele
// para de descartar e espera o render -- a velocidade cai, mas a apresentacao
// nao degrada mais. 0 = nunca descarta; 100 = sempre descarta (comportamento
// antigo). Setado pela opcao do core; FC_SKIP_BUDGET sobrescreve para A/B.
// Modelo novo (docs/frame_pacing_plan.md): a emu NUNCA espera o render -- roda
// sempre a 100%. O renderizador descarta o que nao cabe (gate: a fila tem 1
// slot; se o render ainda esta no frame anterior, o novo e ignorado).
// FC_EMU_WAIT=1 restaura o comportamento antigo (espera) para A/B.
int g_emuNeverWaits = 1;
// FC_EMU_WAIT_RE (default 1): mesmo no modelo no-wait, a emu espera o re.Set()
// do rend_end_render. Esse re e sinalizado logo apos o Process (upload de
// textura), nao depois do draw -- e o unico ponto que precisa serializar para a
// VRAM nao ser sobrescrita enquanto o render ainda le as texturas (glitch de
// sprite). Custa o Process, nao o frame inteiro.
int g_emuWaitRe = 1;

int g_frameskipBudgetPct = 33;
void ta_set_frameskip_budget(int pct)
{
	if (pct < 0) pct = 0;
	if (pct > 100) pct = 100;
	g_frameskipBudgetPct = pct;
}

TA_context* ta_ctx;
tad_context ta_tad;

TA_context*  vd_ctx;
rend_context vd_rc;

// helper for 32 byte aligned memory allocation
void* OS_aligned_malloc(size_t align, size_t size)
{
#ifdef __MINGW32__
   return __mingw_aligned_malloc(size, align);
#elif defined(_WIN32)
   return _aligned_malloc(size, align);
#elif defined(HAVE_LIBNX)
   return memalign(align, size);
#else
   void *p = NULL;
   int ret = posix_memalign(&p, align, size);
   return (ret == 0) ? p : 0;
#endif
}

// helper for 32 byte aligned memory de-allocation
void OS_aligned_free(void *ptr)
{
#ifdef __MINGW32__
   __mingw_aligned_free(ptr);
#elif defined(_WIN32)
   _aligned_free(ptr);
#else
   free(ptr);
#endif
}

void SetCurrentTARC(u32 addr)
{
	if (addr != TACTX_NONE)
	{
		if (ta_ctx)
			SetCurrentTARC(TACTX_NONE);

      verify(ta_ctx == 0);
		//set new context
		ta_ctx = tactx_Find(addr,true);

		//copy cached params
		ta_tad = ta_ctx->tad;
	}
	else
	{
		//Flush cache to context
      verify(ta_ctx != 0);
		ta_ctx->tad=ta_tad;
		
		//clear context
		ta_ctx=0;
      ta_tad.Reset(0);
	}
}

bool TryDecodeTARC(void)
{
   verify(ta_ctx != 0);
   
	if (vd_ctx == 0)
	{
		vd_ctx = ta_ctx;

		vd_ctx->rend.proc_start = vd_ctx->rend.proc_end + 32;
		vd_ctx->rend.proc_end = vd_ctx->tad.thd_data;
			
      vd_ctx->rend_inuse.lock();
		vd_rc = vd_ctx->rend;

		//signal the vdec thread
		return true;
	}
   else
      return false;
}

void VDecEnd(void)
{
   verify(vd_ctx != 0);

	vd_ctx->rend = vd_rc;

   vd_ctx->rend_inuse.unlock();

	vd_ctx = 0;
}

cMutex mtx_rqueue;
TA_context* rqueue;
cResetEvent frame_finished;

bool QueueRender(TA_context* ctx)
{
   verify(ctx != 0);

   // Fracao de frames descartados, em media movel (constante ~32 frames). A
   // decisao de esperar-ou-descartar abaixo usa esta media: enquanto estiver
   // dentro do teto, descarta (velocidade primeiro); acima, espera o render.
   static bool lastFrameDropped = false;
   static float dropRateEma = 0.f;
   dropRateEma = dropRateEma * (31.f / 32.f) + (lastFrameDropped ? 1.f : 0.f) * (1.f / 32.f);
   lastFrameDropped = false;

   // Frame pacer (docs/frame_pacing_plan.md): pula determinísticamente os
   // frames que nao serao apresentados, para os apresentados ficarem even.
   static int pacerEnvInit = 0;
   if (pacerEnvInit == 0)
   {
      pacerEnvInit = 1;
      const char *e = getenv("FC_PACER");
      if (e != nullptr)
         g_pacerEnabled = atoi(e) != 0;
      const char *d = getenv("FC_PACER_DIV");
      if (d != nullptr)		// diagnostico: div fixo, sem adaptacao
      {
         g_pacerDiv = (u32)atoi(d);
         if (g_pacerDiv < 1) g_pacerDiv = 1;
      }
   }
   static bool pacerFixed = getenv("FC_PACER_DIV") != nullptr;
   if (g_pacerEnabled)
   {
      pacerWinFrames++;
      if (!pacerFixed && pacerWinFrames >= 120)
      {
         float resid = (float)pacerWinDrops / (float)pacerWinFrames;
         if (resid > 0.05f && g_pacerDiv < 4)
            g_pacerDiv++;
         else if (resid < 0.01f && g_pacerDiv > 1)
            g_pacerDiv--;
         pacerWinDrops = 0;
         pacerWinFrames = 0;
      }
      pacerTick++;
      if (g_pacerDiv > 1 && (pacerTick % g_pacerDiv) != 0)
      {
         g_pacerSkips++;
         tactx_Recycle(ctx);
         return false;	// nao conta como drop do budget (proativo, even)
      }
   }

   // Orcamento por TEMPO: descarta a renderizacao se o frame chegou antes do
   // alvo de frame time. Nao espera -- segue em frente (VEL 100%). Adaptativo:
   // descarte residual (fila ocupada) -> aumenta o intervalo; sem descarte por
   // varias janelas -> diminui (probe). FC_RENDER_BUDGET_MS fixa (A/B).
   static int budgetMsInit = 0;
   static bool budgetFixed = false;
   static u32 budgetWinFrames, budgetWinDrops, budgetCool;
   if (budgetMsInit == 0)
   {
      budgetMsInit = 1;
      const char *b = getenv("FC_RENDER_BUDGET_MS");
      if (b != nullptr)
      {
         g_renderBudgetUs = (u64)atoi(b) * 1000ull;
         budgetFixed = true;
      }
   }
   static u64 budgetTail;
   static int budgetAdaptive = -1;
   if (budgetAdaptive == -1)
      budgetAdaptive = getenv("FC_RENDER_BUDGET") != nullptr ? 1 : 0;
   if (!budgetFixed && budgetAdaptive)
   {
      // TOC: o intervalo entre frames renderizados tem de caber a CAUDA do
      // custo (Process+Render+present), nao a media. A cauda "diz que esta
      // estrangulada" -> alivia (aumenta o budget); se ela cai, aperta.
      extern u64 g_lastRendWorkUs, g_lastPresentUs;
      u64 cause = g_lastRendWorkUs + g_lastPresentUs;
      if (cause != 0)
      {
         budgetTail = (u64)(budgetTail * 0.90);	// decai ~10 frames
         if (cause > budgetTail)
            budgetTail = cause;			// sobe na hora (cauda)
      }
      u64 want = budgetTail + budgetTail / 8 + 1000;	// cauda + folga
      if (want > 60000) want = 60000;
      g_renderBudgetUs = want;
   }
   if (g_renderBudgetUs != 0)
   {
      u64 nowUs = (u64)std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
      if (lastRenderUs != 0 && nowUs - lastRenderUs < g_renderBudgetUs)
      {
         tactx_Recycle(ctx);
         return false;
      }
      lastRenderUs = nowUs;
   }

   if (FrameSkipping && frameskip) {
 		frameskip=1-frameskip;
 		tactx_Recycle(ctx);
 		fskip++;
 		lastFrameDropped = true;
 		return false;
  	}

   if (settings.pvr.SynchronousRendering)
   {
      //Try to limit speed to a "sane" level
      //Speed is also limited via audio, but audio
      //is sometimes not accurate enough (android, vista+)
      static double last_frame = 0;
      static u64 last_cycles   = 0;
      u64 sched_now            = sh4_sched_now64();
      u32 cycle_span           = (u32)(sched_now - last_cycles);
      last_cycles              = sched_now;
      double time_in_secs      = os_GetSeconds();
      double time_span         = time_in_secs - last_frame;
      last_frame               = time_in_secs;
      bool too_fast            = (cycle_span / time_span) > SH4_MAIN_CLOCK;

      // Vulkan: RTT frames seem to be discarded often
      if (rqueue && (too_fast || ctx->rend.isRTT))
      {
         //wait for a frame if
         //  we have another one queue'd and
         //  sh4 run at > 120% on the last slice
         //  and SynchronousRendering is enabled
         frame_finished.Wait();
      }
   }


#if !defined(TARGET_NO_THREADS)
   // The previous frame is still being rendered. Before this, the new one was
   // simply dropped (unless SynchronousRendering caught it above) -- harmless
   // while emulation was slower than presentation, but once the idle
   // fast-forward (docs/tech_debits.md 4.14) made kofxi run at 100% speed it
   // threw away 25% of the frames the game rendered. Upstream flycast waits
   // here whenever the CPU keeps up (>= 85% speed over the last 4 vblanks,
   // AutoSkipFrame "normal") and only drops to catch up when it can't -- which
   // is what keeps slow games at the right game speed on this device.
   // FC_AUTOSKIP=2 restores the old always-drop behaviour for A/B.
   //
   // On top of upstream's rule: only wait if the render thread keeps up with
   // the game. Waiting can't make the render thread faster, so when it needs
   // more real time per frame than the game's own render interval (mslug6's
   // boss scene: ~34ms of Process+Render per frame against a 16.7ms render
   // interval), waiting just drags
   // the whole game down to its pace -- measured 99.7% -> 92.4% game speed.
   // Dropping, as before, keeps the game at speed there. Where it does keep up
   // (kofxi: ~6ms against 16.7ms) waiting costs nothing and removes the drops.
   {
      static int autoskip = -1;
      if (autoskip == -1)
      {
         const char *e = getenv("FC_AUTOSKIP");
         autoskip = e != nullptr ? atoi(e) : 1;
      }
      // The game's render interval in emulated cycles, smoothed.
      static u64 lastQueueCycles;
      u64 nowCycles = sh4_sched_now64();
      if (lastQueueCycles != 0)
      {
         u64 interval = nowCycles - lastQueueCycles;
         if (interval < SH4_MAIN_CLOCK / 2)
            g_rendIntervalCyclesEma = g_rendIntervalCyclesEma == 0 ? (u32)interval
                  : (u32)(((u64)g_rendIntervalCyclesEma * 15 + interval) / 16);
      }
      lastQueueCycles = nowCycles;

      extern bool SH4FastEnough;
      extern std::atomic<u32> g_rendWorkUsEma;
      u64 workCycles = (u64)g_rendWorkUsEma.load(std::memory_order_relaxed) * (SH4_MAIN_CLOCK / 1000000);
      // Exige FOLGA, nao so "cabe": com o render no limite (KOF Evolution na
      // chuva apos o glDrawRangeElements: 15,9ms de 16,7) a espera derrubava o
      // jogo de 100% para 92% de velocidade, porque a emu thread ja estava
      // saturada. So espera se o render usar ate `margin`% do intervalo do
      // jogo (kofxi/MBAA: ~6ms de 16,7, seguem esperando). FC_AUTOSKIP_MARGIN
      // ajusta (100 = regra antiga). docs/tech_debits.md 4.29.
      static int margin = -1;
      if (margin == -1)
      {
         const char *m = getenv("FC_AUTOSKIP_MARGIN");
         margin = m != nullptr ? atoi(m) : 75;
         if (margin <= 0 || margin > 100)
            margin = 75;
      }
      bool renderKeepsUp = workCycles != 0 && workCycles * 100 <= (u64)g_rendIntervalCyclesEma * margin;
      // Teto de frameskip: se a fracao descartada ja passou do teto, para de
      // descartar e espera o render -- a velocidade cai, a apresentacao nao
      // degrada mais. Dentro do teto, descarta como antes (velocidade primeiro).
      static int budgetEnv = -2;
      if (budgetEnv == -2)
      {
         const char *b = getenv("FC_SKIP_BUDGET");
         budgetEnv = b != nullptr ? atoi(b) : -1;
      }
      const int budgetPct = budgetEnv >= 0 ? budgetEnv : g_frameskipBudgetPct;
      const bool overBudget = dropRateEma * 100.f > (float)budgetPct;
      // Modelo novo: a emu nunca espera -- descarta e segue (o gate da fila
      // cuida de nao acumular). FC_EMU_WAIT=1 restaura a espera (A/B).
      static int emuWaitInit = 0;
      if (emuWaitInit == 0)
      {
         emuWaitInit = 1;
         const char *e = getenv("FC_EMU_WAIT");
         if (e != nullptr)
            g_emuNeverWaits = atoi(e) == 0;
      }
      if (!g_emuNeverWaits && rqueue && settings.rend.ThreadedRendering
            && (autoskip == 0 || (autoskip == 1 && SH4FastEnough && (renderKeepsUp || overBudget))))
      {
         g_queueWaits++;
         frame_finished.Wait();
      }
   }
#endif

	// Liberacao antecipada (Renderer_if.cpp): o slot pode estar livre com o
	// render ainda desenhando o frame anterior. So enfileira se ele estiver
	// perto de acabar (FC_EARLY_THRESHOLD_US, padrao 1000); senao descarta,
	// como quando o slot esta ocupado.
	{
		extern std::atomic<u64> g_rendBusyUntilUs;
		static int thresholdUs = -1;
		if (thresholdUs == -1)
		{
			const char *t = getenv("FC_EARLY_THRESHOLD_US");
			thresholdUs = t != nullptr ? atoi(t) : 1000;
		}
		u64 busy = g_rendBusyUntilUs.load(std::memory_order_relaxed);
		if (!rqueue && busy != 0)
		{
			u64 now = (u64)std::chrono::duration_cast<std::chrono::microseconds>(
					std::chrono::steady_clock::now().time_since_epoch()).count();
			if (busy > now + (u64)thresholdUs)
			{
				g_queueDrops++;
				g_queueBusyDrops++;
				pacerWinDrops++;
				budgetWinDrops++;
				lastFrameDropped = true;
				tactx_Recycle(ctx);
				return false;
			}
		}
	}
	if (rqueue)
   {
		// The previous frame hasn't been picked up by the render thread yet:
		// this one is thrown away. Counted because it's silent otherwise and
		// only happens once emulation runs ahead of presentation.
		g_queueDrops++;
		pacerWinDrops++;
		budgetWinDrops++;
		lastFrameDropped = true;
		tactx_Recycle(ctx);
		return false;
	}
	g_queueOk++;

   frame_finished.Reset();
   mtx_rqueue.lock();
	TA_context* old = rqueue;
	rqueue=ctx;
   mtx_rqueue.unlock();

   verify(!old);

	return true;
}

TA_context* DequeueRender(void)
{
   mtx_rqueue.lock();
	TA_context* rv = rqueue;
   mtx_rqueue.unlock();

	if (rv)
		FrameCount++;

	return rv;
}

bool rend_framePending(void)
{
   mtx_rqueue.lock();
	TA_context* rv = rqueue;
   mtx_rqueue.unlock();

	return rv != 0;
}

void FinishRender(TA_context* ctx)
{
	if (ctx != NULL)
	{
		verify(rqueue == ctx);
		mtx_rqueue.lock();
		rqueue = NULL;
		mtx_rqueue.unlock();

		tactx_Recycle(ctx);
	}
	frame_finished.Set();
}

static cMutex mtx_pool;

/* texture cache entry pool. */
static std::vector<TA_context*> ctx_pool;
static std::vector<TA_context*> ctx_list;

TA_context* tactx_Alloc(void)
{
	TA_context* rv = 0;

   mtx_pool.lock();
	if (!ctx_pool.empty())
	{
		rv = ctx_pool[ctx_pool.size()-1];
		ctx_pool.pop_back();
	}
   mtx_pool.unlock();
	
	if (!rv)
   {
      rv = new TA_context();
      rv->Alloc();
   }

   return rv;
}

void tactx_Recycle(TA_context* poped_ctx)
{
   mtx_pool.lock();
   if (ctx_pool.size()>2)
   {
      poped_ctx->Free();
      delete poped_ctx;
   }
   else
   {
      poped_ctx->Reset();
      ctx_pool.push_back(poped_ctx);
   }
   mtx_pool.unlock();
}

TA_context* tactx_Find(u32 addr, bool allocnew)
{
   for (size_t i=0; i<ctx_list.size(); i++)
   {
      if (ctx_list[i]->Address==addr)
         return ctx_list[i];
   }

   if (allocnew)
   {
      TA_context *rv = tactx_Alloc();
      rv->Address=addr;
      ctx_list.push_back(rv);

      return rv;
   }

   return 0;
}

TA_context* tactx_Pop(u32 addr)
{
	for (size_t i=0; i<ctx_list.size(); i++)
   {
      if (ctx_list[i]->Address == addr)
      {
         TA_context *rv = ctx_list[i];

         if (ta_ctx == rv)
            SetCurrentTARC(TACTX_NONE);

         ctx_list.erase(ctx_list.begin() + i);

         return rv;
      }
   }
	return 0;
}

const u32 NULL_CONTEXT = ~0u;

void SerializeTAContext(void **data, unsigned int *total_size)
{
	if (ta_ctx == nullptr)
	{
		LIBRETRO_S(NULL_CONTEXT);
		return;
	}
	LIBRETRO_S(ta_ctx->Address);
	const u32 taSize = ta_ctx->tad.thd_data - ta_ctx->tad.thd_root;
	LIBRETRO_S(taSize);
	LIBRETRO_SA(ta_ctx->tad.thd_root, taSize);

   LIBRETRO_S(ta_ctx->tad.render_pass_count);
	for (u32 i = 0; i < ta_ctx->tad.render_pass_count; i++)
	{
		u32 offset = (u32)(ta_ctx->tad.render_passes[i] - ta_ctx->tad.thd_root);
		LIBRETRO_S(offset);
	}
}

void UnserializeTAContext(void **data, unsigned int *total_size, serialize_version_enum version)
{
	u32 address;
	LIBRETRO_US(address);
	if (address == NULL_CONTEXT)
		return;
	SetCurrentTARC(address);
	u32 size;
	LIBRETRO_US(size);
	LIBRETRO_USA(ta_ctx->tad.thd_root, size);
	ta_ctx->tad.thd_data = ta_ctx->tad.thd_root + size;
   if (version >= V12)
	{
		LIBRETRO_US(ta_ctx->tad.render_pass_count);
		for (u32 i = 0; i < ta_ctx->tad.render_pass_count; i++)
		{
			u32 offset;
			LIBRETRO_US(offset);
			ta_ctx->tad.render_passes[i] = ta_ctx->tad.thd_root + offset;
		}
	}
	else
	{
		ta_ctx->tad.render_pass_count = 0;
	}
}
