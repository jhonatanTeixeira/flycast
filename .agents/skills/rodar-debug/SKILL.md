---
name: rodar-debug
description: Rodar um jogo no device R36 para MEDIR/DEBugar (não só jogar): carregar savestate, criar uma config separada por teste, benchmark com JSON de janela rolante, dumper leve do JIT, contadores de sincronização (FC_SYNC_STATS), A/B de core ou config e ler os números (VEL%, fps NOVOS, dupes, frame time). Use sempre que for rodar um jogo para medir performance, comparar cores/configs, capturar o dump do JIT, reproduzir um bug no device, ou quando o usuário pedir "roda o X e mede", "faz um A/B", "captura o dump do X", "testa esse core".
---

# Rodar um jogo no device com debug/medição

Device: SSH `ark@192.168.0.14` (senha `ark`, via `sshpass`). Frontend: `retrorun3`.
Isto é o passo a passo de **medir** (o `rodar-games` cobre o "só jogar" e os
backups de core; o `jit-nativo` cobre a análise do dump). Regra de ouro: **medir
antes de otimizar** e **sempre guardar o log/JSON** da rodada.

## 0. Como o ES lança (referência do que reproduzir)

`/etc/emulationstation/es_systems.cfg` → `sudo perfmax %GOVERNOR% %ROM%; nice -n -19
/usr/local/bin/<sistema>.sh %EMULATOR% %CORE% %ROM%; sudo perfnorm`. O
`/usr/local/bin/dreamcast.sh` (e `naomi.sh`/`atomiswave.sh`) no ramo `retrorun3` faz:

```bash
env RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2 \
  /home/ark/rr_capture.sh -c /home/ark/.config/retrorun.cfg --triggers \
  -s /roms2/<sistema> -d /roms2/bios <core> <rom>
```

O `rr_capture.sh` cria **uma pasta por sessão** em `/roms2/dcbat/<AAAAMMDD-HHMMSS>_<jogo>/`
e exporta `FC_JIT_DUMP="$D" FC_JIT_DUMP_LITE=1 FC_PERF_MAP=1 FC_SYNC_STATS=1
RETRORUN_BENCHMARK_ALLOW_HOTKEYS=1 RETRORUN_BENCHMARK_ROLLING=1`, roda
`retrorun3 --benchmark 40 --benchmark-warmup 5 --benchmark-json "$D/bench.json" "$@"`
e ainda prende um `perf record -F 299 -k mono -p <pid>`. Dentro: `jit-*.txt`,
`samples.txt.gz`, `sync-stats-*.txt`, `bench.json`, `live.log`, `exit_code.txt`.

**Sempre rode entre `sudo perfmax performance <rom>` e `sudo perfnorm`** (governor
`performance`; em `ondemand` os números saem piores, 4.70).

## 1. Savestate: config separada por teste

O savestate automático fica ao lado da ROM: `<rom>.<variante>-rrstate.auto`
(ex.: `Grandia II (USA).fc2021-rrstate.auto`). O retrorun3 procura, em ordem,
`<jogo>.rrstate.auto`, `<jogo>.fc2022-rrstate.auto`, `<jogo>.fc2021le-rrstate.auto`,
`<jogo>.fc2021-rrstate.auto`.

**A config padrão do ES (`/home/ark/.config/retrorun.cfg`) NÃO tem `retrorun_auto_load`
→ não carrega savestate (cold boot).** Para carregar, use/crie uma config com
`retrorun_auto_load = true`:

```bash
# no device: copia a config oficial e liga o auto_load (uma por jogo/teste)
cp /home/ark/.config/retrorun.cfg /home/ark/r_<jogo>.cfg
echo "retrorun_auto_load = true" >> /home/ark/r_<jogo>.cfg
```

Configs já existentes: `r_g2.cfg` (auto_load), `r_napple_load.cfg`, `r_t2load.cfg`
(auto_load); `r_noload.cfg`, `r_t_disabled.cfg` (cold boot). As configs antigas
`r_t2off.cfg`/`r_t2load.cfg` são de quando havia o tier2 (removido em 2026-10-10);
a linha `flycast2026_tier2` delas ficou sem efeito. Convenção: **uma config por
teste**, nomeada pelo jogo e pela variável testada; as opções `flycast2026_*` ficam nela.

## 2. Rodar com benchmark + captura (o comando de medir)

```bash
timeout 120 sshpass -p ark ssh -o StrictHostKeyChecking=no ark@192.168.0.14 '
CORE=/home/ark/.config/retroarch/cores/flycast2026_libretro.so   # ou um .so de teste
ROM="/roms2/dreamcast/<jogo>.chd"
L=<rotulo>; D=/roms2/dcbat/ab_$L; rm -rf "$D"; mkdir -p "$D"
pkill -x retrorun3 2>/dev/null; sleep 2
sudo perfmax performance x >/dev/null 2>&1
nohup env RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2 FC_SYNC_STATS=1 \
  /usr/local/bin/retrorun3 --benchmark 40 --benchmark-warmup 5 --benchmark-json "$D/bench.json" \
  -c /home/ark/r_<jogo>.cfg --triggers -s /roms2/dreamcast -d /roms2/bios "$CORE" "$ROM" > "$D/live.log" 2>&1 &
P=$!; sleep 55; kill -TERM $P 2>/dev/null; sleep 3
cp /tmp/sync-stats-$P.txt "$D/" 2>/dev/null
cat "$D/bench.json"; echo; cat "$D"/sync-stats-*.txt
'
```

- O JSON é **janela rolante dos últimos 40 s** e é gravado **ao fechar** — um
  `kill -TERM` no pid basta (o retrorun escreve o JSON no shutdown).
- **Tempo de parede tem que cobrir load + warmup + 40 s de emulação.** Jogos que
  demoram a carregar (mbaa.zip 245 MB, CHDs grandes) precisam de `sleep 70-80`, senão
  sai `Benchmark failed: measurement did not complete` e `bench.json` vazio.
- `-s /roms2/<sistema>` = pasta das ROMs; Naomi/Atomiswave usam `.zip` MAME ou
  diretório com `gdl-*.chd`; DC usa `.chd`.
- Para só jogar (sem benchmark), tire `--benchmark*` e deixe rodando (o usuário
  fecha); use o mesmo `env` + `-c`.

## 3. Variáveis de debug (todas opt-in)

| var | o que faz | onde lê |
|---|---|---|
| `FC_SYNC_STATS=1` | contadores de sincronização emu↔render | `/tmp/sync-stats-<pid>.txt` |
| `FC_JIT_DUMP=<dir>` + `FC_JIT_DUMP_LITE=1` | dump leve do JIT (SH4/SHIL/ARM64 por bloco) | `<dir>/jit-<pid>.txt` |
| `FC_PERF_MAP=1` | mapa do cache de código p/ o `perf` | usado pelo `jit_lite_report.py` |
| `FC_FRAME_WAIT_MS=N` | prazo da main p/ o próximo frame (0 = antigo; sem a var = auto, o fps medido) | — |
| `FC_EMU_WAIT=1` | restaura a espera antiga da emu (A/B) | — |
| `FC_HLE=0/1` | desliga/liga as funções nativas por assinatura | — |
| `FC_STATE_HASH` + `FC_RTC_FIXED` + `FC_INPUT_NEUTRAL` | validação frame-a-frame | stdout |
| `FC_REND_SPLIT=1`, `FC_TA_SPLIT=1` | quebra o custo por frame (wait/process/render; lock/decode/index) | `/tmp/rend-split-*.txt` |

`FC_JIT_DUMP` **completo derruba o jogo** (4.98); use o `_LITE`. Dump serve para a
**distribuição** do código quente, **nunca** para tempo/VEL — métricas sempre de uma
rodada **limpa** separada.

## 4. Ler os números (o que importa)

Do `bench.json`:
- **VEL%** = `audio.audio_frames / (duration_seconds * sample_rate)` (sample_rate
  44100). É a velocidade **real** do jogo — a que vale.
- `presented_frames / duration_seconds` = fps **apresentado** (conta repetidos).
- `duplicated_frames` = frames **repetidos** (o retrorun reapresenta o último frame).
- `timing_ms.core_*` e `active_frame_*` em p50/p95/p99 + média.

Do `sync-stats-*.txt`:
- `new_fps` = taxa **real** de frames NOVOS do jogo (é isto que tem que casar com o
  fps do jogo: 30 num jogo de 30, 60 num de 60).
- `game_frame_ema` = o intervalo medido do jogo (ms) que o core usa no prazo da main.
- `dropped` / `new_frame_interval` p50/p95/p99.

**Sempre olhar apresentado × novos × dupes juntos.** Dupes altos = apresentação
inflada (a emulação pode estar a VEL 100%) e causam artefatos (era a causa dos
artefatos do mbaa, 4.110). O core hoje casa a apresentação com o fps medido.

## 5. A/B de core ou config

- Core de teste **nunca** sobrescreve o oficial: `scp flycast_libretro.so
  ark@192.168.0.14:/home/ark/flycast_<rotulo>.so` e passe esse caminho no lugar do
  `<core>`. (Cuidado com hardlinks do cache `~/.debug` do perf — no device, prefira
  `cp` p/ temporário + `mv`.)
- Mesma cena/savestate, `perfmax performance`, e a tabela completa dos dois lados.
- Para comparar variáveis do **core**, use `env ... FC_X=...` (ex.: A = sem a var,
  B = `FC_FRAME_WAIT_MS=40`); não precisa rebuildar.

## 6. Dump do JIT e análise

O `rr_capture.sh` já grava o dump leve. Para analisar: puxe `jit-<pid>.txt` +
`samples.txt.gz` (`scp`), descomprima e rode `tools/jit_lite_report.py jit-<pid>.txt
samples.txt` (resolve amostra do perf → bloco e separa blocos de stubs);
`tools/sh4dis.py` desmonta o SH4. O resto do ciclo (reescrever em nativo, plugar por
assinatura) está na skill **`jit-nativo`**.

## 7. Deploy do core oficial (com backup)

```bash
# no device: backup do atual, e o novo (copiar p/ temp + mv por causa de hardlinks)
cp -p /home/ark/.config/retroarch/cores/flycast2026_libretro.so \
      /roms2/backups/flycast2026_libretro.so.bak-<motivo>-<data>
cp /home/ark/flycast_<rotulo>.so /home/ark/.config/retroarch/cores/.tmp.so && \
  mv /home/ark/.config/retroarch/cores/.tmp.so \
     /home/ark/.config/retroarch/cores/flycast2026_libretro.so
```

O oficial é o que o ES usa (`flycast2026_libretro.so`); o `flycast_libretro.so` é o
**upstream** (flyinghead). `/` do device é apertado (~200 MB livres) — cores de teste
antigos vão para `/roms2/backups/old_test_cores/`.

## 8. Regras

- Toda medição entre `perfmax performance` e `perfnorm`.
- Mesma cena/conteúdo nos dois lados do A/B; confirmar o que cada rodada mediu.
- Guardar sempre o log/JSON (`/roms2/dcbat/...`) — é a única forma de entender um
  crash/freeze depois.
- Registrar achado em `docs/tech_debits.md` e a sessão em `docs/history.md`.
- Build cross-compile: ver a skill `rodar-games` (flags explícitas + `make clean` com
  os mesmos argumentos; cuidado com objetos de outro `platform=` — se o link acusar
  `EM: 62`, `find . -name '*.o' -delete`).
