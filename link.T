{
   global:
      retro_*;
      /* Extensao privada do frontend RetroRun (ver core/libretro/retrorun_ext.cpp):
         o RetroRun faz dlsym destes nomes antes do launch. Sem isto, o script de
         versao os tornaria locais e o dlsym falharia. */
      flycast_retrorun_*;
   local: *;
};
