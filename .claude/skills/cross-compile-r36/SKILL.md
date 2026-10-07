---
name: cross-compile-r36
description: Como compilar (cross-compile) binários aarch64 para o device R36 a partir da máquina local x86_64, sem buildar no device. Cobre o toolchain aarch64-linux-gnu-g++-13, como montar o sysroot a partir do device (headers de terceiros + libs), os erros clássicos (glibc misturado, SDL_config.h, deps transitivas do libcurl) e como verificar. Use sempre que precisar gerar um binário para o device (retrorun, testes, qualquer ferramenta aarch64) — a regra do projeto é NÃO compilar no device.
---

# Cross-compile para o R36 (aarch64) a partir da máquina local

**Regra do projeto: não compilar no device.** O device (`ark@192.168.0.14`) só recebe o
binário pronto. Todo build para o R36 é cross-compile na máquina local (x86_64).

## Ferramentas que já existem na máquina

- Toolchain: `aarch64-linux-gnu-g++-13` / `aarch64-linux-gnu-gcc-13` (é o mesmo usado pelo
  build do core flycast; ver `CLAUDE.md`). Só existem os binários com sufixo `-13`.
- O toolchain traz **glibc 2.39** + crt (`/usr/aarch64-linux-gnu/lib`: `crt1.o`,
  `libc.so`, `libc_nonshared.a`, `libm.so`, `libdl.*`, `libpthread.*`).
- O device roda **glibc 2.41** (Debian 13). Como glibc é retrocompatível, um binário
  linkado com 2.39 roda no 2.41 — **mas o contrário não**. Não precisa copiar o glibc do
  device.

## Passo 1 — montar o sysroot (uma vez; refaça se trocar de device/firmware)

O sysroot é só **headers de terceiros + as `.so` que vamos linkar**. **Nunca** copie o
`/usr/include` inteiro nem os `bits/`/`sys/` do device: eles são glibc 2.41 e brigam com o
glibc 2.39 do toolchain (erro `'__time64_t' does not name a type`).

```bash
SR=/tmp/opencode/r36-sysroot          # onde quiser; fora do repo
rm -rf "$SR"; mkdir -p "$SR/usr/lib/aarch64-linux-gnu" "$SR/usr/include"
D=ark@192.168.0.14
SSH="sshpass -p ark rsync -az -e 'ssh -o StrictHostKeyChecking=no'"

# headers de terceiros (glibc fica por conta do toolchain)
sshpass -p ark rsync -az -e "ssh -o StrictHostKeyChecking=no" \
  $D:/usr/include/{SDL2,EGL,GLES2,GLES3,KHR,curl} "$SR/usr/include/"
sshpass -p ark rsync -az -e "ssh -o StrictHostKeyChecking=no" \
  $D:/usr/include/{png.h,pngconf.h,pnglibconf.h,zlib.h,zconf.h} "$SR/usr/include/"

# o SDL_config.h do Debian é um indirecionador para <SDL2/_real_SDL_config.h>,
# que fica no dir multiarch — copie SÓ a pasta SDL2 de lá (o resto é glibc):
sshpass -p ark rsync -az -e "ssh -o StrictHostKeyChecking=no" \
  $D:/usr/include/aarch64-linux-gnu/SDL2 "$SR/usr/include/aarch64-linux-gnu/"
# e remova qualquer outra coisa que tenha vindo junto nesse dir
find "$SR/usr/include/aarch64-linux-gnu" -mindepth 1 -maxdepth 1 ! -name SDL2 -exec rm -rf {} +

# libs (siga os SONAMEs; --copy-links resolve os symlinks)
sshpass -p ark rsync -az --copy-links -e "ssh -o StrictHostKeyChecking=no" \
  $D:/usr/lib/aarch64-linux-gnu/{libSDL2.so,libSDL2-2.0.so.0,libpng16.so,libpng16.so.16,libcurl.so,libcurl.so.4,libEGL.so,libEGL.so.1,libGLESv2.so,libGLESv2.so.2,libz.so,libz.so.1,libdl.so.2,libpthread.so.0,libm.so.6,libc.so.6} \
  "$SR/usr/lib/aarch64-linux-gnu/"
```

Não copie as deps transitivas (openssl, nghttp2/3, brotli, libsamplerate, libdrm, …): no
link usamos `--allow-shlib-undefined` (elas existem no device em runtime).

## Passo 2 — compilar

O ponto chave: passar as flags de include/lib **explícitas** (os `?=` do Makefile do
retrorun permitem override) e usar `-Wl,--allow-shlib-undefined`.

### Retrorun (build SDL, `build/linux-sdl`)

```bash
SR=/tmp/opencode/r36-sysroot
cd <retrorun>
make -C build/linux-sdl config=release clean
make -C build/linux-sdl config=release -j2 \
  CXX=aarch64-linux-gnu-g++-13 CC=aarch64-linux-gnu-gcc-13 \
  SDL_CFLAGS="-I$SR/usr/include/SDL2 -I$SR/usr/include/aarch64-linux-gnu -D_REENTRANT" \
  SDL_LIBS="-L$SR/usr/lib/aarch64-linux-gnu -Wl,--allow-shlib-undefined -lSDL2" \
  PNG_CFLAGS="-I$SR/usr/include" \
  PNG_LIBS="-L$SR/usr/lib/aarch64-linux-gnu -lpng16" \
  GLES_CFLAGS="-I$SR/usr/include" \
  GLES_LIBS="-L$SR/usr/lib/aarch64-linux-gnu -lEGL -lGLESv2"
```

- `$(CC)` é usado para os `.c` (rcheevos/libchdr) — por isso `CC=` também.
- **Não** passe `LDFLAGS=` na linha de comando: isso sobrescreve o `LDFLAGS` inteiro do
  Makefile (que monta `$(SDL_LIBS) $(PNG_LIBS) $(GLES_LIBS) -lcurl -lz -ldl -pthread`).
  O `-L` do sysroot vai embutido no `SDL_LIBS`.

### Core flycast

O Makefile do flycast já tem o caminho; ver `CLAUDE.md`:

```bash
make platform=arm64 CC_PREFIX=aarch64-linux-gnu- \
  CXX=aarch64-linux-gnu-g++-13 CC=aarch64-linux-gnu-gcc-13 \
  CC_AS=aarch64-linux-gnu-g++-13 HAVE_OPENMP=0 LDFLAGS="-L." -j2
```
(ele precisa do `libGLESv2.so` copiado para o dir do repo, `LDFLAGS="-L."`).

## Passo 3 — verificar e deployar

```bash
file retrorun-sdl2        # tem que dizer: ELF 64-bit ... ARM aarch64
sshpass -p ark scp retrorun-sdl2 ark@192.168.0.14:/home/ark/retrorun3_test
sshpass -p ark ssh ark@192.168.0.14 'chmod +x /home/ark/retrorun3_test'
```

## Erros clássicos (e a causa)

| sintoma | causa | fix |
|---|---|---|
| `'__time64_t' does not name a type`, `'time_t' does not name a type` | copiou os headers do glibc do device (2.41) junto com o toolchain (2.39) | só headers de terceiros no sysroot |
| `fatal error: SDL2/_real_SDL_config.h: No such file` | `SDL_config.h` do Debian indireciona p/ o dir multiarch | copiar `aarch64-linux-gnu/SDL2/` e adicionar `-I.../usr/include/aarch64-linux-gnu` |
| `referência não definida para "SSL_..." / "nghttp2_..." / "src_process@libsamplerate"` | deps transitivas do `libcurl`/`libSDL2` fora do sysroot | `-Wl,--allow-shlib-undefined` no link |
| `símbolo local no índice N (>= sh_info de 3)` em `libGLESv2.so`/`libEGL.so` | `.dynsym` das libs Mali | inofensivo (só aviso), como no build do core |
| binário roda e morre com `Syntax error: "(" unexpected` | deployou um binário x86_64 | confira `file` — tem que ser ARM aarch64 |

## Notas

- O device tem os headers e libs dev em `/usr/include` e `/usr/lib/aarch64-linux-gnu`
  (não precisa de `-dev` package).
- Se o link reclamar de um símbolo que **é** seu (não de uma dep), aí sim a lib está
  faltando no sysroot — copie a `.so` correspondente.
