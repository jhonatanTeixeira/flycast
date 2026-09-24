// Replay da sequencia real de saidas dinamicas (dyn-*.bin do FC_JIT_DUMP) na
// tabela FPCB (128MB, paginas de 4KB sob demanda, como no flycast) e numa
// tabela compacta com os mesmos destinos. Carga dependente (como ldr+br) e,
// opcionalmente, K leituras de "RAM do jogo" entre saidas num buffer de W MB.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
static double now(){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static int cmpu(const void*a,const void*b){uint32_t x=*(uint32_t*)a,y=*(uint32_t*)b;return x<y?-1:x>y;}
int main(int argc,char**argv){
  FILE*f=fopen(argv[1],"rb"); fseek(f,0,2); size_t n=ftell(f)/4; fseek(f,0,0);
  uint32_t*pc=malloc(n*4); if(fread(pc,4,n,f)!=n) return 1; fclose(f);
  size_t N=n>8000000?8000000:n;
  // tabela real: 16M entradas x 8B
  uint64_t*big=mmap(0,(size_t)16<<20<<3,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE,-1,0);
  uint32_t*bi=malloc(N*4),*ci=malloc(N*4);
  // indices compactos
  uint32_t*s=malloc(N*4); memcpy(s,pc,N*4); qsort(s,N,4,cmpu); size_t u=0; for(size_t i=0;i<N;i++) if(!i||s[i]!=s[i-1]) s[u++]=s[i];
  for(size_t i=0;i<N;i++){ bi[i]=(pc[i]>>1)&((1u<<24)-1); big[bi[i]]=0;
    size_t lo=0,hi=u; while(lo<hi){size_t m=(lo+hi)/2; if(s[m]<pc[i]) lo=m+1; else hi=m;} ci[i]=lo; }
  uint64_t*small=calloc(u,8);
  printf("saidas %zu, destinos distintos %zu, tabela compacta %zu KB\n",N,u,u*8/1024);
  size_t W=(size_t)16<<20; uint64_t*ram=malloc(W); memset(ram,0,W);
  uint32_t rng=12345;
  int Ks[]={0,16}; size_t Ws[]={0,1<<20,4<<20,16<<20};
  for(int ki=0;ki<2;ki++) for(int wi=0;wi<4;wi++){
    int K=Ks[ki]; size_t ws=Ws[wi]; if((K==0)!=(ws==0)) continue;
    size_t mask=ws?ws/8-1:0;
    for(int mode=0;mode<2;mode++){
      uint64_t*t=mode?small:big; uint32_t*ix=mode?ci:bi; uint64_t x=0,acc=0; double best=1e9;
      for(int r=0;r<3;r++){ double t0=now(); rng=12345;
        for(size_t i=0;i<N;i++){ x=t[ix[i]+x];
          for(int k=0;k<K;k++){ rng=rng*1664525u+1013904223u; acc+=ram[(rng>>3)&mask]; } }
        double dt=now()-t0; if(dt<best) best=dt; }
      printf("  %-9s K=%2d W=%5zuKB: %6.1f ns por saida%s\n",mode?"compacta":"FPCB",K,ws/1024,best/N*1e9,(int)(acc+x)==-1?"!":"");
    }
  }
  return 0;
}
