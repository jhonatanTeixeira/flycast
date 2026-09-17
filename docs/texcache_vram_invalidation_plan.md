# Invalidação de textura por página de VRAM — diagnóstico e plano de correção

> Alvo: Metal Slug 6 (Naomi/Atomiswave), savestate pesado do usuário
> (`/roms2/naomi/mslug6.fc2021-rrstate.auto`). Todos os números medidos no
> device (R36/RK3326) em 2026-09-16, 30s por rodada, cena idêntica garantida
> por savestate. Instrumentação em `b509c7ee7`.
>
> **AVISO: o diagnóstico das seções iniciais foi REFUTADO por medição
> posterior.** Leia primeiro a seção "CORREÇÃO" no fim do documento: as
> texturas não estavam sendo invalidadas à toa, o conteúdo muda de verdade, e
> o ganho de +63% da tentativa 1 vinha de pular trabalho necessário. O texto
> original fica preservado porque o caminho do erro é parte do registro.

## Resumo

O gargalo do Metal Slug 6 **não é CPU**. É re-upload desnecessário de textura:
~394 texturas reconvertidas e reenviadas pra GPU **todo frame**, consumindo
~40ms de um frame de ~63ms. A causa não é o jogo reescrevendo textura — é a
proteção/invalidação de VRAM funcionar por **página de 4KB inteira**, de modo
que cada escrita do jogo derruba em média **29,88 texturas**, quase todas
intactas.

## Como chegamos aqui (cadeia de medição)

Cada etapa respondeu à anterior. O caminho importa porque duas hipóteses
intermediárias — inclusive minhas — foram refutadas pelo dado.

| # | Instrumento | Resultado |
|---|---|---|
| 1 | `FC_BLOCK_PROF` (novo) | 4 blocos JIT = **74,6%** das instruções host: spin loop em `8C05B3A4`/`8C05B3D0`/`8C05B68E`/`8C05B3D6` |
| 2 | `FC_WATCH_ADDR` (novo) | O flag esperado (`0x8C386928`) alterna 0↔1 a cada 1-2 frames — escritor OK, **idle loop legítimo, não bug** |
| 3 | `FC_IDLE_OPS`/`FC_IDLE_MUL` | Instruções host **26,5G→13,7G (-48%)** e **fps inalterado** (14,79→14,46) |
| 4 | `FC_REND_SPLIT` (novo) | `rsWait` 8,7ms · **`Process` 56,8ms** · `render` 17,4ms |
| 5 | `FC_TA_SPLIT` (novo) | Dentro do `Process`: decode 42,1ms, e dentro dele **`GetTexture` 41,2ms (98%)** — 558 chamadas/frame, **74,8µs cada** |
| 6 | Contadores de textura | **miss 70,1%**, **17,65s dos 30s** dentro de `tf->Update()` |
| 7 | Split da causa do miss | `miss_vram_dirty` **164.717** · `miss_palette` **7** · `miss_both` 11.843 |
| 8 | Contadores de VRAM | faults **5.899** (~13/frame) · invalidações **176.299** (~398/frame) ⇒ **29,88 texturas por escrita** |

### Duas leituras erradas que o dado corrigiu

- **"O laço gira mais do que o SH4 real conseguiria"** (minha afirmação):
  errado. 423k iterações/frame × ~15 ciclos ≈ 6,3M ciclos ≈ exatamente um
  frame a 200MHz. Cabe no orçamento; é espera legítima.
- **"O `perf` mostra 53,9% em `SH4_TCB`, logo o JIT é o gargalo"**: o `perf`
  mostra **consumo de CPU somado entre threads**, não caminho crítico. A
  `emu_thread` de fato queima CPU no spin loop, mas termina antes da main
  thread precisar dela — por isso `rsWait` é só 8,7ms. A prova é o passo 3:
  metade do trabalho do JIT eliminado, zero efeito no fps.
- **"Jogo 2D animando paleta invalida as texturas paletizadas"** (hipótese
  minha, plausível e falsa): 7 misses de paleta em 30 segundos.

## O mecanismo atual

Proteção (`TexCache.cpp`, `vramlock_list_add`): cada `vram_block` cobre o
intervalo real da textura (`start`..`end`), mas é registrado em **todas as
páginas** que ele cruza, e a proteção é aplicada por página inteira:

```c
u32 base = block->start / PAGE_SIZE;
u32 end  = block->end   / PAGE_SIZE;
for (u32 i = base; i <= end; i++) {
    ...
    _vmem_protect_vram(i * PAGE_SIZE, PAGE_SIZE);   // 4KB de cada vez
}
```

Invalidação (`VramLockedWriteOffset`), no fault de escrita:

```c
size_t addr_hash = offset / PAGE_SIZE;
std::vector<vram_block*>& list = VramLocks[addr_hash];
for (auto& lock : list)
    if (lock != nullptr)
        rend_text_invl(lock);        // <<< invalida TODAS da página
list.clear();
_vmem_unprotect_vram(offset & ~PAGE_MASK, PAGE_SIZE);
```

**O ponto central:** a função **recebe o `offset` exato da escrita** e cada
`vram_block` **já conhece seu intervalo exato** (`start`/`end`). A informação
necessária pra ser precisa está toda ali — ela simplesmente não é usada. Com
`PAGE_SIZE = 4096` (`core/stdclass.h:12`) e sprites pequenos de jogo 2D,
dezenas de texturas dividem a mesma página.

## Plano de correção

### Parte 1 — invalidar só quem foi realmente escrito (fácil)

```c
for (auto& lock : list)
    if (lock != nullptr && offset >= lock->start && offset <= lock->end)
        rend_text_invl(lock);
```

As sobreviventes permanecem na lista em vez de `list.clear()`.

### Parte 2 — reproteger a página (é aqui que está o trabalho, e o risco)

A Parte 1 sozinha **está errada e causa bug visual silencioso**. Motivo:
depois do fault a função é obrigada a chamar `_vmem_unprotect_vram(página)`,
senão a escrita que faltou nunca completa e refaulta pra sempre. Se as
sobreviventes continuam vivas numa página agora **desprotegida**, uma escrita
futura nelas não gera fault nenhum — o jogo passa a exibir textura velha.
Corrupção intermitente, difícil de rastrear, exatamente a classe de bug que
já nos custou o speedhack de skip de Translucent (ver `tech_debits.md` 5.2).

Então é preciso **reproteger a página depois que a escrita completar**, num
ponto seguro. Esboço:

1. Ao desproteger, registrar a página numa lista de "pendentes de reproteção"
   (as sobreviventes seguem em `VramLocks[página]`).
2. Num ponto de sincronização por frame — candidato natural: início do
   `ta_parse_vdrc()`/`ProcessFrame()`, que já roda uma vez por frame e já
   sincroniza com a `emu_thread` via `rend_inuse` — percorrer a lista e
   rechamar `_vmem_protect_vram()` nas páginas que ainda têm bloco vivo.
3. Limpar a lista.

Consequência: mais faults por frame (a página volta a ser vigiada), mas hoje
são só ~13/frame. Mesmo subindo uma ordem de grandeza, cada fault evitado de
invalidação em massa economiza ~30 uploads × ~45µs ≈ **1,35ms**. A troca é
amplamente favorável.

## Ganho estimado e como validar

Se a razão de 29,88 cair para perto de 1, as invalidações caem de ~398 para
~13 por frame, e o tempo de `Update()` de ~40ms/frame para algo na casa de
1-2ms. Isso é potencialmente **metade do frame** do Metal Slug 6.

**Validação obrigatória, nesta ordem:**

1. Contadores já existentes (`FC_TA_SPLIT`): `tex_killed_per_write` deve cair
   para ~1, `tex_miss_pct` despencar, `tex_update_us_total` idem.
2. **Inspeção visual pelo usuário** — este é o critério que manda. Nenhum
   contador detecta textura velha na tela. Testar em mslug6, kofnw, mbaa e
   Shenmue, procurando sprite/cenário com textura errada ou "congelada".
3. A/B de performance com savestate (2 rodadas por lado, frame time + fps em
   p50/p95/p99 + média, conforme regra do `CLAUDE.md`).

**Risco:** MÉDIO-ALTO na correção (bug visual silencioso se a reproteção
falhar em algum caminho), ALTO no retorno. A Parte 1 sem a Parte 2 é rápida,
tentadora e **errada** — registrar aqui pra não ser "otimizada" depois por
quem só ler o resumo.

## Estado

Diagnóstico completo e commitado (`b509c7ee7`). Implementação **não iniciada**
— aguardando decisão. Toda a instrumentação necessária pra validar já está no
código e é opt-in.

---

## Tentativa 1 (2026-09-16): precisa + reproteção no frame seguinte — GANHO ENORME, MAS QUEBRA VISUALMENTE

Implementada como opt-in (`FC_TEX_PRECISE_INVL`, **default OFF**), exatamente
as duas partes do plano acima.

### Os números confirmam o diagnóstico e o tamanho do prêmio

| | OFF | ON |
|---|---|---|
| texturas mortas por escrita | 29,83 | **1,01** |
| miss rate | 76,7% | **5,1%** |
| invalidações (30s) | 158.996 | **10.967** (-93%) |
| tempo de textura/frame | 58,06 ms | **17,86 ms** |
| `core_average` | 62,08 ms | **33,63 ms** (-46%) |
| **fps** | **14,23** | **23,23** (**+63%**) |

Write faults dobraram (5.329→10.819), exatamente o custo previsto, irrelevante
perto do que economiza. A reproteção rodou (10.816 páginas com sobreviventes,
10.803 reprotegidas).

### Mas quebra a imagem — e o usuário viu em minutos

Relato: mslug6 virou "sopa de sprites, todos desconexos pela tela"; kofnw com
"bonecos embaralhados como quebra-cabeça mal montado"; mbaa **não** quebrou os
sprites. Isso é textura **obsoleta** (o jogo reusa a mesma região de VRAM para
sprites diferentes e quem não foi invalidado segue mostrando o anterior), não
corrupção de dados.

### Causa: a Parte 1 está certa, a Parte 2 tem janela grande demais

A razão de **1,01 invalidação por fault** prova que o teste de intervalo
funciona — quase todo fault acha exatamente a textura certa. O furo é o
intervalo de tempo:

1. Fault na página P → invalida só a textura A → **desprotege P**
2. **Ainda no mesmo frame**, o jogo escreve na textura B, também em P →
   **nenhum fault**, P está desprotegida
3. No frame seguinte reprotegemos, mas B já está errada e nunca foi marcada suja

O código original não tem esse furo justamente porque mata **tudo** na página:
nada sobrevive para ficar obsoleto. A lentidão dele é o preço da correção.
Reprotejer "no próximo frame" deixa uma janela de até um frame inteiro, e num
jogo que faz streaming de sprites isso é uma eternidade.

### Caminhos possíveis

- **A — verificar por hash na reproteção:** guardar hash dos sobreviventes ao
  desproteger e recalcular ao reprotejer, invalidando quem mudou. Fecha o furo
  de forma garantida; custa hashear ~30 texturas por fault, muito mais barato
  que re-uploadar 30. Incremental sobre o que já está escrito.
- **B — emular a escrita no handler e nunca desproteger:** elimina a janela por
  construção; é o que emuladores maduros fazem. Exige decodificar a instrução
  ARM64 que faltou — o projeto já tem maquinário próximo (`ngen_Rewrite`
  decodifica acessos de memória para o stub de Store Queue).
- **C — olhar como o `flyinghead/flycast` atual resolve isso.** São ~10 anos a
  mais de evolução exatamente neste arquivo, e o `CLAUDE.md` já trata comparação
  com o upstream como ferramenta legítima de investigação. Antes de inventar,
  vale ver se já existe solução conhecida.

**Estado:** código presente, opt-in, **desligado por padrão** (sem
`FC_TEX_PRECISE_INVL` o comportamento é exatamente o original). Não usar em
build de uso real até o furo ser fechado.

---

## CORREÇÃO (2026-09-16): o diagnóstico acima está ERRADO — não é invalidação espúria

**Leia isto antes de qualquer coisa acima.** A conclusão de que as ~394
texturas re-uploadadas por frame estavam "quase todas intactas" é **falsa**, e
a tentativa 1 (+63% de fps) estava sendo rápida às custas de correção.

### O que refutou

Implementado `FC_TEX_SKIP_UNCHANGED` (opt-in): mantém a invalidação
conservadora intacta e, dentro do `Update()`, hasheia (XXH32) exatamente o
mesmo intervalo de VRAM que o lock vigia (`[sa_tex, sa+size)`, mais o
`palette_hash` para paletizadas) e pula conversão+upload se o conteúdo for
idêntico ao do último upload. Seguro por construção: nunca deixa de
**detectar** mudança, só evita **refazer trabalho idêntico**.

Resultado no mslug6: **3.560 pulos em ~174.000 updates = 2%**. Em **98% dos
casos o conteúdo da VRAM realmente mudou.**

### O erro de raciocínio

Interpretei `vram_write_faults` ≈ 13/frame como "o jogo só escreve 13 vezes
por frame". **Fault não é escrita.** Depois do primeiro fault a página é
desprotegida, então todas as escritas seguintes naquela página **não geram
fault nenhum**. Os 13 faults podem cobrir megabytes de escrita.

Logo a razão "29,88" não é "30 texturas mortas à toa por escrita" — é "30 por
*fault*", e cada fault abre uma página que depois recebe muita escrita
legítima. O Metal Slug 6 faz streaming real de sprites para a VRAM todo frame.

Isso explica de forma coerente as três observações:

- a invalidação precisa quebrou visualmente porque **perdia mudanças reais**;
- o skip por hash quase não dispara porque **o conteúdo mudou mesmo**;
- o +63% de fps vinha de **pular trabalho necessário** — rápido e errado,
  mesma natureza do speedhack de skip de Translucent (`tech_debits.md` 5.2).

### O que continua válido

Medido e não contestado: `Process()` é ~57ms de um frame de ~63ms, e ~41ms
disso é converter e subir ~390 texturas por frame, a ~74,8µs cada. O trabalho
é **real**. Para ganhar, ele precisa ficar **mais barato**, não ser evitado.

### A pergunta que reorienta o alvo (levantada pelo usuário)

*"Mas o hardware do Naomi aguentava fazer essa parada toda?"* — **Não, e ele
nunca precisou.** O PowerVR lê textura **direto da VRAM, no formato nativo**:
twiddled (ordem Morton), VQ comprimido, paletizado. O jogo escreve os bytes do
sprite na VRAM e a GPU amostra dali ao rasterizar. Não existe upload de
textura, não existe conversão, não existe memória separada de GPU. Escrever
~390 sprites por frame é tráfego trivial para o barramento do Naomi.

Ou seja, **os 41ms não são trabalho do jogo sendo emulado — são imposto da
emulação**. Os passos que pagamos (detectar mudança via proteção de página,
converter de twiddled/VQ/paletizado para um formato da GL, subir pra memória
de textura) não existem no console.

### Próxima direção (não medida ainda)

Fazer o que o hardware fazia: **deixar a GPU consumir o formato nativo**,
subindo bytes crus e destwiddlando/despaletizando no shader, em vez de
converter na CPU. O código já tem um embrião disso — `IsGpuHandledPaletted()`
(`TexCache.h`), cujo próprio comentário diz *"Some palette textures are
handled on the GPU. This is currently limited to textures using nearest
filtering and not mipmapped"*. O caminho existe mas é restrito.

Duas medições decidem:

1. **Split do `Update()`**: quanto dos 74,8µs é `texconv` (CPU) e quanto é
   `UploadToGPU` (GL). Define se o alvo é a conversão ou a transferência.
2. **Quantas texturas do mslug6 se qualificam** ao caminho GPU hoje, e o que
   as desqualifica (mipmap? filtro bilinear?).

### Estado do código

- `FC_TEX_PRECISE_INVL`: **não usar** — quebra a imagem, mantido só como
  registro do experimento.
- `FC_TEX_SKIP_UNCHANGED`: correto e seguro, mas **quase sem efeito** neste
  jogo (2%). Pode ter valor em jogos com textura estática; não medido.
- Ambos opt-in e **desligados por padrão**.

### Lição de processo

Isto foi pego porque o usuário perguntou se o hardware real daria conta — a
mesma pergunta que já tinha derrubado antes a hipótese do spin loop. Vale como
heurística permanente: **quando o emulador parece estar fazendo muito mais
trabalho do que o console fazia, ou a conta está errada, ou o trabalho é
imposto da emulação e não do jogo.** Nos dois casos a resposta muda o alvo.

---

## RESOLVIDO (2026-09-16): o caminho de paleta na GPU estava morto por default não inicializado

Depois de o diagnóstico da invalidação ser refutado (seção acima), o split do
`Update()` mostrou onde o custo realmente está, e daí saiu a correção.

### Onde o tempo estava

| | |
|---|---|
| `UploadToGPU` (chamada GL) | **15,49 s** de 30s = **35,0 ms/frame** |
| `texconv` (conversão CPU) | 1,32 s = 3,0 ms/frame |
| updates | 166.310 |
| dados de origem, total | **21,2 MB em 30s** (~134 bytes por textura) |

Não é largura de banda (0,7 MB/s) e não é conversão. São ~166 mil uploads de
sprites minúsculos a ~93µs cada: **custo por chamada**. Trocar `glTexImage2D`
por `glTexSubImage2D` quando o layout não muda (`FC_TEX_SUBIMAGE`) rendeu só
-4% — logo também não é a realocação.

### O bug

`gpu_handled` = **0** de 166.285 texturas paletizadas, e nenhuma desqualificada
por filtro, mipmap ou VQ. O desqualificador era `settings.rend.TextureUpscale`
valendo **0**:

1. `Makefile:19` → `HAVE_TEXUPSCALE := 0`
2. O único lugar que atribui esse campo (`libretro.cpp`) está dentro de
   `#ifdef HAVE_TEXUPSCALE` → compilado fora
3. O outro candidato (`LoadSettings()`, `nullDC.cpp`) estava dentro de
   `#ifndef __LIBRETRO__` → também compilado fora numa build libretro
4. O campo nunca era inicializado: ficava **0**
5. Todo o resto do código lê `> 1` e tolera o 0. **Só** `IsGpuHandledPaletted()`
   (`TexCache.h:744`) usa `== 1` — com 0, retorna sempre falso

Resultado: o caminho que faz **o que o hardware do Naomi fazia** (a GPU
consome o formato nativo, paleta aplicada no shader, sem expansão na CPU)
estava **permanentemente desativado**, por um default perdido entre dois
`#ifdef`. Correção: inicializar `settings.rend.TextureUpscale = 1` em
`LoadSettings()`, **fora** do `#ifndef __LIBRETRO__`.

### Resultado medido (savestate, 30s, cena idêntica)

| | fps antes | fps depois | `core_average` | `core_p95` | `core_p99` |
|---|---|---|---|---|---|
| **mslug6** | 14,4 | **19,4 (+35%)** | 60,8→42,7 (-30%) | 112,9→60,9 (**-46%**) | 149,8→64,3 (**-57%**) |
| **kofnw** | 52,0 | **55,7 (+7%)** | 11,0→9,5 (-14%) | 20,8→11,4 (**-45%**) | 40,1→18,6 (**-54%**) |
| mbaa | 50,5 | 49,6 (-2%) | +1% | +3% | ~0% (ruído) |

Tempo de textura no mslug6: 42,5 → 20,6 ms/frame (-52%); upload 34,6 → 13,1
ms/frame (-62%).

### E corrigiu glitches visuais

Verificação do usuário: *"corrigiu uns glitches que tinham, primeira vez que
vemos 20 fps aqui"*. Faz sentido — o caminho antigo expandia a paleta na CPU e
a assava na textura no momento do upload, então mudança de paleta sem mudança
de dados deixava cor obsoleta. O caminho da GPU aplica a paleta no shader,
sempre atual. **Mais rápido e mais correto.**

### Estado dos experimentos desta investigação

| flag | estado |
|---|---|
| (correção do `TextureUpscale`) | **aplicada, default, sem flag** — é bug fix |
| `FC_TEX_SUBIMAGE` | opt-in, OFF. Ganho ~4%, não validado visualmente |
| `FC_TEX_SKIP_UNCHANGED` | opt-in, OFF. Correto mas ~2% de acerto neste jogo |
| `FC_TEX_PRECISE_INVL` | opt-in, OFF. **Não usar** — quebra a imagem |
