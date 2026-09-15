#!/bin/bash
export SDL_VIDEO_EGL_DRIVER=libEGL.so
export DEVICE_NAME=RG351MP
cd /home/ark

/usr/local/bin/retrorun3 -c /home/ark/retrorun_debug.cfg --triggers \
  -s /roms2/dreamcast -d /roms2/bios \
  --benchmark 90 --benchmark-warmup 90 \
  --benchmark-json /home/ark/perf_mine_final.json \
  /home/ark/.config/retroarch/cores/flycast_libretro.so \
  "/roms2/dreamcast/Shenmue (USA) (Disc 1).chd" \
  > /home/ark/perf_mine_final.log 2>&1
echo DONE_EXIT_$? >> /home/ark/perf_mine_final.log
