# Status por jogo — fonte da verdade do último teste

> **Esta tabela É o estado conhecido de cada jogo.** Não precisa refazer A/B nem
> nova rodada só para saber "como está": o que está aqui é o último resultado
> medido/observado e vale até alguém testar de novo.

## Como manter (regras)

1. **Uma linha por jogo, atualizada inline.** Qualquer melhoria, piora ou nova
   observação reportada (pelo usuário jogando ou por medição) **substitui o conteúdo
   da linha do jogo**. **Não** criar seção nova com data, não acrescentar "Atualização
   AAAA-MM-DD" abaixo — o histórico vive em `docs/history.md` e `git log`.
2. Cada linha tem a **data do último teste** (coluna `Teste`) e, quando houver, a
   condição que importa (savestate × cold boot, core, tier2 on/off). Mudou o estado →
   atualize a data junto.
3. Jogo novo: acrescentar a linha no grupo da plataforma (Naomi / Atomiswave /
   Dreamcast), ordem alfabética.
4. **Fonte da verdade:** ao reportar o estado de um jogo, ou decidir o que atacar,
   leia daqui. Só medir de novo quando (a) mudou o código que afeta o jogo,
   (b) o usuário reportou algo diferente, ou (c) a linha diz "não testado". Não
   fazer A/B de rotina para jogos já registrados.
5. Colunas: `VEL%` = `audio_frames/(duração×44100)` (velocidade real); `fps` =
   frames **novos**/s quando medido (dupes inflam o contador do retrorun). Número
   de benchmark e impressão do usuário são ambos válidos — marque qual é qual na
   observação ("jogando:" = usuário; sem prefixo = benchmark). Onde só há
   impressão, deixe `—` nos números.
6. Itens abertos referenciados (4.xx/5.x) estão em `docs/tech_debits.md`.

Vocabulário: "cauda" = picos p95/p99 (o que se sente como hicup); "cold" = cold
boot sem savestate; "ES" = lançado pelo EmulationStation (`perfmax performance`,
`rr_capture.sh`). Estado do core: tier2 **aposentado** (OFF por padrão desde
2026-10-07); funções nativas por assinatura (`hle_fn`) ligadas.

## Naomi

| Jogo | Teste | VEL% | fps | Estado |
|---|---|---|---|---|
| asndynmt | 2026-09-28 | 61 | 58 | cold, tier2 OFF: liso até o character select, **crasha depois de escolher o personagem** (4.94). Com tier2 ON triângulo laranja (4.93) |
| azumanga | 2026-09-28 | 96 | 60 | cold, tier2 OFF: **perfeito** (com tier2 tela preta) |
| capsnk | 2026-09-28 | 94 | 58 | cold: **perfeito**; suavizar movimento (cache de paleta + wait curto) |
| cspike | 2026-09-28 | 96 | 52 | cold: **jogável**, iluminação ok (tier2 destruía) |
| cvs2 | 2026-10-07 | 99,6 | 58,3 | ES: **muito bem** 60 fps; hicups novos desde o wait; boot rápido (lazy GD, 22 s→1 s) |
| cvsgd | — | — | — | boota; usuário não tem o GD |
| ggx | 2026-09-28 | 94 | 59 | cold, tier2 OFF: **perfeito** (tier2 travava no parental advisory) |
| ggxx | 2026-09-28 | 92 | 60 | cold, tier2 OFF: **perfeito**. Savestate antigo V12 aborta o load (usuário vai regenerar) |
| ggxxac | 2026-09-28 | 92 | 60 | cold: **liso, ~100%** sensação; demora a iniciar |
| ggxxsla | 2026-09-28 | 92 | 60 | cold, tier2 OFF: **perfeito** (crashava no disclaimer com tier2). Savestate antigo a regenerar |
| gwing2 | 2026-09-28 | 92 | 60→45 | cold: jogável; slowdown com tiros massivos (~45 fps na cena pesada) |
| ikaruga | 2026-09-28 | 94 | 52 | decente, mas **controles invertidos + aspect ratio errado** (bug do retrorun, formato vertical) |
| mbaa | 2026-10-06 | 100 | 60 | **60 fps lisos**, ~1% dupes, artefatos sumiram (eram dupes, 4.110). Glitches do savestate antigo: ver 4.31 |
| meltyb | 2026-09-28 | 95 | 58 | cold: transição ok, **trava (tela preta) quando a luta começa** (4.95) |
| meltybld | 2026-09-28 | 94 | 60 | cold: **perfeito**, referência de suavidade; barras de life ainda erradas |
| sfz3ugd | 2026-09-28 | 95 | 60 | cold: **perfeito** (bench 2026-09-26: 100%/59,7) |
| slashout | 2026-09-28 | 91 | 27-32 | cold: jogável; fps 27-32 na cena pesada |
| spawn | 2026-09-28 | 88 | 58 | cold: jogável; controle pouco responsivo |
| zombrvn | 2026-09-26 | 100 | 43 | jogável, 40-55 fps no gameplay; sensação boa |

## Atomiswave

| Jogo | Teste | VEL% | fps | Estado |
|---|---|---|---|---|
| ggisuka | 2026-09-28 | 95 | 59 | **perfeito** |
| ggx15 | 2026-09-26 | 100 | 49 | ok (savestate); frame time com picos de 100 ms no ES quando tier2 ON (hoje OFF) |
| kofnw | 2026-09-26 | 99 | 54 | quase lá; cauda ainda incomoda (hicups). Perda de 2-3% do acesso compacto aceita |
| kofxi | 2026-10-06 | 100 | 58,9 | **60 fps constantes**, sem hicup, ~1% dupes |
| mslug6 | 2026-09-26 | 83 | 22 | 30 fps na maior parte; **~20 em boss** (partículas); VEL oscila 66↔100% entre rodadas; render pesado |
| ngbc | 2026-09-26 | 100 | 55 | ok |
| samsptk | 2026-09-26 | 100 | 43-59 | **full speed** após shader de paleta bilinear (20→59 fps); aguarda validação visual |

## Dreamcast

| Jogo | Teste | VEL% | fps | Estado |
|---|---|---|---|---|
| Capcom vs SNK 2 (CvS2) | 2026-09-27 | 100 | 56 | descrachou; cauda baixa |
| DOA2 | 2026-10-07 | 89 | 37 | tier2 OFF: slowdowns, "quase jogável"; teto = throughput do SH4 (4.20). HLE do laço de vértices ~+1% |
| EGG | 2026-10-07 | 99,5 | 46 | tier2 OFF: **destravou** (travava a 11 fps com tier2) |
| Evolution (1) | 2026-10-09 | 99 | 30 | "roda muito bem, 30 fps"; pedaços do chão pretos (sort per-strip, 4.124; per-triangle corrige mas custa ~4 fps). **FMV a 100% (era ~78%)**, vídeo 30 quadros/s, usuário: "vídeo perfeito, áudio certo" (4.123). Obs.: há CHD com zstd que não abre (`cdzs`) |
| Evolution 2 | 2026-10-07 | 97 | 28 | jogável a 30, quedas p/ 24; save/load a ver |
| Grandia II | 2026-10-06 | 100 | 30 | **30 fps lisos**, ~0% dupes. **Som estourado + vozes baixas** (4.116) |
| KOF Evolution | 2026-10-07 | 98 | 51 | 60 fps fora da chuva; chuva ~54 com quedas p/ 32 e hicups |
| Le Mans | 2026-10-07 | 95 | 30 | cold, HLE: velocidade perfeita, 30 fps; **áudio quebrado (uma batida em loop, bug antigo)**; load demorado; `declared_fps=7.5` a investigar |
| Macross M3 | 2026-10-07 | 97 | 6,8 | **tela preta após o logo, preso em ~145 ms/frame** (4.115) |
| MvC2 | 2026-10-07 | 100 | 47 | 33-45 fps, **glitches** e suspeita de regressão (4.118); sem tier2 44 fps |
| Napple Tale | 2026-10-07 | 99 | 30 | 30 cravado, partes pesadas ~27; áudio afinado (WSOLA) mas **som estourado** (4.116); **reclama de espaço no memory card / crash ao salvar** (4.117) |
| Phantasy Star Online v2 | 2026-10-07 | 98 | 16,5 | **trava após load game** (preso em ~71 ms/frame); new game/save funcionou antes |
| Power Stone | 2026-10-07 | 97 | 38 | parece 100% mas a ~45 contados (dupes/frameskip?) |
| Project Justice | 2026-10-07 | 91 | 55 | era "sempre perfeito"; agora ~118 dupes, "passa de 60", slowdown em special (4.118) |
| Resident Evil CV | 2026-10-07 | 94 | 29 | perfeito; slowdowns em explosões/fogo; FMV ~83% (IDCT da Sofdec é alvo) |
| Sonic Adventure 2 | 2026-10-07 | — | — | **crash após os logos** (exit 134, `SH4ThrownException`, 4.114); só com tier2 ON — rever com OFF |
| Sonic Shuffle | 2026-10-07 | 68 | 14,8 | cena pesada do savestate a ~7 fps, p95 141 ms; laço de busca linear sem fix (ver tech_debits) |
| Shenmue | 2026-10-07 | 83 | 25 | tier2 OFF: não crasha; 30 fps dentro de casa. Crash no loading entre lugares era tier2 |
| Shenmue II | 2026-10-07 | 94 | 28 | ON: 19-30 fps, jogável a ≥25; **reclama de memory card cheio** (4.117). Save pesado: ~60% / 18 fps |
| Skies of Arcadia (Disc 1) | 2026-10-07 | 99,6 | 29,8 | **30 cravados, rodando muito bem** |
| Skies of Arcadia (Disc 2) | 2026-10-07 | — | — | **não boota** (exit 133, SIGSEGV; suspeita CHD) |
| Soulcalibur | 2026-10-07 | 89 | 53,5 | 60 cravados; no ES do zero congelou no boot (4.30, não reconferido) |
| TR Chronicles | 2026-10-07 | — | 9 | BIOS Windows CE demora; **tela preta / teto ~147 ms/frame** (4.115), não é tier2 |

## Padrões observados (resumo do que explica a tabela)

- **Tier2 OFF** corrigiu a maioria das quebras (EGG, Shenmue, SA2, carts Naomi).
- **Emu-bound** (VEL < 100%): Shenmue/Shenmue II, DOA2, mslug6, Sonic Shuffle, Zombie
  Revenge — o teto é o throughput do SH4, não o render.
- **Presos em teto de frame** (~90-150 ms, ~100% dupes): EGG (resolvido), Macross,
  PSO, TR Chronicles — assinatura de espera/timeout (4.115).
- **Som:** estourado/vozes baixas em Grandia II e Napple (4.116); abaixo de 100% o
  WSOLA do retrorun mantém o tom.
- **Memory card** cheio/crash ao salvar: Napple e Shenmue II (4.117).
