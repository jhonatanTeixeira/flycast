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
5. **`docs/game_status.md`** — estado por jogo do ponto de vista de quem JOGA
   (fps sentido, cauda longa, hicups, glitches), avaliado pelo usuário no
   device. É o contraponto ao benchmark: já apareceu jogo com distribuição de
   CPU plana no benchmark e hicup claro jogando (MBAA). Atualizar quando o
   usuário reavaliar.

## Regras de ouro

- **Código não se reverte — se corrige. O projeto só anda pra frente.** Quando uma
  implementação não entrega o ganho esperado, a conclusão NÃO é "o caminho é
  inválido, vamos reverter" — é "a implementação (ou a premissa dela) está
  incompleta, vamos achar o porquê e corrigir". Nada de `git revert`, nada de
  descartar trabalho feito, nada de voltar pro estado anterior como "solução".
  Isso vale com força dobrada quando existe prova em outros emuladores maduros de
  que o caminho está certo: se lá funciona e aqui piorou, o que está errado é a
  nossa versão, não o caminho — vá investigar a diferença até achar. Já aconteceu
  neste projeto de uma mudança dar resultado nulo porque a premissa da
  investigação estava furada (o fim de bloco forçado do FPSCR já existia desde
  2015, então a "nova" implementação só trocava o destino da chamada em vez de
  eliminá-la) — a resposta certa ali era corrigir pra eliminar a chamada de
  verdade, não desfazer.
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
- **`make clean` sozinho (sem os mesmos argumentos do build) NÃO limpa os objetos
  arm64 — limpa a lista de objetos de outro platform default, deixando os `.o`
  arm64 antigos parados.** `clean:` no Makefile é `rm -f $(OBJECTS) $(TARGET)`, e
  `$(OBJECTS)` é calculado condicionalmente a partir de `platform=`/flags — se
  você rodar só `make clean`, ele limpa a lista errada. **Sempre rode `make clean`
  com EXATAMENTE os mesmos argumentos do comando de build** (`platform=arm64
  CC_PREFIX=... CXX=... CC=... CC_AS=... HAVE_OPENMP=0 LDFLAGS="-L."`), e depois
  do clean confirme com `find . -name "*.o" | wc -l` (deve dar 0) antes de
  reconstruir. Isso já causou duas regressões de performance catastróficas
  nesta sessão (~9x mais lento) por objetos desatualizados discordando do layout
  de `settings_t`/`types.h` — sem erro nem warning no build, só regressão
  silenciosa. O rastreamento de dependência de header deste Makefile também não é
  confiável em geral — depois de editar QUALQUER header amplamente incluído
  (`types.h` em especial), sempre passe pelo `make clean` completo acima, mesmo
  que o build incremental "funcione" sem erro.
- **Métrica de performance é frame time + fps, sempre os dois, sempre em
  p50/p95/p99 E média geral — nunca só um número.** Olhar só a média (de fps ou
  de frame time) já escondeu ganho real mais de uma vez neste projeto, e olhar só
  um dos dois (só fps ou só frame time) também esconde coisas — a cauda (p95/p99)
  pode contar uma história diferente da média, e frame time real (`active_frame_*`
  do benchmark JSON, core+vídeo combinados) é mais fiel ao que se sente na tela
  do que `core_*` isolado. Ao comparar A/B, sempre puxar a tabela completa dos
  dois lados antes de concluir qualquer coisa.
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
