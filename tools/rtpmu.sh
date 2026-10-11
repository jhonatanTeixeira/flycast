# uso no device: rtpmu.sh <rotulo> <cfg> <rom> [env...] -- esfria, roda, perf stat por thread 12 s, VEL do core
L=$1; CFG=$2; ROM=$3; shift 3
D=/roms2/dcbat/rtpmu_$L; rm -rf $D; mkdir -p $D
pkill -x retrorun3; sudo perfnorm >/dev/null 2>&1
while [ $(cat /sys/class/thermal/thermal_zone0/temp) -gt 63000 ]; do sleep 5; done
sudo perfmax performance x >/dev/null 2>&1
nohup env SDL_VIDEO_EGL_DRIVER=libEGL.so DEVICE_NAME=RG351MP RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2 FC_SYNC_STATS=1 "$@" \
  /usr/local/bin/retrorun3 -c $CFG --triggers -s /roms2/dreamcast -d /roms2/bios /home/ark/flycast_rt.so "$ROM" > $D/live.log 2>&1 < /dev/null &
P=$!; sleep 22
( for i in $(seq 1 12); do echo "$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq) $(cat /sys/class/thermal/thermal_zone0/temp) $(cat /sys/devices/platform/ff400000.gpu/utilisation)"; sleep 1; done ) > $D/freq.txt & FP=$!
sudo perf stat -e task-clock,cycles,instructions,r01,r02,r17,r16,page-faults,context-switches --per-thread -p $P -o $D/stat.txt -- sleep 12 >/dev/null 2>&1
wait $FP
kill -TERM $P; sleep 4; cp /tmp/sync-stats-$P.txt $D/ 2>/dev/null; pkill -x retrorun3; sudo perfnorm >/dev/null 2>&1
echo "$L ok"
