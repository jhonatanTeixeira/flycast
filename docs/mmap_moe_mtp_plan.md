# Plano: mmap, MoE e MTP no flycast (dual core do som à la Dolphin)

## Contexto

A emulação (thread do SH4, núcleo 3) é o limite nos jogos pesados: Shenmue II a 73,5% com
DSP desligado (~55% com DSP ligado), DOA2 a 84%. Medido hoje:
- ~10 M faltas de L1I por segundo emulado (~7-9% do tempo esperando código, 4.127);
- limpar o cache de código custa 120-140 ms (e recompilar ~0,2-0,5 ms por bloco). A D3 que fiz
  (limpar + recompilar os quentes) travava 0,15-1 s e mudava as fronteiras dos blocos, então
  o estado divergia. Contrariou o plano original, que é promover bloco a bloco sem limpar;
- o som ainda roda na thread da emulação: ARM7 ~2,9 ms + controle de vozes ~5,2 ms por frame
  (Shenmue II). **Com o DSP ligado, o core liga `NoBatch=1`** (`core/libretro/libretro.cpp:1117-1133`)
  e a mixagem completa + DSP voltam para a thread da emulação, amostra por amostra
  (`AICA_Sample`, `core/hw/aica/sgc_if.cpp:1874`): ~23 ms/frame (4.122). É por isso que o
  DSP ligado "derruba" o Shenmue II, e o usuário quer o DSP ligado (som estourado sem ele).

O usuário pediu técnicas análogas a três do mundo de LLM:
- **mmap**: não jogar fora o que já foi compilado;
- **MoE**: só a parte quente na memória rápida;
- **MTP / dual core do Dolphin**: adiantar trabalho em paralelo e sincronizar.

Decisões do usuário: ordem "ganhos rápidos primeiro". O dual core pode deixar de ser idêntico
bit a bit ao core atual; ele é validado por auto-consistência (2 rodadas iguais entre si) +
ouvir o som + A/B.

Cache de código em disco (o mmap "entre sessões") **fica fora**: o código gerado embute
ponteiros absolutos que mudam a cada execução (ASLR, `virt_ram_base`, contexto) e decisões de
runtime (ro/fold/alias). Não é viável sem uma tabela de relocação, e o custo que ele salvaria
nunca foi medido.

Regras de sempre:
- implementar e depois medir (sem rodada prévia, memória `implement-then-ab`);
- 2 rodadas anunciadas, partindo da mesma temperatura (`tools/rtabt.sh`, < 63 °C);
- **cfg de debug recopiado do oficial** (`/home/ark/r_cur_*.cfg`);
- VEL junto com frames novos e descartes; contadores da thread da emulação com `tools/rtpmu.sh`;
- estado validado com `tools/sthash.sh` + `tools/sthcmp.py` (alinha por ciclo emulado);
- commit a cada marco; docs (`history.md`, `tech_debits.md`, `game_status.md`);
- nada de pôr a culpa no hardware.

Observação: a D3 por limpeza ainda está **não commitada** no working tree (`driver.cpp`
`jit_hot_*`, amostragem no stub `intc_sched` em `rec_arm64.cpp`) e ligada por padrão. Até a
fase 2 ela fica desligada por padrão; nenhum commit da fase 1 a leva ligada.

---

## Fase 1: ganhos rápidos

### 1a. DSP no caminho em lote (MTP leve: o DSP vai para a thread de mixagem que já existe)

Hoje, com DSP ligado: `NoBatch=1` → `AicaUpdate` pula o `AICA_Sample32` (`aica.cpp:109`) e o
`libAICA_TimeStep` chama `AICA_Sample()` a cada amostra (`aica.cpp:130`). O caminho em lote
(4.44: `AICA_Sample32` → controle na emu → `aica_block_publish` → `aica_render_worker`,
`sgc_if.cpp:1552-1800`) já calcula `oDsp` por voz (`sgc_if.cpp:1638`) e o descarta.

O que fazer:
- No worker (`aica_render_block`), por amostra: acumular `oDsp` em `dsp.MIXS[ISEL]` (a
  seleção de envio da voz), rodar `dsp_step()` e somar a saída EFREG (com EFSDL/EFPAN) e o CDDA
  na mixagem final. Mesma conta do `AICA_Sample()` (`sgc_if.cpp:1874-1959`), reaproveitada.
- **Estado do DSP por bloco:** o SH4 e o ARM7 escrevem programa/coeficientes
  (`dsp_writenmem`, `WriteCommonReg8` RBP/RBL, `sgc_if.cpp:1536-1544`). Uma cópia vai junto
  no `RBlock` só quando muda (contador de versão), e o worker aplica antes de mixar o bloco.
  A recompilação do programa do DSP (`dyndirty`, `dsp_arm64.cpp:462`) passa a ser feita no
  worker.
- **ARAM do DSP** (buffer circular MEMS/MWT em RBP/RBL): quem escreve passa a ser o worker.
  - `FC_STATE_HASH` e o savestate drenam a fila antes (`aica_mix_sync`, que já existe; somar
    a chamada no hash, `Renderer_if.cpp` ~1153).
  - Corrida aceita: SH4/ARM7 lendo essa área do buffer do DSP enquanto o worker escreve. Os
    jogos não leem o eco.
- `libretro.cpp:1117-1133`: DSP ligado deixa de forçar `NoBatch=1`. O `NoBatch` continua
  disponível para A/B.
- CDDA: hoje `libCore_CDDA_Sector` é chamado no caminho por amostra. No lote, o setor do bloco
  é lido na thread da emulação (no `AICA_Sample32`) e vai junto no bloco.

Validação:
- `FC_AUDIO_DUMP` do PCM: NoBatch × lote com DSP, no Shenmue II. Esperado quase igual (o
  controle das vozes já é por bloco de 32 amostras no lote);
- o usuário ouve;
- `FC_STATE_HASH` com DSP em lote: duas rodadas iguais entre si.

A/B: Shenmue II com **DSP ligado** (cópia do `r_cur_shenmue2.cfg` com DSP ligado), NoBatch ×
lote. Esperado: dos ~23 ms/frame de som, ~15 ms saem da thread da emulação.

### 1b. mmap na sessão: parar de limpar o cache inteiro por uma falha de checagem

`rdv_BlockCheckFail` sem MMU (`driver.cpp` ~457-461) chama `recSh4_ClearCache()` em
**qualquer** falha de checagem de código. Cada uma custa 120-140 ms mais a recompilação de
tudo, aos poucos (engasgos).

O que fazer:
- Antes de mudar, contar as limpezas: um contador e o motivo no `FC_SYNC_STATS`
  (BlockCheckFail, cache cheio, PCs de boot, load de savestate).
- Correção: descartar só os blocos da página do código que mudou, com a invalidação por página
  que já existe (`blocks_per_page`/`bm_DiscardBlock`, `blockmanager.cpp:284-312`, como o caminho
  MMU faz). O resto do cache fica mapeado.
- Se as limpezas restantes ainda pesarem, ver por que a limpeza custa 120 ms: `bm_ResetCache`
  faz `Relink` + `Discard` de todos os blocos (`blockmanager.cpp:450-500`).

Validação:
- estado: duas rodadas iguais entre si;
- contagem de limpezas antes e depois nos jogos medidos;
- frame time p99 nos jogos com limpezas.

---

## Fase 2: MoE, a arena quente bloco a bloco (D3 do jeito do `docs/jit_hot_path.md`)

Substitui a D3 por limpeza. Fica a amostragem (stub `intc_sched` → `jit_hot_sample`, 1 a cada
64 fatias, anel de 16 K). Sai o `jit_hot_relayout` por `recSh4_ClearCache`.

- **Arena:** o `TempCodeCache` (1 MB, `CodeCache + CODE_SIZE`). Sem MMU ele fica ocioso (o
  `smc_hotspots` só é preenchido no ramo MMU) e está ao alcance de `bl` do runtime.
- **Promoção** num ponto seguro (`rdv_DoInterrupts`), com orçamento de ~1 ms ou poucos blocos
  por chamada, na ordem quente-primeiro e com os sucessores quentes logo depois:
  1. pré-condições:
     - `!mmu_enabled()`;
     - bloco vivo e não temporário;
     - não `(has_fpu_op && sr.FD)`, senão o decode levanta exceção (`decoder.cpp:1410`);
     - arena com ≥ 16 KB livres.
  2. guardar `vaddr`, `fpu_cfg`, `blockcheck_failures`;
  3. `bm_DiscardBlock(old)` **antes** de compilar a cópia. Com `NO_VERIFY`, a ordem inversa
     zera a entrada nova da FPCB;
  4. apontar o emissor para a arena (`emit_ptr`/`emit_ptr_limit`, restaurados ao sair), usando
     uma variante de `compile_block` sem o epílogo do `TempLastAddr`;
  5. `bm_AddBlock`; ligações preguiçosas pelo `rdv_LinkBlock`, que já existe.
- **Quando promover:** fração de amostras fora da arena > 15%. A primeira promoção acontece
  depois de ~5 s de amostras.
- **Arena cheia:** é uma época. Descartar todos os blocos da arena (eles voltam ao cache
  principal sob demanda) e rebobinar depois do próximo `bm_Periodical_1s`, que limpa
  `del_blocks`. O `recSh4_ClearCache` também zera a arena.
- **Acessos compactos** (`RewriteCompactMem`): a cópia nova recomeça sem os trampolins. Cada
  acesso gera uma falha de página uma vez e fica certo depois.

Validação:
- `FC_STATE_HASH` **idêntico** a `FC_JIT_HOT=0`. Nada é limpo e o bloco é recompilado no mesmo
  endereço com o mesmo `fpu_cfg`, então as fronteiras não mudam. DOA2, Napple, Shenmue II;
- sem engasgo: tempo de promoção por chamada no log.

A/B: Shenmue II e DOA2, `rtpmu.sh` com faltas de L1I e ciclos por segundo emulado + VEL.
Alvo: cortar a maior parte dos ~10 M/s de faltas de L1I.

---

## Fase 3: MTP / dual core, ARM7 + controle de vozes numa thread própria (SyncGPU do Dolphin)

Teto: ~8 ms/frame no Shenmue II (sem DSP). Mapa completo de pontos de sincronia: S1-S14 do
levantamento (`aica_if.cpp`, `aica_mem.cpp`, `aica.cpp`, `sb_mem.cpp`, `_vmem.cpp`).

### Desenho: o som roda 1 quantum (32 amostras, 0,73 ms emulados) adiantado

- **`AicaUpdate`** (`aica.cpp:103`) no instante T_k:
  1. espera o quantum k terminar;
  2. aplica as saídas na thread da emulação: estado do IRQ do SPU para o SH4 via
     `asic_Raise/CancelInterrupt` (só nesta thread), DMA interna, consumo de CDDA;
  3. publica um **retrato dos registradores de monitor** (CA/EG/LP/KYONB, timers, SCIPD/MCIPD);
  4. dispara o quantum k+1 na thread do som (`aicaarm::run(32)` + controle + produção do
     `RBlock`).
- **Leituras do SH4 nos registradores do AICA** saem do retrato de T_k, sem esperar. Foi a
  espera nessas leituras (~60/frame) que zerou a 1ª tentativa (4.44).
- **Escritas do SH4** (registradores, KEY_ON, SCIPD/MCIRE, timers) entram numa fila aplicada no
  início do próximo quantum. Atraso ≤ 1 quantum, determinístico.
- **Drenagem obrigatória** (espera o quantum em voo):
  - ARMRST/VREG (`aicaarm::enable`/`reset`, `FlushCache`);
  - G2-DMA com destino em ARAM (`Write_SB_ADST`, `aica_if.cpp:340-405`);
  - DMA interna disparada pelo SH4;
  - acesso a ARAM pelo handler (`sb_mem.cpp:446/540`);
  - HLE `reios`;
  - savestate (`dc_serialize`), `FC_STATE_HASH`, reset/term, mudança de `settings.aica`.
- **ARAM pelo fastmem do SH4** (S6, sem gancho): é o ponto aberto.
  - **3.0 (portão):** medir quantas vezes o SH4 toca ARAM por frame nos jogos-alvo. Num modo
    de diagnóstico, a visão do SH4 passa pelo handler e cada acesso é contado. O 4.45 disse
    "~0" no Shenmue II, DOA2, Shenmue e MBAA.
  - Se ~0: aceitar a corrida. Senão: nesse modo, a visão do SH4 vai pelo handler que drena
    (como no modo 512 MB, `_vmem.cpp:489`).
  - **Resultado (2026-10-11, `FC_ARAM_COUNT=1` + `sh4_aica_por_s_emulado` no `FC_SYNC_STATS`):**
    registradores do AICA pelo SH4 = **0** em DOA2, MvC2, Shenmue II, Napple (os ~60/frame do
    4.44 eram o ARM7). ARAM: DOA2 44.298 escritas e 62 leituras por segundo emulado; MvC2
    44.316 / 192; Shenmue II 0; Napple pouco. **Desenho ajustado:** no modo dual core a visão
    da ARAM do SH4 passa pelo handler (como no diagnóstico); as escritas vão para uma fila
    aplicada pela thread do som no início do quantum seguinte (determinístico, sem espera); as
    leituras drenam (esperam o quantum em voo), ~1-3% de custo estimado.
- **Thread e núcleo:** a thread do som produz os `RBlock` e funde com o worker de mixagem do
  4.44. Cuidado com o achado: o worker hoje pode nascer depois do pin e herdar o núcleo 3,
  disputando com o SH4. Primeiro no núcleo 0 (main/apresentação); medir.
  - Sem espera ativa: espera em futex, regra do `thread_separation_plan` §6.
- **`FC_AICA_DUAL=0`** volta ao som síncrono (A/B).

Validação:
- 2 rodadas do modo novo com `FC_STATE_HASH` iguais entre si;
- `FC_AUDIO_DUMP` comparado ao modo síncrono (diferenças só de ≤ 1 quantum);
- o usuário ouve Shenmue II, MvC2, DOA2 (vozes, música, efeitos).

A/B: Shenmue II (DSP desligado e ligado), DOA2, MvC2. Contar as drenagens por frame: se forem
muitas, o ganho some, como na 1ª tentativa.

---

## Arquivos principais

- **Fase 1a:** `core/hw/aica/sgc_if.cpp` (worker, `AICA_Sample32`, `RBlock`),
  `core/hw/aica/dsp*.cpp`, `core/hw/aica/aica.cpp`, `core/libretro/libretro.cpp` (NoBatch),
  `core/hw/pvr/Renderer_if.cpp` (drenagem antes do hash).
- **Fase 1b:** `core/hw/sh4/dyna/driver.cpp` (`rdv_BlockCheckFail`, contador de limpezas),
  `core/hw/sh4/dyna/blockmanager.cpp`.
- **Fase 2:** `core/hw/sh4/dyna/driver.cpp` (`jit_hot_*`, promoção), `core/rec-ARM64/rec_arm64.cpp`
  (amostragem já feita), `blockmanager.cpp` (sequência discard/add).
- **Fase 3:** `core/hw/aica/aica.cpp`, `aica_if.cpp`, `aica_mem.cpp`, `sgc_if.cpp`,
  `core/hw/arm7/*`, `core/hw/holly/sb_mem.cpp`, `core/hw/mem/_vmem.cpp` (portão 3.0),
  `core/serialize.cpp`.
- **Docs:** novo `docs/mmap_moe_mtp_plan.md` (este plano); `current_plan.md` (D3 refeita),
  `tech_debits.md` (4.122, 4.127, novo item para as limpezas de cache), `history.md`,
  `game_status.md`.

## Verificação ponta a ponta (cada marco)

1. Build com as flags do CLAUDE.md (`-j2`; header amplo → `make clean` com os mesmos
   argumentos).
2. Estado:
   - fase 2: idêntico ao modo desligado;
   - fases 1a, 1b e 3: 2 rodadas iguais entre si.

   `tools/sthash.sh` + `tools/sthcmp.py`, cfgs `r_cur_*`.
3. Som (1a, 3): `FC_AUDIO_DUMP` comparado e o usuário ouve.
4. A/B: 2 rodadas anunciadas, `tools/rtabt.sh` (esfria < 63 °C). Tabela com VEL, fps novos,
   descartes, frame p50/p95/p99, underruns; `tools/rtpmu.sh` quando o alvo é a thread da
   emulação.
5. Commit do marco + docs. Pergunta aberta ao usuário sobre o que viu e ouviu na tela.
