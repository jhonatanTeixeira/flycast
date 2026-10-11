#include "types.h"
#include "emulator.h"

#include <libretro.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <atomic>

#define SAMPLE_COUNT 512

// FC_SYNC_STATS (Renderer_if.cpp): amostras geradas pelo AICA e entregues ao frontend,
// para conferir a VEL contada pelo frontend contra o relogio emulado
std::atomic<u64> g_audioGenerated{0}, g_audioDelivered{0};

extern retro_audio_sample_batch_t audio_batch_cb;

// FC_AUDIO_DUMP=arquivo: grava o PCM gerado (diagnostico, compara versoes do mixer)
static FILE *audioDump;
static int audioDumpInit;

// Carga de savestate: recomeca o arquivo (o audio do boot antes da carga
// tem duracao variavel). Chamado com o render de som ja drenado.
void audio_dump_restart()
{
   if (audioDump != nullptr)
   {
      fflush(audioDump);
      if (ftruncate(fileno(audioDump), 0) == 0)
         rewind(audioDump);
   }
}

void WriteSample(s16 r, s16 l)
{
   static SoundFrame Buffer[SAMPLE_COUNT];
   static u32 writePtr; // next sample index
   Buffer[writePtr].r = r;
   Buffer[writePtr].l = l;
   if (!audioDumpInit)
   {
      audioDumpInit = 1;
      const char *p = getenv("FC_AUDIO_DUMP");
      if (p != nullptr)
         audioDump = fopen(p, "wb");
   }
   if (audioDump != nullptr)
      fwrite(&Buffer[writePtr], sizeof(SoundFrame), 1, audioDump);

   if (++writePtr == SAMPLE_COUNT)
   {
      g_audioGenerated.fetch_add(SAMPLE_COUNT, std::memory_order_relaxed);
      if ( dc_is_running() && (!settings.rend.ThreadedRendering || settings.aica.LimitFPS) )
      {
         audio_batch_cb((const int16_t*)Buffer, SAMPLE_COUNT);
         g_audioDelivered.fetch_add(SAMPLE_COUNT, std::memory_order_relaxed);
      }
      writePtr = 0;
   }
}
