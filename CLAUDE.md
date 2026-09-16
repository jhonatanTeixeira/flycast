# CLAUDE.md

Este arquivo orienta o Claude Code neste repositório especificamente. Este NÃO é o
código atual do flycast (`flyinghead/flycast`) — é o fork `metallic77/flycast`,
commit `603814c9f73b773c455d9a497f389d2f93a257fd`, checado out na branch
`flycast2021-metallic77-base` como um `git worktree` separado (o `.git` real é o do
repositório `flyinghead/flycast`). Esse fork diverge do `flyinghead/flycast` em
**2015** — são ~10 anos de evolução paralela, não uma versão "congelada em 2021"
como o nome popular ("flycast2021") sugere.

## Por que este repositório existe

É o alvo de um projeto de otimização de performance rodando num handheld retro R36
(RK3326, Cortex-A53 quad-core @ 1.5GHz, Mali-G31, OpenGL ES 3.2, sem Vulkan
utilizável). Esse fork/build é o que roda bem no device hoje (estável, boa
performance); o `flyinghead/flycast` atual é **muito mais lento nesse hardware —
o usuário relata ~10x** (não é "só" um bug de crash isolado). A investigação de
2026-09-13 (ver `docs/history.md`) confirma isso: rodando a mesma cena, o master
mostrou frames **escalando de ~26ms pra ~100ms antes de eventualmente também
travar** (SIGSEGV no block-dispatch table do dynarec ARM64) — ou seja, a
lentidão e o crash são provavelmente a MESMA causa raiz (dispatch de bloco
degradando), não dois problemas separados. **Não é o foco deste projeto
consertar/rodar o master** (o fork é o que roda bem no device), mas **entender
o QUE no master ficou mais lento/degrada é uma pista válida** para otimizar
este fork — pode revelar uma técnica que o master abandonou (e este fork ainda
tem) ou um problema de arquitetura a evitar. Não persiga rodar o master no
device a menos que explicitamente pedido, mas comparação de código entre os
dois é uma ferramenta de investigação legítima.

## Documentação do projeto (leia nesta ordem)

1. **`docs/profiling_plan.md`** — auditoria estática completa do código apontando
   onde e por que instrumentar (arquivo:linha, motivo, como medir), ranqueada por
   impacto.
2. **`docs/tech_debits.md`** — tabela de status de cada achado do plano de
   profiling (`não investigado` / `instrumentado` / `confirmado` / `descartado` /
   `corrigido`). Atualizar sempre que um achado for investigado.
3. **`docs/current_plan.md`** — o que está sendo trabalhado agora, com status
   (`pendente` / `in progress` / `done` / `bloqueado`).
4. **`docs/history.md`** — log cronológico com timestamp de tudo que foi feito.
   Adicionar uma entrada por sessão/marco relevante.

## Regras de ouro

- **Meça antes de otimizar, sempre.** Toda hipótese levantada só de olhar código
  (mesmo as "óbvias") precisa ser validada com medição real no device antes de
  virar uma mudança de código. Neste projeto, hipóteses razoáveis já cairam por
  terra na medição mais de uma vez (clock de GPU não mudou o tempo de "core";
  algoritmo de sort é idêntico entre versões).
- **Toda comparação de performance precisa medir a mesma cena/conteúdo.** Um teste
  A/B só é válido se as duas rodadas passarem pelo mesmo trecho de jogo. Já
  aconteceu de uma comparação de governor de CPU virar inválida porque uma rodada
  pegou a intro e a outra pegou "New Game" + cutscene de neve (cena bem mais
  pesada) — sempre confirme o que cada rodada realmente mediu antes de comparar
  números.
- **Testes de performance precisam de pelo menos ~90-100s de warmup** antes de
  começar a medir, pra passar do boot/BIOS/logos e chegar em conteúdo 3D real do
  jogo. Não tire conclusão de rodadas curtas (<30s) a menos que o objetivo seja
  especificamente isolar a tela de boot.
- **Build cross-compile (aarch64) tem bugs conhecidos no Makefile deste fork —
  não confie em `CXX ?=`/`CC_AS ?=` do Makefile.** Sempre passe explicitamente na
  linha de comando:
  ```
  make platform=arm64 CC_PREFIX=aarch64-linux-gnu- \
       CXX=aarch64-linux-gnu-g++ CC=aarch64-linux-gnu-gcc \
       CC_AS=aarch64-linux-gnu-g++ HAVE_OPENMP=0 -j2
  ```
  (`CXX ?= g++` na linha 922 do Makefile sobrescreve o cross-compiler pra alguns
  arquivos silenciosamente — um `.o` compilado errado não dá erro até o link ou,
  pior, até rodar. Depois de qualquer mudança nessas variáveis, rode `make clean`
  antes do rebuild pra não deixar objetos da arquitetura errada parados.) Falta
  `-lGLESv2` pro linker aarch64 local — copiar `libGLESv2.so` do device pra dentro
  deste diretório e passar `LDFLAGS="-L."`.
- **O rastreamento de dependência de header do Makefile deste fork não é
  confiável — depois de editar QUALQUER header amplamente incluído (`types.h`
  em especial, mas vale pra qualquer `.h` compartilhado), rode `make clean`
  antes do rebuild, mesmo que o build incremental "funcione" sem erro.**
  Descoberto em 2026-09-16: mudar o TIPO de um campo dentro do struct global
  `settings_t` (`bool`→`float` em `core/types.h`) com rebuild incremental
  recompilou só 1-3 `.cpp` (os editados diretamente), não os ~100+ que também
  incluem `types.h` — build limpo (sem erro, sem warning), mas o binário
  ficou com `.o`s discordando sobre o layout de `settings` (uns viram o campo
  novo, outros ainda veem o antigo, cada um com offsets diferentes pros
  campos seguintes na struct) → corrupção de memória silenciosa, sem crash,
  manifestando como regressão de performance catastrófica e sem relação
  lógica com a mudança feita (~9x mais lento, 9fps). `make clean` + rebuild
  completo (111 arquivos) resolveu por completo. Ver `docs/tech_debits.md`.
- **Não use `-j$(nproc)` nesta máquina.** É compartilhada com várias outras sessões
  de Claude Code + Docker + Grafana/Tempo rodando ao mesmo tempo; builds grandes
  são derrubados por um watchdog de baixa-memória do sistema (não é o build en si
  que estoura memória). Use `-j1` ou `-j2` e simplesmente retome o `make`
  (incremental) se for interrompido.
- **Este fork não tem a infraestrutura de profiling do `flyinghead/flycast` atual**
  (`core/profiler/fc_profiler.*`, `ui/`, etc. não existem aqui). Instrumentação
  precisa ser feita com `chrono`/contadores simples direto no código, ou portando
  um subconjunto mínimo — decisão ainda em aberto, ver `docs/current_plan.md`.
- **Acesso ao device de teste:** SSH em `192.168.0.14` (usuário `ark`, senha
  `ark`, via `sshpass`). Frontend de teste é o `retrorun3`
  (`/usr/local/bin/retrorun3`), que tem um modo `--benchmark N --benchmark-warmup
  M --benchmark-json arquivo.json` que já dá `core_average`/`video_average` por
  frame sem precisar de instrumentação nenhuma no próprio flycast — é o primeiro
  lugar pra olhar antes de instrumentar código.
- **Registre todo achado novo em `docs/tech_debits.md`** com status, e toda
  sessão de trabalho relevante em `docs/history.md` com timestamp — não deixe
  conhecimento só na conversa.
