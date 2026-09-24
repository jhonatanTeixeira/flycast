#!/bin/bash
# pmu_game.sh TAG SAVEDIR ROM -- contadores do A53 na emu thread (2 grupos) + amostragem JIT x C++
export SDL_VIDEO_EGL_DRIVER=libEGL.so DEVICE_NAME=RG351MP
TAG="$1"; SAVEDIR="$2"; ROM="$3"
cd /home/ark
env RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 timeout 75 \
/usr/local/bin/retrorun3 -c /home/ark/retrorun_dbg3.cfg --triggers -s "$SAVEDIR" -d /roms2/bios \
  /home/ark/flycast_test.so "$ROM" > /home/ark/pmu_${TAG}.log 2>&1 &
sleep 12; PID=$(pgrep -x retrorun3 | head -1)
declare -A a; for t in /proc/$PID/task/*; do f=($(cat $t/stat)); a[$(basename $t)]=$((f[13]+f[14])); done
sleep 3; best=0; EMU=
for t in /proc/$PID/task/*; do tid=$(basename $t); f=($(cat $t/stat)); d=$((f[13]+f[14]-${a[$tid]:-0})); [ $tid != $PID ] && [ $d -gt $best ] && { best=$d; EMU=$tid; }; done
echo "pid=$PID emu_tid=$EMU jiffies3s=$best" > ~/pmu_${TAG}.txt
perf stat -t $EMU -x, -e cycles,instructions,r01,r02,r03,r04,r05,r17,r16 -o ~/pmu_${TAG}_a.csv -- sleep 14
perf stat -t $EMU -x, -e cycles,rE0,rE1,rE2,rE4,rE5,rE6,rE7,rE8,r10,rCA,rCC -o ~/pmu_${TAG}_b.csv -- sleep 14
perf record -t $EMU -e cycles -c 200000 -o ~/pmu_${TAG}_cyc.data -- sleep 10 >/dev/null 2>&1
perf record -t $EMU -e rE1 -c 20000 -e rE7 -c 20000 -o ~/pmu_${TAG}_stall.data -- sleep 10 >/dev/null 2>&1
kill $PID 2>/dev/null; wait
