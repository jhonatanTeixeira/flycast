---
name: bateria
description: Rodar a bateria completa de cold boot de um sistema no device R36 (naomi, dreamcast, atomiswave) — um jogo por vez, com o benchmark do retrorun3 ligado, esperando o usuário fechar o jogo e dar a impressão dele, gravando números + a observação do usuário no docs/game_status.md. Use SEMPRE que o usuário disser "vamos fazer a bateria de {naomi,dreamcast,atomiswave}", "rodar a bateria", "bateria completa", "testar todos os jogos de X", ou pedir para re-testar todos os jogos de um sistema no device.
---

# Bateria de jogos (cold boot) no device R36

Teste de regressão do emulador: abre **todos** os jogos de um sistema no device, um por
vez, com o benchmark do `retrorun3` ligado, e combina os **números objetivos** (do
benchmark) com a **impressão do usuário** olhando a tela. O JSON sozinho engana (jogo
parado na logo NAOMI dá 60 fps / 100% VEL) — por isso a observação do usuário é parte
obrigatória do resultado.

Antes de começar, leia `rodar-games` (como rodar jogos, backups e logs no device).

## Fluxo (repete por jogo)

1. **Anuncie qual jogo** e abra ele no device com o benchmark ligado (comando abaixo).
2. **Espere o usuário fechar** o jogo. Faça poll com `pgrep -x retrorun3` (com
   `RETRORUN_BENCHMARK_KEEP_RUNNING=1` o processo **só fecha quando o usuário fecha**;
   o JSON já é gravado aos 40s).
3. Quando fechar, **espere o usuário dizer a impressão dele** (fps sentido, suavidade,
   hicups, glitch, se travou). Não avance sem isso.
4. Leia o JSON e calcule os números. **Atualize a linha do jogo no `docs/game_status.md`**
   (inline, números + observação do usuário; ver formato abaixo).
5. Vá para o próximo jogo.

**Regra de parada:** se o usuário disser que um jogo **crashou** (ou travou/congelou de
forma anormal), **pare a bateria**, investigue e corrija, e **retome a partir daquele
jogo** (não pule). Não continue a bateria com um crash em aberto.

## Abrir um jogo com o benchmark

Sistema → pasta de ROMs: `naomi` → `/roms2/naomi`, `dreamcast` → `/roms2/dreamcast`,
`atomiswave` → `/roms2/atomiswave` (confirme a pasta no device se não existir).

```bash
timeout 30 sshpass -p ark ssh -o StrictHostKeyChecking=no ark@192.168.0.14 \
  'cd /home/ark; export SDL_VIDEO_EGL_DRIVER=libEGL.so; export DEVICE_NAME=RG351MP; \
   export RETRORUN_BENCHMARK_ROLLING=1; \
   sudo perfmax performance x >/dev/null 2>&1; \
   nohup retrorun3 -c /home/ark/r_noload.cfg --triggers -s /roms2/<sistema> -d /roms2/bios \
     --benchmark 40 --benchmark-warmup 5 --benchmark-json /home/ark/bench_<jogo>.json \
     <core> "/roms2/<sistema>/<jogo>.zip" > /home/ark/live_<jogo>_bench.log 2>&1 & \
   echo "aberto pid=$!"'
```

- `<core>`: o `.so` de teste/deploy em uso (ex.: `/home/ark/flycast_lazygd.so` ou o
  oficial `/home/ark/.config/retroarch/cores/flycast2026_libretro.so`). **Anote qual
  core** rodou a bateria — os números só valem para aquele binário.
- `r_noload.cfg` = cold boot (sem savestate). (O `r_t2off.cfg` era de quando havia
  o tier2, removido em 2026-10-10.)
- `RETRORUN_BENCHMARK_ROLLING=1` = **janela rolante**: o benchmark guarda só os
  **últimos 40s** (descarta o dado antigo) e **não fecha o jogo** — reporta o JSON no
  fechamento (requer o `retrorun3` de 2026-09-29+). É o padrão da bateria: pega a cena
  no fim da sessão (cenas pesadas tardias) e você fecha quando quiser. (Sem ela,
  `RETRORUN_BENCHMARK_KEEP_RUNNING=1` grava aos 40s e não fecha, mas o JSON é dos
  **primeiros** 40s.)
- `--benchmark 40 --benchmark-warmup 5`: 40s de medição após 5s de warmup (é o tamanho
  da janela rolante). Ajuste se o jogo for muito lento pra bootar.

Poll para saber quando fechou:

```bash
for i in $(seq 1 20); do sleep 15; \
  if ! timeout 20 sshpass -p ark ssh -o StrictHostKeyChecking=no ark@192.168.0.14 \
       'pgrep -x retrorun3 >/dev/null' 2>/dev/null; then echo "FECHOU ($i)"; break; \
  else echo "rodando ($i)"; fi; done
```

## Extrair os números do JSON

O JSON do benchmark fica em `/home/ark/bench_<jogo>.json`. Campos:
`counters` (`core_frames`, `duplicated_frames`), `timing_ms` (`core_average`,
`core_p50/p95/p99`, `video_average`, `active_frame_p50/p95/p99`), `audio`
(`buffer_underruns`), `duration_seconds`, `sample_rate`.

- **VEL%** = `audio_frames / (duration_seconds * sample_rate) * 100` (sample_rate 44100).
- **fps** = `core_frames / duration_seconds`.
- Sempre os dois (frame time + fps), em p50/p95/p99 **e** média.

Se o jogo **não bootou** ou o usuário fechou antes, o JSON pode não existir — registre
isso (ex.: "não bootou", "sem JSON") junto com a observação.

## Formato do registro em `docs/game_status.md`

**Atualize a linha do jogo inline** na tabela da plataforma (Naomi / Atomiswave /
Dreamcast) — **não crie seção nova por bateria nem por data.** A tabela é a fonte da
verdade do último teste: substitua `Teste` (data), `VEL%`, `fps` e `Estado` com o que
o usuário relatou (suavidade, hicups, glitch, freeze, "não boota", crash) + a
condição (cold/savestate, core usado). Os números detalhados (core avg/p50/p95/p99,
underruns) ficam no JSON em `/roms2/dcbat/` ou no `docs/history.md`, não na tabela.

## Ao terminar a bateria

- Consolidar o resumo (quais bootam, quais não, quais crasham, anotações de perf).
- Registrar em `docs/history.md` (com timestamp) e atualizar `docs/tech_debits.md`
  para cada achado novo (crash, freeze, "não boota", regressão).
