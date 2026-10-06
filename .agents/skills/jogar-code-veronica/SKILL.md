---
name: jogar-code-veronica
description: Jogar Resident Evil - Code: Veronica no emulador com memória do jogo. Cobre (1) como pilotar o jogo pelo socket de controle (ver a tela, andar, atirar, abrir porta, usar item, salvar) e (2) como construir e consultar uma memória RAG da partida (salas, itens, portas, onde ficou o quê) para dar backtrack sem perder o fio — o jogo é labiríntico e exige voltar muito. Use quando o usuário pedir para jogar/avançar Code Veronica, "achar o caminho", "voltar numa sala", "onde estava o item X", "o que tinha nesta sala", ou montar/consultar a memória da partida.
---

# Jogar Code: Veronica (com memória da partida)

Este skill junta **duas coisas**: saber *pilotar* o jogo (a mecânica está no skill
`controlar-jogo`) e saber *lembrar* o que já vimos (a memória, que este skill
define). Code: Veronica é labiríntico e obriga a **backtrack** constante: o valor
está em saber onde tem item box, onde ficou aquele item que não cabia, qual porta
faltava a chave. Isso não cabe no contexto — vai para o RAG.

## 0. Antes de tudo

- **Pilotagem** (botões, live/step, `step N`, ver a tela): leia **`controlar-jogo`**.
  Este skill assume que você já sabe usar o `tools/fc_ctrl.py` e o socket
  (`FC_CTRL_PORT`).
- **Memória de imagem** (gravar/buscar cena): leia **`image-graph-rag`**. As
  ferramentas são do MCP `mcp-vision`; a instrução de uso é a mesma — só mudam a
  árvore e o tipo de conteúdo (salas de jogo).
- **Visão da cena**: o melhor legendador é **`qwen`** (com o prompt detalhado do
  servidor). O **WD tagger** (`/api/tag`) acerta atributos do personagem (roupa,
  cabelo, pose), mas **alucina cor** (marca `blood` num vermelho de jaqueta/munição).
- Nenhum modelo nomeia bem **itens de cenário** ("caixa de munição", "cinzeiro",
  "esqueiro", "frasco"). Por isso, ao gravar uma sala, **escreva você mesmo o
  `text`** nomeando os itens (o `text` vai para `extra_text` e é recuperável).

## 1. Subir o jogo com o socket

No notebook (x64), com o core do upstream que tem o socket:

```bash
ssh notebook 'pkill -9 -x retroarch; sleep 2; cd ~
  export DISPLAY=:0 FC_CTRL_PORT=5555
  setsid nohup retroarch --verbose \
    -L ~/.config/retroarch/cores/flycast_upstream_libretro.so \
    "/home/jhonatanteixeira/Downloads/roms/dreamcast/Resident Evil - Code - Veronica (USA) (Disc 1).chd" \
    > ~/live_recv.log 2>&1 < /dev/null & disown; echo aberto'
```

- Espere ~25 s e confirme `ctrl socket: ouvindo na porta 5555` no log.
- O **shot sai em PNG 640×480** (o socket reduz sozinho). ~250 ms por shot.
- Cliente: `python3 tools/fc_ctrl.py --host 192.168.0.16 --port 5555 ...`
  (`shot`, `do`, `mode step`, `step N`).
- O manual do jogo está em
  `~/Downloads/roms/dreamcast/915-resident-evil-code-veronica-dreamcast-manual-usa.pdf`
  (24 páginas escaneadas; use OCR — tesseract `-l eng` funciona bem).

## 2. Conhecimento do jogo (do manual oficial)

### Controles (default)
| Botão | Ação |
|---|---|
| **X** | Ação / confirmar (check item, abrir porta, atacar, subir/descer escada) |
| **A** | Correr (com direcional) / cancelar |
| **B** | **Status Screen** (inventário/condição) / cancelar |
| **Y** | **Map Screen** |
| **R (trigger)** | Sacar arma (segurar) |
| **L (trigger)** | Trocar de alvo (com R segurado) |
| **D-pad / analógico** | Mover; no menu, navegar |
| **A+B+X+Y+START** | Volta à tela de título (soft reset) |

### Ações (Character Actions)
- **Atacar:** segurar **R** e apertar **X** (precisa de arma equipada). Mirar
  cima/baixo com o direcional; algumas armas abrem **Scope Screen** (1ª pessoa).
- **Empurrar objeto:** encostar e segurar o direcional na direção do objeto.
- **Subir/descer de objeto:** encarar e apertar **X**.

### Status Screen (B) — o inventário
- Mostra condição do personagem, **lista de itens**, item equipado e comandos.
- Com o item destacado: **USE** (usar), **EQUIP** (armas — sem equipar não ataca),
  **CHECK** (girar/zoom com L/R; pode revelar pistas de puzzle), **COMBINE**
  (combinar, ex. Handgun + componente → Handgun custom).
- Capacidade **limitada** de itens: o que não couber vai para o **Item Box**.

### Item Box
- Baú para guardar itens que não cabem. **Itens não se perdem** (só se gastam,
  como munição). Ficam espalhados pelo mapa.

### Mapa (Y) — o coração do backtrack
- A área explorada é marcada; sua posição é `>`. Trocar andar com o direcional
  (cima/baixo) e mudar de estágio com esquerda/direita. Zoom: **X** (L/R dão zoom).
- **Cor das portas:**
  - **Amarela** = destrancada.
  - **Vermelha** = trancada e **você não tem a chave**.
  - **Verde** = trancada mas **você tem a chave**.
- O mapa também marca: **ITEM** (achado e não pego), **SAVE POINT** (typewriter),
  **ITEM BOX**. A sala atual aparece em vermelho.
- **FILE:** notas/mensagens ficam no "notebook" e dão dicas de puzzle.

### Save / load (typewriter)
- Achar uma **ink ribbon** e usar numa **typewriter**; **X** na máquina, escolher
  YES. Cada save **consome uma ink ribbon**. Exige VMU (no emulador, é o savestate
  do core).

### Dano / fim de jogo
- Dano contínuo; se passar do limite → Game Over. **Alguns inimigos envenenam** (dano
  gradual — cure com item). Se o **parceiro morre, o jogo acaba**. Em Game Over:
  RETRY do último save.

### Troca de personagem
- Às vezes você controla o parceiro (ex. Steve); a Status muda junto, controles iguais.

## 3. Memória da partida (RAG) — árvore `re-code-veronica`

Tudo numa árvore só: **`re-code-veronica`** (embedding `e5`). Cada **sala** é
gravada como uma imagem (o shot) + um `text` que você escreve.

### Gravar uma sala (quando entra numa sala nova ou muda de estado)

```json
{
  "tree": "re-code-veronica",
  "embedding_model": "e5",
  "image": "<shot da sala, PNG>",
  "text": "Área <nome/andar>. <o que vejo>. Itens visíveis: ... . Portas: <cor/estado>. Saídas: ... .",
  "options": { "caption": true, "caption_model": "qwen", "faces": true, "objects": true, "poses": false, "exif": false },
  "term_extraction": { "engine": "gliner", "labels": ["item","arma","lugar","personagem","porta","chave"] }
}
```

Regras que economizam dor:

- **`text` é o que importa.** Nomeie os itens, o número da sala, as portas e as
  saídas. A extração automática (gliner/yake) **não pega tudo** — teste: numa cena
  real ela achou "cadeira/cinzeiro/cela" mas **perdeu** "caixa de munição",
  "isqueiro", "sacos de areia". O `text` fica guardado como `extra_text` e é o que
  a busca recupera.
- **Idempotência por sha256:** regravar a *mesma* imagem ignora o `text` novo. Se a
  sala mudou de estado (peguei item, abri porta), o shot muda → nova imagem; se a
  tela for idêntica, use uma árvore/arquivo diferente ou anexe via `graph_save`
  (`text`) para ligar termos à imagem existente.
- **Um shot por estado relevante**, não por frame. Grave ao entrar, ao pegar item
  importante, ao resolver puzzle.
- Combine com **tags do WD tagger** (`/api/tag`) só para o personagem (roupa/estado);
  ignore `blood`/`sensitive` como descrição de objeto.

### Consultar (backtrack)

| Pergunta | Ferramenta |
|---|---|
| "onde estava a caixa de munição?" | `memory_search` `query:"caixa de munição sala"` na árvore |
| "que sala é essa?" (tenho o shot) | `image_graph_search` `by:"image"`, `image:<shot>` |
| "imagens parecidas com esta sala" | `image_graph_search` `by:"image"` |
| "tudo que sei da sala X" | `graph_query` `terms:["<nome>","cela"]`, `include_images:true`, `walk:false`, `min_similarity` ~0.88–0.90 |
| detalhe de uma sala | `image_graph_get` (`id` ou `image`) |

- **Compare o shot atual com os antigos** (`image_graph_search by=image`) para
  reconhecer "já estive aqui". É o antídoto do backtrack às cegas.
- Ao voltar numa sala, **atualize a memória**: novo shot + `text` ("peguei a
  munição; porta agora verde").
- Mantenha um **mapa mental em texto** fora do RAG também: lista de salas
  conhecidas, o que tem em cada uma, portas trancadas e onde faltou chave. O RAG é
  para *relembrar*, não para substituir o plano.

### Sugestão de convenção de `text` (consistência ajuda a busca)

```
SALA: <nome informal/andar>. ESTADO: <o que mudou>. VEJO: <personagem, pose, luz>.
ITENS NO CHÃO/PAREDE: <lista>. MÓVEIS/CENÁRIO: <lista>. PORTAS: <amarela|vermelha|verde(+chave)|n/a>,
<destino se conhecido>. SAVE: <typewriter?>. ITEM BOX: <sim/não>.
```

## 4. Laço de jogar (memória + pilotagem)

1. **Veja a tela** (`shot`) e identifique a sala.
2. **Reconheça** a sala: `image_graph_search by=image` com o shot. Se já existe,
   recupere a memória (`image_graph_get`/`memory_search`); se é nova, **grave**.
3. **Planeje** com o mapa (`Y`) e a memória: para onde ir, o que falta, o que
   carregar (cuidado com a capacidade de itens e o Item Box).
4. **Aja em passos curtos** (skill `controlar-jogo`): `mode step` + `do`/`step N`,
   olhando a imagem a cada passo.
5. **Atualize a memória** ao pegar item, abrir porta, resolver puzzle ou descobrir
   uma porta que falta chave (marque vermelha + destino).
6. Repita. Antes de fechar, **`mode live`** (ver `controlar-jogo`).

## 5. Lições já observadas (não repetir o erro)

- **Nenhum modelo viu a caixa de munição** — nem qwen, nem fastvlm, nem o WD tagger,
  nem a minha visão. O item mais importante passou batido. **Desconfie da legenda
  automática para itens** e escreva o `text` você mesmo, olhando a imagem.
- O WD tagger marcou **`blood`** numa cena sem sangue: era o **vermelho da jaqueta/
  munição**. Cor não é objeto.
- A extração antiga (legenda curta + `yake`) gerou lixo (`chair and stacked`,
  `woman stands` — fragmentos, não entidades). Use `text` + `gliner` com labels de
  jogo; ainda assim, **revise os termos** que voltarem.
- Manual é **escaneado**: sem texto nativo; `tesseract -l eng` resolve.
