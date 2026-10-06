---
name: jogar-jogo
description: Jogar um jogo no emulador construindo memória da partida no GraphRAG. Ensina o ciclo de memória — a cada comando você recebe uma imagem; olhe e decida se é uma cena nova/relevante; ao mudar de cena, BUSQUE no GraphRAG se já há memória daquela cena; se não houver, GRAVE a primeira memória detalhando os itens/lugares de interesse. Use sempre que for jogar/avançar um jogo de forma autônoma, "jogar sozinho", "explorar uma sala", "voltei numa sala que já estive", "onde tinha o item X", ou ao montar/consultar memória de uma partida. A mecânica de pilotar fica em controlar-jogo; a memória de imagem em image-graph-rag.
---

# Jogar um jogo com memória (GraphRAG)

Este skill é o **cérebro de memória** de quem joga. Ele não ensina a apertar botão
(isso é `controlar-jogo`) nem o contrato das ferramentas de imagem (isso é
`image-graph-rag`). Ele ensina **quando olhar, quando buscar e quando gravar** —
para a partida não viver no contexto e para você reconhecer lugares onde já esteve.

Por que isso importa: jogos de exploração/backtrack (RE, Zelda, Metroid…) cobram
memória. "Já estive aqui", "aqui peguei o item X", "aqui tem o buraco para encaixar
o escudo". Isso não cabe no contexto — vai para o GraphRAG, que relaciona **sala ↔
itens ↔ eventos** e devolve por texto ou por imagem.

> **Tools MCP (não use HTTP).** A visão usa o MCP `mcp-vision`: `image_graph_save`,
> `image_graph_search`, `image_graph_get`, `memory_search`, `graph_save`, `graph_query`,
> `warmup_models`, `unload_models`; e os passos de visão `image_describe`,
> `image_classify`, `image_objects`, `image_pipeline`, `tag_image`, `extract_terms`,
> `extract_relationships` — cada um com campo `model`. Calibração em §"Compor a memória".

## O ciclo (faça a cada ação)

1. **Olhe a imagem** que voltou do último comando (`shot`, `do`, `step N`).
2. **Decida se vale gravar**: você mudou de ambiente/sala, pegou item, resolveu um
   puzzle, ou viu algo novo? Então é candidato a memória. Se a tela é o mesmo
   estado de antes, **não grave** (idempotência por sha256 ignora a imagem igual).
3. **Mudou de cena? BUSQUE antes de agir.**
   - `image_graph_search by:"image"` com o shot atual → "já tem imagem parecida? já
     estive aqui?"
   - `memory_search` com uma frase do que vê → "há memória desta sala?"
   - Achou? **Leia** (`image_graph_get` / `memory_search`) o que já se sabe: itens,
     portas, eventos. Use isso para decidir o próximo passo.
4. **Não achou? GRAVE a primeira memória** desta cena (ver §Gravar).
5. **Aja** (skill `controlar-jogo`) e volte ao passo 1.

> Regra de ouro: **busca antes de explorar, grava depois de descobrir.** Nunca
> reaprenda uma sala: se você já esteve nela, a memória tem que estar lá.

## Gravar uma cena (primeira memória)

Um shot vira um documento no GraphRAG: a **imagem** (CLIP/busca por similaridade) +
um **`text`** que você escreve (detalhes de interesse) + **termos do grafo** (ligam
a imagem a itens/lugares) + **objetos** (detecção).

Ferramenta: `image_graph_save` (MCP `mcp-vision`). Contrato completo em
`image-graph-rag`. Mínimo recomendado:

```json
{
  "tree": "<árvore do jogo, ex.: re-code-veronica>",
  "embedding_model": "e5",
  "image": "<shot.PNG>",
  "text": "SALA: <nome/andar>. ESTADO: <o que mudou>. VEJO: <personagem, pose, luz>. ITENS: <tudo que importa, nomeado>. CENÁRIO: <móveis, portas, saídas>. EVENTOS: <o que aconteceu aqui>. PORTAS: <cor/estado>. SAVE/ITEM BOX: <sim|não>.",
  "options": { "caption": true, "caption_model": "qwen", "faces": true, "objects": true, "poses": false, "exif": false },
  "term_extraction": { "engine": "gliner", "labels": ["item","arma","lugar","personagem","porta","chave"] }
}
```

### O que procurar para salvar (detalhes de interesse)

Não descreva a cena de forma genérica — salve o que serve para **jogar depois**:

- **Itens** e onde estão (no chão, na mesa, atrás de quê). Inclua itens que você
  **não pegou** (para voltar) e os que **pegou** (para saber que a sala mudou).
- **Portas** e estado: **amarela** (aberta), **vermelha** (fechada sem chave),
  **verde** (fechada com chave). Anote o **destino** se souber.
- **Objetos de puzzle** e receptáculos ("buraco onde encaixa o escudo", "typewriter").
- **Item box / save point** (typewriter) — essenciais para o backtrack.
- **Saídas/conexões** ("a porta à esquerda leva ao corredor X").
- **Eventos** ("aqui apertei o botão e a grade abriu").
- **Nome da sala/andar** para você referenciar depois.

### Namespace/árvore

Uma árvore por jogo (ex.: `re-code-veronica`). Só crie árvore nova se for outro
jogo; para outro save/replay, use outra árvore (`<jogo>-run2`). Fixe o
`embedding_model` (`e5` para pt/es).

## Compor a memória (tools MCP + a sua visão)

A legenda e os detectores ajudam, mas **não são ground truth**. Use as **tools do MCP
`mcp-vision`** (não a API REST). Cada passo escolhe o modelo:

- **Descrição:** tool `image_describe` (`model`: `qwen` detalhado/melhor, `fastvlm`
  rápido, `vit-gpt2` fraco que **ignora instruções**, `wd-tagger` = tags).
- **Objetos (zero-shot, por prompt):** tool `image_objects`
  (`labels:[...]`, `model`, `threshold`). O que **cada um pega melhor** (testado numa
  cena de RE — Claire na cela com esqueiro, sacos, munição):
  | model | pega bem | observação |
  |---|---|---|
  | `grounding-dino` | **itens** (`ammo box`, `shirt`) | prompt em texto; labels vêm concatenados (a tool já limpa) |
  | `owlv2` | **cenário** (`prison cell door`, `ashtray`, `lighter`, `sandbag`) | labels limpos |
  | `yolo-world` | `chair`, `person`, `prison cell door`, `ashtray`, `sandbag` | precisa do text encoder CLIP; **threshold baixo (~0.05)** |
  | `florence2` | classes abertas (`person`, `human face`, `footwear`) via `<OD>` | **não** aceita prompt de classe |
  | `yolo-coco` | só 80 classes COCO (`person`, `chair`…) | baseline; não conhece itens de jogo |
- **Classificação zero-shot:** tool `image_classify` (`openclip`, `siglip`, `wd-tagger`).
  Bom para tipo de ambiente/inventário (ex.: "storage room" 0.55 vs "prison cell" 0.37).
- **Tags:** tool `tag_image` (`wd-tagger`) — atributos do **personagem**
  (roupa/cabelo/pose), mas **alucina cor** (`blood` num vermelho de jaqueta/munição).
- **Termos:** tool `extract_terms` (`gliner`, `qwen`, `yake`, `winknlp`).
- **Relações:** tool `extract_relationships` (`qwen`, `cooccurrence`, `patterns`, `embeddings`).
- **Pipeline (faz tudo):** tool `image_pipeline`
  (`image`, `describe_model`, `object_model`, `object_labels`, `term_engine`, `relation_engine`, `labels`)
  → encadeia describe → objects → terms → relations.

**Calibração que economiza tentativa e erro (medido nesta cena):**

- **Threshold baixo para itens**: 0.05–0.15. Os scores de itens são modestos
  (ex.: `ammo box` 0.37, `lighter` 0.23, `sandbag` 0.06). Suba só se vier ruído.
- **Nenhum modelo pega tudo.** Combine: `grounding-dino` para **itens** e
  `owlv2`/`yolo-world` para **cenário** — juntos cobrem munição, esqueiro, cinzeiro,
  sacos e a cela.
- **Nada disso nomeia com a confiança que você quer** para o inventário: a caixa de
  munição passou batida na minha visão, no qwen, no fastvlm, no WD e no YOLO COCO;
  só o zero-shot por prompt (`"ammo box"`) a achou. **Nomeie os itens no `text`** —
  o detector serve para *confirmar/localizar*, não para decidir sozinho.
- **Cor não é objeto**: WD marcou `blood` num vermelho de jaqueta/munição.

**A sua visão fecha a conta.** Exemplo real: numa cena de RE, qwen/fastvlm/WD/YOLO
todos perderam a caixa de munição — o `image_objects` com `grounding-dino` a achou
com o prompt `"ammo box"` e o `owlv2` achou a cela (`prison cell door`), o cinzeiro
e o isqueiro. Combine os dois.

## Consultar a memória (backtrack)

| Pergunta enquanto joga | Ferramenta |
|---|---|
| "já estive nesta sala?" | `image_graph_search` `by:"image"`, `image:<shot>` |
| "o que tem/teve nesta sala?" | `image_graph_get` (pelo id/imagem) ou `memory_search` por frase |
| "onde estava o item X?" | `memory_search` `query:"item X <sala>"` |
| "salas com o item X" | `image_graph_search` `by:"object"` ou `graph_query` `terms:["X"]`, `include_images:true` |
| "quais ambientes ligam a esta sala?" | `graph_query` `terms:["<nome da sala>"]`, `walk:true`, `include_images:true` |
| "para onde vai a porta/entrada Y?" | `graph_query` `terms:["<nome da sala>","<porta>"]`, `walk:false` |
| "portas trancadas / o que falta abrir" | `graph_query` `terms:["porta","trancada"]`, `walk:false` |
| conceitos relacionados | `graph_query` (`min_similarity` ~0.88–0.90 com e5) |

Ao reconhecer a sala, sempre **atualize** a memória se o estado mudou (pegou item,
abriu porta) — grave um **novo shot** (a imagem muda quando a cena muda; a
idempotência sha256 só bloqueia re-gravar a **mesma** imagem).

## Mapear os ambientes (o grafo de navegação)

Além de ligar imagem↔itens, o **mesmo grafo** (mesma `tree`) guarda a **topologia
dos ambientes**: como as salas se conectam. É isso que responde "daqui, onde vou?"
e sustenta o backtrack.

### Regra da direção: relativa a portas/entradas/saídas

Não invente N/S/L/O. A **maioria dos jogos tem câmera livre** e você não sabe onde
é o norte. Registre a conexão **pela porta/entrada/saída que leva de um ambiente ao
outro**, em termos **relativos**:

- `porta_grade` (pela grade da cela), `porta_leste_da_sala`, `passagem_escura_esquerda`,
  `escada_sobe`, `escada_desce`, `porta_trancada_vermelha`, `corredor_em_frente`…
- Use o que a cena mostra: "ao entrar, a passagem escura à esquerda leva a Y", "a
  grade à direita leva a Z", "em frente tem um corredor".
- **Se o HUD tiver bússola/rosa-dos-ventos** (aí sim você sabe N/S/L/O), use a
  direção absoluta. Sem bússola, **sempre relativa à porta/entrada**.

A direção relativa **não precisa ser a mesma nos dois sentidos**: grave a aresta da
sala atual para a vizinha; a inversa (quando você entrar nela) só precisa existir
como fato, não necessariamente com a mesma "direção".

### O nó do ambiente: nome inferido curto e estável

O nome do ambiente é a **chave do nó** e é buscado por **embedding** — então a LLM
pode variar o nome e ainda achar. Para a busca semântica não quebrar, siga regras:

- **Curto** (2–4 palavras). Ex.: `cela grade aberta`, `corredor escuro`, `sala do
  painel`, `depósito de sacos`.
- **Descritivo e único**: inclua a característica marcante (não só "sala"), para
  diferenciar de outras salas parecidas.
- **Serve o nome que você inferir da cena** — não precisa ser oficial. O embedding
  casa variações; mas **evite sinônimos aleatórios** na mesma sala (fixe o nome que
  você escolheu e reuse ao voltar).
- Se souber o **andar/área** ("ilha Rockfort, 1º andar"), acrescente — ajuda a
  desambiguar salas parecidas em andares diferentes.

Ao **entrar num ambiente, busque o nó antes**: `graph_query`/`memory_search` com o
nome inferido. Se já existe, **não recrie** — só complemente (nova saída, novo
estado de porta). Só crie nó novo se a busca não achar nada equivalente.

### Como gravar (duas opções — a skill do jogo específico reforça qual)

**(A) `relationships` explícitas** — determinístico, não depende de extração.
Recomendado para as arestas do mapa:

```
graph_save (tree, embedding_model, graph):
  terms: ["cela grade aberta", "corredor escuro", "depósito de sacos"]
  relationships: [
    { source: "cela grade aberta", target: "corredor escuro",   label: "porta grade a direita",  evidence: "grade a direita ao sair" },
    { source: "cela grade aberta", target: "depósito de sacos", label: "passagem escura esquerda", evidence: "passagem escura a esquerda" }
  ]
```

**(B) Texto livre + extração** — mais simples, menos preciso. O `graph_save` com
`text` extrai termos e relações:

```
graph_save (tree, embedding_model, text):
  "O ambiente 'cela grade aberta' liga ao 'corredor escuro' pela porta grade a
   direita e ao 'depósito de sacos' pela passagem escura a esquerda. A grade esta
   aberta; a passagem a esquerda tem caixa de munição."
```

Use **as duas juntas** quando ajudar: as `relationships` para as arestas exatas e o
`text` para itens/eventos de cada ambiente. Ligue a **imagem** da sala ao nome do
ambiente via `image_graph_save` (`text` citando o nome), na mesma `tree` — assim
`graph_query terms:["cela grade aberta"] walk:true include_images:true` traz
conexões + itens + a imagem.

### Registrar eventos e relações (além do mapa)

- `graph_save` (texto) — "notas" do jogo: "o escudo encaixa no buraco da sala do
  painel"; vira termos e relações. Bom para arquivos/diários que você lê in-game.
- Relações `qwen` (engine de relationship) — liga item↔lugar↔evento com mais sentido
  que `co-occurs` (o `yake`/co-occurs cru gera lixo tipo "chair and stacked").

## Convenções que evitam retrabalho

- **`text` é o que salva.** A extração automática (gliner/yake) **perde itens**;
  o `text` fica como `extra_text` e é recuperável. Escreva os itens por extenso.
- **Nome de ambiente fixo**: escolha um nome curto por sala e **reuse-o** ao voltar
  (não troque sinônimos). O nome é o nó do mapa e é achado por embedding.
- **Direção por porta/entrada** (N/S/L/O só com bússola no HUD).
- **Um shot por estado relevante**, não por frame.
- **Busca por imagem, não só por texto**: reconhecer "já vi esta sala" é
  similaridade visual.
- **Mantenha também um plano em texto** (fora do RAG): lista de salas conhecidas,
  o que falta, portas trancadas e chaves. O RAG é para *relembrar*, não para
  *planejar*.
- **Antes de fechar o jogo**: `mode live` (ver `controlar-jogo`) e, se o backend
  suportar, `unload_models` para liberar RAM.
