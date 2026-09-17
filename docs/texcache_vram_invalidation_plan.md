# Invalidação de textura por página de VRAM — diagnóstico e plano de correção

> Alvo: Metal Slug 6 (Naomi/Atomiswave), savestate pesado do usuário
> (`/roms2/naomi/mslug6.fc2021-rrstate.auto`). Todos os números medidos no
> device (R36/RK3326) em 2026-09-16, 30s por rodada, cena idêntica garantida
> por savestate. Instrumentação em `b509c7ee7`. **Nada implementado ainda.**

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
