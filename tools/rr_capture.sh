#!/bin/bash
# Captura da bateria DC (docs/sync_emu_render.md, dumper leve 4.100): roda o
# retrorun3 oficial com o dumper leve do JIT, contadores de sincronizacao,
# benchmark de janela rolante (ultimos 40 s, grava ao fechar) e perf preso ao
# processo. Uma pasta por sessao em /roms2/dcbat/. Chamado pelo dreamcast.sh no
# lugar do /usr/local/bin/retrorun3 (mesmos argumentos).
ROM="${@: -1}"
NAME=$(basename "$ROM" | sed 's/\.[^.]*$//' | tr -c 'A-Za-z0-9_-' '_' | cut -c1-40)
D=/roms2/dcbat/$(date +%Y%m%d-%H%M%S)_$NAME
mkdir -p "$D"
export FC_JIT_DUMP="$D" FC_JIT_DUMP_LITE=1 FC_PERF_MAP=1 FC_SYNC_STATS=1 RETRORUN_BENCHMARK_ROLLING=${RETRORUN_BENCHMARK_ROLLING:-1}
/usr/local/bin/retrorun3 --benchmark 40 --benchmark-warmup 5 --benchmark-json "$D/bench.json" "$@" > "$D/live.log" 2>&1 &
P=$!
echo "$ROM" > "$D/rom.txt"; date '+%F %T' > "$D/inicio.txt"
PERFDATA=/tmp/rr_capture_$P.data
if command -v perf >/dev/null 2>&1; then
  perf record -N -F 299 -k mono -p $P -o "$PERFDATA" > "$D/perf_record.log" 2>&1 &
fi
wait $P
RC=$?
date '+%F %T' > "$D/fim.txt"; echo "$RC" > "$D/exit_code.txt"
sleep 1
cp /tmp/sync-stats-$P.txt "$D/" 2>/dev/null; rm -f /tmp/sync-stats-$P.txt /tmp/perf-$P.map
if [ -f "$PERFDATA" ]; then
  perf script -i "$PERFDATA" -F comm,tid,time,ip,sym --ns 2>/dev/null | gzip -1 > "$D/samples.txt.gz"
  rm -f "$PERFDATA"
fi
exit $RC
