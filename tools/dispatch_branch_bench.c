// Custo do desvio indireto do despacho: stubs "ret" nos offsets reais do
// codigo JIT de cada destino (dyn_off.bin) e blr pela sequencia real.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
static double now(){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static double run(uint64_t *tg, size_t n){
  double best=1e9;
  for(int r=0;r<3;r++){ double t0=now();
    for(size_t i=0;i<n;i++){ void (*fn)(void)=(void(*)(void))tg[i]; __asm__ volatile("blr %0"::"r"(fn):"x30","memory"); }
    double dt=now()-t0; if(dt<best) best=dt; }
  return best/n*1e9;
}
int main(int argc,char**argv){
  FILE*f=fopen(argv[1],"rb"); fseek(f,0,2); size_t n=ftell(f)/4; fseek(f,0,0);
  uint32_t*off=malloc(n*4); if(fread(off,4,n,f)!=n) return 1; fclose(f);
  size_t SZ=16<<20;
  uint8_t*c=mmap(0,SZ,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  uint32_t RET=0xd65f03c0;
  for(size_t i=0;i<n;i++) memcpy(c+(off[i]&~3u),&RET,4);
  __builtin___clear_cache((char*)c,(char*)c+SZ);
  uint64_t*tg=malloc(n*8);
  for(size_t i=0;i<n;i++) tg[i]=(uint64_t)(c+(off[i]&~3u));
  double real=run(tg,n);
  // mesmo destino sempre (previsao perfeita, 1 linha de codigo)
  uint64_t*one=malloc(n*8); for(size_t i=0;i<n;i++) one[i]=tg[0];
  double same=run(one,n);
  // destinos reais, mas compactos (64B entre eles): separa layout de previsao
  uint32_t*uq=malloc(n*4); size_t u=0;
  uint8_t*c2=mmap(0,SZ,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
  uint64_t*tc=malloc(n*8);
  // mapa offset->slot compacto (hash simples)
  size_t HS=1<<20; uint32_t*hk=calloc(HS,4),*hv=calloc(HS,4);
  for(size_t i=0;i<n;i++){ uint32_t k=off[i]+1; size_t h=(k*2654435761u)&(HS-1); while(hk[h]&&hk[h]!=k) h=(h+1)&(HS-1);
    if(!hk[h]){hk[h]=k;hv[h]=u++; memcpy(c2+hv[h]*64,&RET,4);} tc[i]=(uint64_t)(c2+hv[h]*64); }
  __builtin___clear_cache((char*)c2,(char*)c2+SZ);
  double compact=run(tc,n);
  printf("saidas %zu, destinos distintos %zu\n",n,u);
  printf("  destino fixo (previsao perfeita):        %6.1f ns por saida\n",same);
  printf("  destinos reais, codigo compacto (64B):   %6.1f ns por saida\n",compact);
  printf("  destinos reais, layout real do JIT:      %6.1f ns por saida\n",real);
  return 0;
}
