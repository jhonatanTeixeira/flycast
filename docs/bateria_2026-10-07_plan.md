# Plano de ataque — bateria DC + Naomi de 2026-10-07

Base: bateria pelo ES com o core oficial (`flycast2026` md5 `961f9a55`) e `retrorun3`
`53040f37`. Números e observações em `docs/game_status.md` (seção "Bateria DC + Naomi —
2026-10-07"). Capturas em `/roms2/dcbat/20261007-15xx…16xx_<jogo>/` (dump leve do JIT +
sync-stats + perf + benchmark rolante).

Ordem por **impacto × evidência**: primeiro o que quebra (crash/freeze), depois o que
estraga a experiência (áudio, dupes), depois o que é otimização.

---

## P0 — Crashes e freezes (a regra da bateria manda parar aqui)

### 0.1 `SH4ThrownException` não tratada → SIGABRT (SA2, Shenmue) — **SA2 = tier2, CONFIRMADO**

- **A/B feito (2026-10-07):** SA2 com `flycast2026_tier2 = enabled` → **exit 134**
  (`iNimp 0010 @ pc 8c501bba` → `terminate`); com `tier2 = disabled` → **exit 124** (só o
  timeout de 90 s, **sem crash**). **A causa é o tier2** — uma região que faz o jogo
  **executar dados** (mesma família do bug 4.74, o laço com store no delay slot).
- **Evidência completa:** `iNimp 0010 @ pc 8c501bba (illegal instruction -- executing data
  or stale code?)` + `iNimp ctx: pr=8c062f92 spc=8c023e7c ...`. O PC ruim (`8c501bba`) **não**
  está em nenhuma das regiões emitidas (#14-17 são 8C0A/8C0C) → vem de um **exit** de região.
- **Fixes (dois):** (a) **robustez** — pegar o `SH4ThrownException` que escapa e chamar
  `Do_Exception(epc,0x180,0x100)` em vez de terminar (o guest trata a exceção); (b) **raiz** —
  achar a região/condição que faz o jogo executar dados e rejeitá-la (como o 4.74 faz com
  `slotStore`). Sem o A/B original (abaixo).

- **Evidência (histórico):** os logs de SA2/Shenmue terminam com `terminate called after
  throwing an instance of 'SH4ThrownException'` (exit 134). O **Napple não crashou** (o
  `exit 134` da pasta dele é suspeito — provavelmente no shutdown). Skies D2 é outra
  assinatura (ver 0.2).
- **O que é:** o SH4 lança `SH4ThrownException` (exceção do guest: `sh4_opcodes.cpp:2111`
  `{0x180}`, `mmu.cpp:145`, `sh4_core.h:95` `{0x800}`) e algum caminho **não tem
  try/catch**. Os catch existem no `rec_arm64` (750/766), no interpretador
  (`sh4_interpreter.cpp` 64/151/167), no `driver.cpp:206`, no `blockmanager.cpp:1412` e
  no `gdrom_hle.cpp` (570/603) — falta um.
- **Como atacar:** reproduzir cada jogo (Shenmue no loading entre lugares, SA2 após os
  logos, Napple ao salvar) e capturar o **backtrace** (o `live.log` tem o `samples.txt.gz`
  do `perf` da sessão inteira; e o core tem o handler de SIGSEGV em
  `libretro/common.cpp:492`/`3920`). Achar qual `throw` escapa e onde (provavelmente
  caminho de HLE/`hle_fn` ou de exceção dentro de bloco do JIT).
- **Sinais nos logs:** `perf_record.log` + `samples.txt.gz` de cada pasta.

### 0.2 Skies Disc 2 — SIGSEGV no dyna code

- **Evidência:** `SIGSEGV @ … (nil) -> was not in vram (dyna code 0)` + `die(): segfault`
  + `DEBUGBREAK!` (`common.cpp:492`, `libretro.cpp:3920`), exit 133. **Duas** tentativas.
- **Hipótese:** o usuário suspeita do CHD (disc 2). Antes de culpar o emulador, validar o
  CHD (rodar com o core upstream / verificar o dump). Se o CHD estiver bom, é bug do
  dynarec/JIT (`was not in vram` = PC executado fora da VRAM).
- **Ação:** (1) validar o CHD; (2) se bom, achar o PC/opcode com o `FC_JIT_TRACE`/`FC_JIT_DUMP`
  da pasta.

### 0.3 Presos num teto de tempo de frame (EGG, Macross M3)

- **Evidência:** EGG **89,6 ms/frame** (p50 89,5, p95 89,8) com **442/443 dupes**; Macross
  M3 **144,7 ms** (p50 150,4) com 271/275 dupes. Assinatura de **espera/timeout**, não de
  CPU (mesma cara do achado 4.73: "100,5 ms cravado = espera de 100 ms").
- **Contexto:** EGG "travou após load do save"; Macross "tela preta após o logo ShoEISHA".
- **Como atacar:** instrumentar/olhar o `rs.Wait`/timeout do render (`Renderer_if.cpp`) e o
  laço principal; ver se é CHD (`FC_CHD_PREFETCH`), AICA (`FC_AICA_THREAD`) ou o frame-wait
  (4.110). A/B por env var na mesma build, lendo o `live.log` + o JSON.

### 0.4 TR Chronicles — tela preta após a BIOS (**mesma família do 0.3: EGG/Macross**)

- **Evidência:** sem crash (exit 0), sem JSON; log normal. BIOS diferente ("powered by
  Windows CE" — WinCE HLE). "Trava em tela preta".
- **Como atacar:** testar com `flycast2026_boot_to_bios`/`hle_bios` e ver se é o HLE do
  WinCE; comparar com o core upstream.

---

## P1 — Estraga a experiência

### 1.1 Áudio: "som estourado" + "vozes muito baixas" (Grandia II, Napple)

- **Evidência:** Grandia II — FMV 42 fps (dupes → desafinado) mas gameplay 30 cravado;
  usuário relata música/efeitos **saturando** e vozes **muito baixas**. Napple idem.
  **Overruns altíssimos:** Grandia II **8278**, Skies D1 **12550**.
- **Hipótese:** o WSOLA (4.113) preserva o tom, mas algo satura/mistura errado; ou o
  volume/mixer (o `reicast_audio_mixer`/`volume_modifier`) ou o buffer (overruns). Separar:
  (a) só o WSOLA ligado/desligado (`RETRORUN_AUDIO_TIME_STRETCH=0/1`); (b) o mixer do core
  (accurate × lowend); (c) a taxa (o desafinado do FMV a 42 fps).
- **Ação:** A/B de áudio no Grandia II (FMV + gameplay) e no Napple, ouvindo ao vivo.

### 1.2 Memory card — "falta de espaço" (Napple, Shenmue II) + crash do Napple ao salvar

- **Evidência:** os dois reclamam de espaço no VMU; o Napple **crashou (exit 134)** na
  sessão em que o usuário tentou salvar. Achávamos que havia **VMU por jogo**.
- **Como atacar:** ver o `per_content_vmus` (`flycast2026_per_content_vmus = VMU A1` no
  cfg) e como o nome/arquivo do VMU é resolvido por jogo; confirmar se todos os jogos
  caem no mesmo VMU. O crash ao salvar pode ser o mesmo `SH4ThrownException` (0.1) ou
  algo do save.

### 1.3 Regressões: MvC2 (glitches + 33-45 fps) e Project Justice (dupes)

- **Evidência:** MvC2 47,1 fps médios, p95 39 ms, **glitches**, "regressão"; Project
  Justice **118 dupes**/2156 e "passa de 60 fps". Power Stone 45 fps (36 dupes),
  Soulcalibur 84 dupes.
- **Pedido do usuário:** usar o **JIT do MvC2** para achar melhorias de processamento
  **independentes do peso das chamadas** (talvez via HLE) — ou seja, o gargalo pode ser
  overhead por bloco/chamada, não trabalho útil.
- **Ação:** (1) dupes — casar apresentação × `new_fps` (o core agora reporta o fps natural;
  ver 4.112); (2) glitches — isolar o tier2 (A/B `flycast2026_tier2 = enabled/disabled`);
  (3) seguir o ciclo `jit-nativo` no MvC2.

### 1.4 Sonic Shuffle — o "7 fps" com p95 141 ms

- **Evidência:** 68,1% VEL, 14,8 fps, p95 **141,3 ms**, p99 147,9; savestate do usuário numa
  cena a 7 fps.
- **Como atacar:** é a frente do "skip do laço de varredura" (escrito, nunca validado) e/ou
  do tier2. Analisar o JIT dele (dump leve) e achar o laço quente; HLE/tier2.

### 1.5 Le Mans — load muito demorado + menus lentos

- **Evidência:** corrida ok (26-30 fps), mas o load e os menus são lentos. Suspeita: I/O do
  CHD (`FC_CHD_PREFETCH`) ou o carregamento inicial.

---

## P2 — Otimização / "nunca tínhamos aberto"

### 2.1 Evolution 2 — 30 → 24 fps nas quedas

- Jogável; quedas para 24 fps; o frameskip "dá uma melhorada". Candidato a HLE/tier2.
- Verificar save/load (o usuário suspeita de problemas de save em alguns jogos).

### 2.2 KOF Evolution — a chuva (54 → 32 fps)

- 60 fps fora da chuva; a chuva instabiliza. Novo savestate deixado nessa cena.

### 2.3 DOA2 — "quase jogável" (88,3% VEL)

- Já diagnosticado como emu-bound (SH4). Frente do laço de vértices (skill `jit-nativo`).

### 2.4 cvs2 / Power Stone / Soulcalibur — dupes/hickups

- cvs2: "hickups que antes não tinha" (58,3 fps, p95 30,8 ms). Power Stone 45 fps (dupes).
  Soulcalibur 60 cravado mas 84 dupes. Casar apresentação × novos.

---

## Método (vale para todos)

1. **Mesma cena/savestate** em qualquer A/B; sempre **fps E frame time**, em
   **p50/p95/p99 E média**, e a **VEL%**; olhar **apresentado × novos × dupes**.
2. **Medir no device** entre `perfmax performance` e `perfnorm`.
3. **Commitar a cada marco** validado (código + docs).
4. Registrar achados em `tech_debits.md` e a sessão em `history.md`.

## Primeiro passo sugerido

**0.1 (o `SH4ThrownException`)** — porque: (a) são 3 jogos da lista (Shenmue, SA2, Napple);
(b) a assinatura é inequívoca (exceção do guest escapando); (c) provavelmente é **uma**
correção para os três; (d) temos o backtrace nos `samples.txt.gz` das capturas. Em
paralelo, o **0.3** (EGG/Macross) tem a mesma cara do 4.73 (espera de ~100-150 ms) e é
outro "uma correção, dois jogos".
