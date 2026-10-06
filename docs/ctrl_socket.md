# Socket de controle do core (`FC_CTRL_PORT`)

Servidor TCP dentro do core para pegar a tela e mexer no controle 1 de fora.
Serve para reproduzir um caminho no device sem ninguém segurando o controle e é a
base para uma LLM jogar. Código: `core/libretro/ctrl_socket.cpp`. Cliente:
`tools/fc_ctrl.py`.

Liga só com a variável: `FC_CTRL_PORT=5555`. Sem ela o core não muda.

## Comandos (uma linha cada)

| Comando | O que faz |
|---|---|
| `do SEQUENCIA` | executa a sequência e bloqueia até ela terminar. Normal: responde `ok N leituras`. Pausado: responde `PPM <bytes> ok N leituras` + a tela do resultado |
| `mode step` | pausa o jogo; cada `do` despausa, executa, pausa de novo e devolve a tela |
| `mode live` | volta ao normal |
| `step N` | (modo step) anda N quadros e pausa de novo, devolvendo a tela. `step 0` só congela. Serve para deixar cutscenes/transições andarem sozinhas sem sair do step |
| `shot` | `PPM <bytes>\n` + imagem P6; pausado, devolve o quadro em que parou |
| `set tap ON OFF` | toque padrão: leituras apertado / solto (padrão 6/6) |
| `status` | leitura atual, modo, fila |
| `pc N` | amostra PC e PR do SH4 N vezes (1 ms): onde o jogo está girando |
| `press K N`, `hold K`, `release`, `stick X Y`, `wait N` | comandos antigos |

## Sequência

Passos separados por `;`, executados em ordem. Cada passo junta elementos com `+`
(apertados ao mesmo tempo) e pode ter modificadores no fim.

| Elemento | Significado |
|---|---|
| `A B X Y START` (`S`) | botões do Dreamcast |
| `UP DOWN LEFT RIGHT` (`U D L R`) | direcional |
| `LT`, `RT`, `LT(60)` | gatilho, força em % |
| `LS(up,40)` / `LS(-30,80)` | analógico esquerdo: direção + força %, ou X,Y em % |
| `RS(...)` | analógico direito |
| `wait` | passo vazio |

Direções do analógico: `up down left right upleft(ul) upright(ur) downleft(dl)
downright(dr) center`.

| Modificador | Significado |
|---|---|
| `:3s` `:500ms` `:20f` | segura por esse tempo (`f` = leituras do controle) |
| `*5` | repete 5 vezes |
| `/200ms` | tempo solto depois de cada repetição |

Exemplo: `do LS(up,35):2s; A; wait:1s; A+B*3/100ms; LT(100)+RIGHT:20f`

- O tempo em `s`/`ms` é o tempo **emulado** (relógio do SH4): com o jogo lento, o
  passo dura mais na parede e o mesmo no jogo.
- Os nomes são os botões do Dreamcast. No Grandia II, o `A` do socket abriu o
  cristal de save e confirmou o menu, o mesmo que o A físico do R36 faz.
- A sequência só anda quando o jogo lê o controle. Em tela de carregamento que
  não lê, ela espera.
- O controle de verdade continua valendo; enquanto um passo usa o analógico, o
  eixo do socket substitui o físico.

## Modo step

O `do` bloqueia quem pediu até a sequência terminar e o jogo parar, e a resposta
já é a imagem do resultado (um ciclo dá ~0,5 s no Wi-Fi, com a imagem). No
cliente, `do SEQ > arquivo.png` salva a imagem; sem `> arquivo`, ele numera
`passo_001.png`, `passo_002.png`...

A thread de emulação fica parada dentro da leitura do controle. Antes de parar,
o core pede um quadro ao render, e esse quadro é o que o `shot` devolve. O som
fica mudo e o frontend repete o último quadro. **Volte para `mode live` antes de
fechar o jogo**: com a emulação parada, o fechamento pode travar.

## Cliente

```
python3 tools/fc_ctrl.py shot tela.png
python3 tools/fc_ctrl.py do "LS(up,35):2s; A"
python3 tools/fc_ctrl.py run "mode step | do A > a.png | do DOWN; A > b.png | mode live"
```

`run` separa comandos com `|`, porque o `;` pertence à sequência.

Validado no Grandia II (2026-10-02): pausa e retomada, analógico a 30% e a 100%
(passo curto e corrida), Y com repetição, combinação analógico+B, erros de sintaxe.
