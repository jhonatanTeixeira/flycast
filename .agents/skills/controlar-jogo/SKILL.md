---
name: controlar-jogo
description: Ensinar a jogar/controlar um jogo rodando no emulador pelo socket do core (FC_CTRL_PORT) — ver a tela, apertar botões, mexer nos analógicos, pausar e despausar o jogo, avançar N quadros (cutscenes) e conferir cada passo por imagem. Use sempre que precisar pilotar um jogo sem ninguém segurando o controle: navegar menus, andar até um ponto, avançar uma cutscene, apertar um botão e ver o resultado, ou quando o usuário pedir "joga", "aperta", "vai até", "abre o menu", "passa essa cena" ou "mostra a tela" do jogo.
---

# Controlar o jogo pelo socket do core

O core flycast deste repositório tem um servidor TCP opcional
(`core/libretro/ctrl_socket.cpp`, referência completa em `docs/ctrl_socket.md`).
Com ele você **vê a tela** e **mexe no controle 1** do jogo que está rodando. É a
base para uma LLM jogar sozinha. O controle físico continua valendo: o socket
**soma** os botões com ele.

A ideia central: você age em passos pequenos e **olha a imagem depois de cada
passo**. É assim que se joga — não tem como saber o que aconteceu sem ver a tela.

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
  não acha e sobe com as opções padrão do core. A rodada fica diferente da do ES
  sem aviso nenhum.
- Para começar de um savestate: copie o cfg com `retrorun_auto_load = true`. O
  savestate é `/roms2/dreamcast/<jogo>.fc2021-rrstate.auto`.
- Para fechar o jogo, use **`pkill -x retrorun3`**. Nunca `pkill -f "retrorun3 ..."`:
  o `-f` casa com a linha de comando do próprio ssh e mata a sua sessão no meio.
- **Volte para `mode live` antes de fechar** (com a emulação parada o fechamento
  pode travar).
- Espere ~25-30 s depois de abrir antes de conectar. A linha
  `ctrl socket: ouvindo na porta 5555` aparece no log.

## 2. Cliente (`tools/fc_ctrl.py`)

```bash
P=/home/jhonatanteixeira/.pyenv/shims/python3   # o python do PATH (venv) não tem Pillow
$P tools/fc_ctrl.py shot tela.png
$P tools/fc_ctrl.py do "LS(up,40):1s; A"
$P tools/fc_ctrl.py step 120            # modo step: anda 120 quadros (cutscene)
$P tools/fc_ctrl.py run "mode step | do A > a.png | step 60 | do DOWN; A > b.png | mode live"
```

`run` separa comandos com `|`, porque o `;` pertence à sequência. Depois de cada
`shot`, `do` ou `step`, **olhe a imagem** (Read no PNG). É a única forma de saber
o que aconteceu.

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

## 4. Modo pausa: ligar, andar e desligar

O jeito de jogar sem pressa é **pausar** o jogo; cada ação sua anda o jogo o
necessário e pausa de novo, devolvendo a tela pronta.

```
mode step                      -> pausa o jogo (congela)
do "A" > a.png                 -> aperta A, o jogo anda sozinho o necessário e pausa;
                                  a resposta JÁ É a tela do resultado
step 60 > cut.png              -> anda 60 quadros (cutscene/transição) e pausa
step 0                         -> só garante que está parado
mode live                      -> despausa: o jogo volta a andar sozinho
```

- **Ligar:** `mode step`. **Desligar:** `mode live` (SEMPRE antes de fechar).
- No modo step, `do` bloqueia até a sequência terminar e o jogo parar, e responde
  `PPM <bytes> ok N leituras` + a imagem do resultado (~0,5 s por ciclo).
- **`step N`** (só no modo step): deixa o jogo andar **N quadros** e pausa de
  novo, devolvendo a tela. Use para deixar cutscenes/transições andarem sozinhas
  sem sair do step. `step 0` só congela.
- `step` fora do modo step responde `ERR step so no modo step`.
- Com a emulação parada o som fica mudo, e fechar o jogo nesse estado pode travar.

## 5. O laço de quem está jogando

1. **Veja a tela** (`shot`) e entenda onde você está.
2. **Pause** (`mode step`) para agir com calma.
3. **Dê um passo curto** (`do "..."` ou `step N`) e **olhe a imagem** que voltou.
4. **Repita** até chegar onde queria; a cada passo, reavalie pela imagem.
5. **Despause** (`mode live`) quando quiser deixar o jogo correr (cutscene longa,
   espera) e para fechar.

Dicas que economizam tentativa e erro:

- **Ande em passos curtos e confira a tela a cada um.** O personagem anda em
  relação à câmera, e a câmera segue o personagem (Grandia II, 2026-10-02).
  Chutar a distância inteira de uma vez erra por "alguns centímetros".
- Para interagir com objetos (cristal de save, NPC), o personagem precisa estar
  **dentro/colado**. Se `A` não faz nada, chegue mais perto antes de trocar de botão.
- Os nomes são os botões do Dreamcast. No Grandia II, `A` abre/confirma e o `Y`
  troca a câmera ("FORWARD"). Não deduza o mapeamento do botão físico do R36 pela
  tabela do core sem testar (isso já deu errado).
- Toque curto (4 leituras) às vezes não registra em jogos com lógica a 30 fps.
  Use o padrão (6) ou `:10f`.
- Cutscene/tela parada no tempo: `mode live` deixa correr; se quiser controlar,
  `step N` avança N quadros por vez.
- Se o usuário estiver segurando o device, avise antes de mexer. Os dois
  controles somam.
- Identificar objetos na tela costuma funcionar bem. Posição exata, tempo e
  botão certo se acertam tentando e olhando de novo.
- `status` mostra a leitura atual, o modo (live/step pausado) e a fila — útil para
  conferir se está pausado antes de fechar.
