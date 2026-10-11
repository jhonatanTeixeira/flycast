# uso no device: sthash.sh <rotulo> <cfg> <rom> <sleep> [env...]
L=$1; CFG=$2; ROM=$3; SL=$4; shift 4
D=/roms2/dcbat/sth_$L; rm -rf $D; mkdir -p $D
pkill -x retrorun3; sleep 3; sudo perfmax performance x >/dev/null 2>&1
env SDL_VIDEO_EGL_DRIVER=libEGL.so DEVICE_NAME=RG351MP RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2 FC_STATE_HASH=$D/hash.txt FC_STATE_HASH_EVERY=1 FC_RTC_FIXED=1 FC_INPUT_NEUTRAL=1 "$@" \
  /usr/local/bin/retrorun3 -c $CFG --triggers -s /roms2/dreamcast -d /roms2/bios /home/ark/flycast_rt.so "$ROM" > $D/live.log 2>&1 < /dev/null &
P=$!; sleep $SL; kill -TERM $P; sleep 4; pkill -x retrorun3; sudo perfnorm >/dev/null 2>&1
echo "$L: $(grep -c '^F ' $D/hash.txt) frames"
