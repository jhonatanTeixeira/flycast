---
name: rodar-games
description: Como rodar um jogo no device R36 de teste (retrorun3 + core flycast), como trocar/fazer backup de cores e configs, e a regra de SEMPRE guardar os logs. Use sempre que o usuário pedir para rodar/abrir um jogo no device, testar um build no device, deployar/fazer backup de um core, ou quando qualquer medição no device for necessária.
---

# Rodar jogos no device R36 (e backups/logs)

Device de teste: SSH `ark@192.168.0.14` (senha `ark`, via `sshpass`). Frontend de teste:
`retrorun3` (`/usr/local/bin/retrorun3`).

**Sempre rode entre `sudo perfmax performance <rom>` e `sudo perfnorm`** — é como o ES
lança dreamcast/naomi/atomiswave (`governor = performance`). Em `ondemand` puro os
números saem piores.

**Sempre guarde o log** da rodada (redirecione para `/home/ark/live_<jogo>.log`). Log é a
única forma de depois entender um crash/freeze sem repetir a rodada — nunca descarte.

## Rodar um jogo (cold boot, na tela)

```bash
timeout 40 sshpass -p ark ssh -o StrictHostKeyChecking=no ark@192.168.0.14 \
  'cd /home/ark; export SDL_VIDEO_EGL_DRIVER=libEGL.so; export DEVICE_NAME=RG351MP; \
   sudo perfmax performance x >/dev/null 2>&1; \
   nohup retrorun3 -c /home/ark/r_noload.cfg --triggers -s /roms2/naomi -d /roms2/bios \
     <core> "/roms2/naomi/<jogo>.zip" > /home/ark/live_<jogo>.log 2>&1 & \
   echo "aberto pid=$!"'
```

- `-c /home/ark/r_noload.cfg` = **cold boot** (sem `auto_load` de savestate). É a política
  nova; `r_t2off.cfg` = mesma config com `flycast2026_tier2 = disabled` (A/B).
- `<core>`: `/home/ark/.config/retroarch/cores/flycast2026_libretro.so` (oficial do
  device) ou um `.so` de teste (ex.: `/home/ark/flycast_lazygd.so`).
- `-s /roms2/<sistema>` = pasta de ROMs; `-d /roms2/bios` = BIOS.
- Para **benchmark** (JSON com `core_average`/`video_average`/`audio_frames` por frame),
  acrescente `--benchmark 40 --benchmark-warmup 5 --benchmark-json /home/ark/bench_<jogo>.json`.
- Para esperar o usuário fechar: poll `pgrep -x retrorun3` até sumir.

Os jogos GD-ROM do Naomi têm diretório próprio com `.chd` (`/roms2/naomi/<jogo>/gdl-*.chd`);
os cartuchos usam o `.zip` MAME. Se um GD-ROM parecer "não bootar", pode ser só a
descompressão do CHD (ver tech_debits 4.89).

## Backups (nunca sobrescreva o core oficial sem backup)

- O core **oficial** do device é `~/.config/retroarch/cores/flycast2026_libretro.so`. Para
  A/B de desenvolvimento **nunca** sobrescreva ele: use um `.so` separado (ex.:
  `/home/ark/flycast_test.so`).
- Ao deployar um build, **faça backup** do que está sendo substituído antes (ex.:
  `cp x.so x.so.bak-<motivo>`), e dos cfgs (`*.bak-<motivo>`).
- **Cuidado com hardlinks:** `cp` por cima de um core com hardlinks (cache `~/.debug` do
  `perf`) altera todos os nomes do inode. Use `cp` para um temporário + `mv`.
- Deploy local → device: `sshpass -p ark scp flycast_libretro.so ark@192.168.0.14:/home/ark/<nome>.so`.

## Build cross-compile (aarch64) — regras que já custaram caro

- O Makefile deste fork tem bugs conhecidos em `CXX ?=`/`CC_AS ?=`. **Passe explícito**:
  ```bash
  make platform=arm64 CC_PREFIX=aarch64-linux-gnu- \
       CXX=aarch64-linux-gnu-g++-13 CC=aarch64-linux-gnu-gcc-13 \
       CC_AS=aarch64-linux-gnu-g++-13 HAVE_OPENMP=0 LDFLAGS="-L." -j2
  ```
  (Falta `-lGLESv2` pro linker local — há um `libGLESv2.so` copiado do device na raiz,
  daí o `LDFLAGS="-L."`.)
- **`make clean` sozinho limpa a lista errada** (de outro `platform=`). Sempre rode o
  `clean` com **exatamente os mesmos argumentos do build** e confirme com
  `find . -name "*.o" | wc -l` = 0.
- Depois de editar um **header amplamente incluído** (`types.h` em especial), faça o
  `clean` completo — o rastreamento de dependência não é confiável e um `.o` desatualizado
  dá regressão silenciosa (sem erro/warning).
- **Não use `-j$(nproc)`** (máquina compartilhada; watchdog de baixa-memória derruba).
  Use `-j1`/`-j2` e retome o `make` incremental se for interrompido.
- **Não commite** nada sem o usuário pedir.

## Instrumentação (opt-in) útil

- `FC_TIER2_DELAY_MS=N` — adia a ativação do tier2 em N ms (evita exceção de FPU no boot).
- `FC_EXC_LOG=1` — loga exceções SH4.
- `FC_JIT_TRACE=<arquivo>` (+ `FC_JIT_TRACE_ALL=1`, `FC_JIT_TRACE_BOOT=1`) — traça blocos.
- `FC_DUMP_BLOCK=<vaddr,...>` — dumpa SH4/ARM64 de blocos.
- `FC_BLOCK_PROF=1` — dumpa os blocos mais quentes em `/tmp/hot-blocks-<pid>.txt`.
- `FC_TIER2_RACE=1` — detector de geração nos mapas do worker do tier2.

Toda instrumentação nova deve ser **opt-in** (custo zero desligada) e registrada em
`docs/tech_debits.md`.
