// Harness do prototipo: roda o laco de vertices do Shenmue II (estado real
// capturado na entrada de 8C1D8BDA) com o codigo do JIT atual (cur.S) e com a
// versao jit_armv8_a escrita a mao (new.S); confere que o resultado e
// identico (registradores, fila SQ, dados enviados ao "TA") e cronometra.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
extern uint32_t cur_run(void *ctx, uint32_t cycles);
extern uint32_t new_run(void *ctx, uint32_t cycles);
extern void sq_stub(void);
extern uint64_t g_membase, g_sink;
static double now(){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static uint8_t cap[448], ctxA[448], ctxB[448];
static uint8_t sinkA[1<<16], sinkB[1<<16];
int main(int argc,char**argv){
  const char*dir=argc>1?argv[1]:".";
  char p[512]; snprintf(p,sizeof p,"%s/cap-8C1D8BDA-ctx.bin",dir); FILE*f=fopen(p,"rb"); if(!f||fread(cap,1,448,f)!=448){puts("ctx?");return 1;} fclose(f);
  size_t RES=0x100000000ull+0x20000;
  uint8_t*reg=mmap(0,RES,PROT_NONE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE,-1,0);
  uint8_t*base=reg+0x10000; uint8_t*ctx=base-0x1c0;
  mprotect(reg,0x10000,PROT_READ|PROT_WRITE);
  int fd=memfd_create("ram",0); ftruncate(fd,16<<20);
  uint32_t mir[]={0x0C,0x0D,0x0E,0x0F,0x8C,0x8D,0x8E,0x8F,0xAC,0xAD,0xAE,0xAF};
  for(int i=0;i<12;i++) if(mmap(base+((uint64_t)mir[i]<<24),16<<20,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_FIXED,fd,0)==MAP_FAILED){puts("mmap ram");return 1;}
  uint8_t*sq=base+0xE0000000ull; if(mmap(sq,64<<20,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED|MAP_NORESERVE,-1,0)==MAP_FAILED){puts("mmap sq");return 1;}
  snprintf(p,sizeof p,"%s/cap-8C1D8BDA-ram.bin",dir); f=fopen(p,"rb"); uint8_t*ram=base+0x0C000000ull; if(!f||fread(ram,1,16<<20,f)!=(16u<<20)){puts("ram?");return 1;} fclose(f);
  *(void**)(ctx-0x48)=(void*)sq_stub; g_membase=(uint64_t)base;
  uint32_t r6=*(uint32_t*)(cap+128+6*4);
  uint8_t*sqwin=sq+((r6&0x03FFFFFF)&~0xFFFFu)-0x10000; size_t sqlen=0x30000;
  // 1) equivalencia
  static uint8_t sqA[0x30000], sqB[0x30000];
  memset(sqwin,0,sqlen); memcpy(ctx,cap,448); g_sink=(uint64_t)sinkA; uint32_t eA=cur_run(ctx,1u<<30); memcpy(ctxA,ctx,448); size_t nA=g_sink-(uint64_t)sinkA; memcpy(sqA,sqwin,sqlen);
  memset(sqwin,0,sqlen); memcpy(ctx,cap,448); g_sink=(uint64_t)sinkB; uint32_t eB=new_run(ctx,1u<<30); memcpy(ctxB,ctx,448); size_t nB=g_sink-(uint64_t)sinkB; memcpy(sqB,sqwin,sqlen);
  printf("saida: atual %08X  nova %08X | bytes enviados ao TA: %zu x %zu\n",eA,eB,nA,nB);
  int bad=0;
  for(int i=0;i<16;i++){ uint32_t a=*(uint32_t*)(ctxA+128+4*i), b=*(uint32_t*)(ctxB+128+4*i); if(a!=b){printf("  r%d difere: %08X x %08X\n",i,a,b);bad++;} }
  for(int i=0;i<32;i++){ uint32_t a=*(uint32_t*)(ctxA+4*i), b=*(uint32_t*)(ctxB+4*i); if(a!=b){printf("  %s%d difere: %08X x %08X\n",i<16?"xf":"fr",i&15,a,b);bad++;} }
  if(*(uint32_t*)(ctxA+260)!=*(uint32_t*)(ctxB+260)){printf("  fpul difere\n");bad++;}
  if(*(uint32_t*)(ctxA+276)!=*(uint32_t*)(ctxB+276)){printf("  T difere: %u x %u\n",*(uint32_t*)(ctxA+276),*(uint32_t*)(ctxB+276));bad++;}
  if(nA!=nB||memcmp(sinkA,sinkB,nA)){printf("  dados do TA diferem\n");bad++;}
  if(memcmp(sqA,sqB,sqlen)){printf("  memoria da SQ difere\n");bad++;}
  uint32_t r3=*(uint32_t*)(cap+128+3*4);
  printf("equivalencia: %s (r3 inicial %u vertices, %zu pref p/ TA)\n",bad?"FALHOU":"IDENTICO",r3,nA/32);
  if(bad) return 2;
  // 2) tempo (estado restaurado a cada rodada; laco e dados quentes no cache)
  int N=argc>2?atoi(argv[2]):200000; double tA=1e9,tB=1e9;
  for(int rep=0;rep<5;rep++){
    double t0=now(); for(int i=0;i<N;i++){ memcpy(ctx,cap,448); g_sink=(uint64_t)sinkA; cur_run(ctx,1u<<30);} double d=now()-t0; if(d<tA)tA=d;
    t0=now(); for(int i=0;i<N;i++){ memcpy(ctx,cap,448); g_sink=(uint64_t)sinkB; new_run(ctx,1u<<30);} d=now()-t0; if(d<tB)tB=d;
  }
  double t0=now(); for(int i=0;i<N;i++){ memcpy(ctx,cap,448); } double tc=now()-t0;
  printf("por strip (%u vertices): atual %.1f ns | nova %.1f ns | copia do estado %.1f ns\n",r3,(tA-tc)/N*1e9,(tB-tc)/N*1e9,tc/N*1e9);
  printf("ganho: %.2fx\n",(tA-tc)/(tB-tc));
  return 0;
}
