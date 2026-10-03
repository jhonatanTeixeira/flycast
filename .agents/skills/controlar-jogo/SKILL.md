---
name: controlar-jogo
description: Controlar um jogo rodando no device R36 pelo socket do core (FC_CTRL_PORT) — ver a tela, apertar botões, mexer analógicos com força, jogar pausado passo a passo e amostrar onde o SH4 está girando. Use quando precisar reproduzir um bug no device sem o usuário segurar o controle (ex.: chegar numa tela que trava), navegar menus, levar o personagem a um ponto, conferir visualmente o efeito de uma mudança, ou quando o usuário pedir para "jogar", "apertar", "ir até", "abrir o menu" ou "ver a tela" do jogo.
---

# Controlar o jogo pelo socket do core

O core flycast deste repositório tem um servidor TCP opcional
(`core/libretro/ctrl_socket.cpp`, referência completa em `docs/ctrl_socket.md`).
Com ele você vê a tela e mexe no controle 1 do jogo rodando no device. O controle
físico continua valendo: o socket soma botões com ele.

## 1. Abrir o jogo com o socket

Device: `ark@192.168.0.14` (senha `ark`, via `sshpass`). Veja também a skill
`rodar-games` (backup de cores, logs, `perfmax`).

```bash
timeout 60 sshpass -p ark ssh ark@192.168.0.14 '
  pkill -x retrorun3; sleep 3; cd /home/ark
  export SDL_VIDEO_EGL_DRIVER=libEGL.so DEVICE_NAME=RG351MP FC_CTRL_PORT=5555 \
         RETRORUN_VSYNC=1 RETRORUN_SDL_THREADED_PRESENT=1 RETRORUN_PRESENT_DEPTH=2
  sudo perfmax performance x >/dev/null 2>&1
  nohup /usr/local/bin/retrorun3 -c /home/ark/.config/retrorun.cfg --triggers \
    -s /roms2/dreamcast -d /roms2/bios <core.so> "/roms2/dreamcast/<jogo>.chd" \
    > /home/ark/live_<jogo>.log 2>&1 < /dev/null & disown; echo aberto'
```

- `<core.so>` precisa ser um build com o socket (commit `eebcf4b4d` em diante).
  `/home/ark/flycast_ctrl.so` é um build de teste com ele.
- **Sempre passe `-c <cfg>`.** Sem ele o retrorun procura `/home/ark/retrorun.cfg`,
  não acha e sobe com as opções padrão do core (tier2 desligado, por exemplo). A
  rodada fica diferente da do ES sem aviso nenhum.
- Para começar de um savestate: copie o cfg com `retrorun_auto_load = true`. O
  savestate é `/roms2/dreamcast/<jogo>.fc2021-rrstate.auto`.
- Para fechar o jogo, use **`pkill -x retrorun3`**. Nunca `pkill -f "retrorun3 ..."`:
  o `-f` casa com a linha de comando do próprio ssh e mata a sua sessão no meio.
- Espere ~25-30 s depois de abrir antes de conectar. A linha
  `ctrl socket: ouvindo na porta 5555` aparece no log.

## 2. Cliente

```bash
P=/home/jhonatanteixeira/.pyenv/shims/python3   # o python do PATH (venv) não tem Pillow
$P tools/fc_ctrl.py shot tela.png
$P tools/fc_ctrl.py do "LS(up,40):1s; A"
$P tools/fc_ctrl.py run "mode step | do A > a.png | do DOWN; A > b.png | mode live"
```

`run` separa comandos com `|`, porque o `;` pertence à sequência. Depois de cada
`shot` ou passo, **olhe a imagem** (Read no PNG). É a única forma de saber o que
aconteceu.

## 3. Linguagem de sequência (`do`)

Passos separados por `;`, em ordem. Elementos juntados por `+` são apertados ao
mesmo tempo.

| Elemento | |
|---|---|
| `A B X Y START` | botões do **Dreamcast** |
| `UP DOWN LEFT RIGHT` (`U D L R`) | direcional |
| `LS(up,40)`, `LS(-30,80)`, `RS(...)` | analógico: direção + força %, ou X,Y em % |
| `LT`, `RT(60)` | gatilhos com força % |
| `wait` | passo vazio |

Modificadores no fim do passo: `:3s` / `:500ms` / `:20f` (segura), `*5` (repete),
`/200ms` (solto entre repetições). Sem modificador é um toque de 6 leituras
apertado + 6 solto (`set tap ON OFF` muda isso).

O tempo em `s`/`ms` é o tempo **emulado**: com o jogo lento, o passo dura mais na
parede e o mesmo no jogo. `f` = leituras do controle (~1 por quadro).

## 4. Modo pausado (o laço de quem está jogando)

```
mode step                      -> jogo congela
do LS(ur,100):800ms > p1.png   -> anda, congela de novo, a resposta JÁ É a tela
do A > p2.png
mode live                      -> SEMPRE antes de fechar o jogo
```

No modo step, `do` bloqueia até a sequência terminar e o jogo parar, e responde
`PPM <bytes> ok N leituras` + a imagem do resultado (~0,5 s por ciclo). Com a
emulação parada o som fica mudo, e fechar o jogo nesse estado pode travar.

## 5. Para debugar

- `pc 2000` amostra PC e PR do SH4 a cada 1 ms e lista os mais frequentes. Serve
  para ver onde o jogo gira quando trava (tela preta, laço de espera). Compare
  com a mesma amostra antes do problema. O PC do contexto só muda quando o JIT
  passa pelo despachante; o PR (endereço de retorno) costuma ser mais estável.
- Reproduzir um bug: chegue no ponto pelo socket uma vez, anote a sequência que
  funcionou e repita em cada build/opção (A/B com tier2 ligado/desligado, core
  antigo de `/home/ark/.config/retroarch/cores/*.bak-*`, etc.).
- O `perf` da captura (`rr_capture.sh`) ocupa o processo. Um segundo
  `perf record -p` falha; nesse caso use `pc`.
- Status e fila: `status`.

## 6. Lições de quem já usou (Grandia II, 2026-10-02)

- **Ande em passos curtos e confira a tela a cada um.** O personagem anda em
  relação à câmera, e a câmera segue o personagem. Chutar a distância inteira de
  uma vez erra por "alguns centímetros".
- Para interagir com objetos (cristal de save, NPC), o personagem precisa estar
  **dentro/colado**. Se A não faz nada, chegue mais perto antes de trocar de botão.
- Os nomes são os botões do Dreamcast. No Grandia II, `A` abre/confirma e o `Y`
  troca a câmera ("FORWARD"). Não deduza o mapeamento do botão físico do R36 pela
  tabela do core sem testar (isso já deu errado).
- Toque curto (4 leituras) às vezes não registra em jogos com lógica a 30 fps.
  Use o padrão (6) ou `:10f`.
- Se o usuário estiver segurando o device, avise antes de mexer. Os dois
  controles somam.
- Identificar objetos na tela costuma funcionar bem. Posição exata, tempo e
  botão certo se acertam tentando e olhando de novo.
