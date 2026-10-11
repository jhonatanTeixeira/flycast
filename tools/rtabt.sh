# uso no device: rtabt.sh <rotulo> <cfg> <rom> <sleep> [env...]  -- esfria < 63C antes e loga clock/temp/gpu
L=$1; CFG=$2; ROM=$3; SL=$4; shift 4
D=/roms2/dcbat/rtabt_$L; rm -rf $D; mkdir -p $D
pkill -x retrorun3; sudo perfnorm >/dev/null 2>&1
while [ $(cat /sys/class/thermal/thermal_zone0/temp) -gt 63000 ]; do sleep 5; done
sudo perfmax performance x >/dev/null 2>&1
nohup env SDL_VIDEO_EGL_DRIVER=libEGL.so DEVICE_NAME=RG351MP RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2 FC_SYNC_STATS=1 "$@" \
  /usr/local/bin/retrorun3 --benchmark 20 --benchmark-warmup 8 --benchmark-json $D/bench.json \
  -c $CFG --triggers -s /roms2/dreamcast -d /roms2/bios /home/ark/flycast_rt.so "$ROM" > $D/live.log 2>&1 < /dev/null &
P=$!
( i=0; while [ $i -lt $SL ]; do echo "$i $(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq) $(cat /sys/class/thermal/thermal_zone0/temp) $(cat /sys/devices/platform/ff400000.gpu/utilisation) $(cat /sys/devices/platform/ff400000.gpu/devfreq/*/cur_freq)"; sleep 1; i=$((i+1)); done ) > $D/freq.txt & FP=$!
wait $FP; kill -TERM $P 2>/dev/null; sleep 4; cp /tmp/sync-stats-$P.txt $D/ 2>/dev/null; pkill -x retrorun3; sudo perfnorm >/dev/null 2>&1
awk -v L=$L 'NR>20 {f+=$2; t+=$3; g+=$4; n++} END {printf "%s: cpu %.0f MHz, temp %.1f C, gpu %.0f%% (s 20-fim)\n", L, f/n/1000, t/n/1000, g/n}' $D/freq.txt
