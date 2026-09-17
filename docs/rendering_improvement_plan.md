# Plano de melhoria de renderização — flycast (fork metallic77), RK3326/Mali-G31

> Sintetiza `docs/gles_code_audit.md` (o que nosso código faz) com
> `docs/mali_gles_best_practices.md` (o que a documentação real diz que
> deveria fazer). Cada item cruza um achado concreto de código com uma
> fonte externa — não é lista de "boas práticas genéricas", é onde as duas
> pesquisas convergem (ou onde uma delas sozinha já é suficientemente
> concreta pra agir). Regra de ouro do projeto continua valendo: **nenhum
> destes itens foi medido ainda** — isto é o "onde olhar", não "o que já
> se sabe que funciona".

## Contexto do pedido

Motivado por um achado do profiling desta sessão: o driver Mali
(`libmali-bifrost-g31-rxp0-gbm.so`) aparece com ~23-30% do total de
ciclos de CPU numa cena pesada, com call graph interno dominado por
`poll()`, `pthread_mutex_lock`, `sem_post`, `sem_wait` — sincronização de
thread, não trabalho de shader. A pergunta era: isso é custo inerente do
driver fechado da Mali, ou o jeito que o flycast usa OpenGL está deixando
dinheiro na mesa?

**Resposta curta, já com alguma confiança**: provavelmente as duas
coisas, em proporção desconhecida. `docs/mali_gles_best_practices.md`
confirma que *parte* desse tempo é espera de GPU genuína e esperada
(mecanismo de submissão de job via `ioctl`+`poll()` do kbase, normal em
qualquer Mali de geração Job-Manager como o G31) — mas também documenta,
com fonte primária da própria ARM, pelo menos dois padrões de uso de GL
que causam sincronização **evitável**, e `docs/gles_code_audit.md`
confirma que o flycast tem exatamente as características de código que
tornam esses padrões plausíveis aqui.

---

## Resumo executivo

| # | Item | Evidência (código + fonte) | Risco | Esforço | Prioridade |
|---|------|------------------------------|-------|---------|------------|
| 1 | `glInvalidateFramebuffer` pra depth/stencil, no fim de frame e em torno do teardown de RTT | audit §4.2 (zero ocorrências, fato) + best-practices §2b (ARM: "buffer transitório precisa ser invalidado ANTES do unbind, não no próximo uso") | Baixo | Baixo | **Alta** |
| 2 | Instrumentar (depois, se confirmado, corrigir) o frameskip da fila `rqueue` de slot único | audit §5.3 (achado de código sozinho, sem correspondente externo — é arquitetura própria do flycast) | Médio | Médio (medir primeiro) | **Alta** |
| 3 | Pool de FBO/renderbuffer/textura de RTT (parar de destruir+recriar do zero) | audit §3.4 (churn confirmado) + best-practices §1e/§4a (ARM: troca de FBO é a mudança de estado mais cara documentada, gatilho de flush) | Baixo-Médio | Médio | Média |
| 4 | Investigar se upload de textura acerta recurso ainda em uso pela GPU (round-robin de texture objects) | best-practices §3a/hipótese-1 (lista de "evitar" da ARM) — audit NÃO confirmou isso no nosso código ainda, é hipótese em aberto | Desconhecido | Investigação primeiro | Média |
| 5 | Revisitar item 4.7 do tech_debits: busca de paleta via shader em vez de expansão CPU-side | best-practices §5a/5b (técnica estabelecida + precedente do Dolphin) — reabre uma conclusão anterior "descartada" | Alto (mudança de shader/formato) | Alto | Baixa-Média (grande escopo) |
| 6 | Apertar `PP_SameGPUState` pra máscara dos 6 bits que `SetGPState` realmente lê | audit §1.5 (só pode ganhar fusão, nunca perder correção) | Muito baixo | Baixo | Baixa (ganho pequeno, mas seguro) |
| 7 | Documentar (não corrigir ainda) o risco latente texid/CustomTextures | audit §1.6 | — | — | Registro apenas |
| — | Overhead base JM (G31) vs CSF pode ser piso arquitetural, não corrigível | best-practices §3c | — | — | Contexto, não item de ação |

---

## Item 1 — `glInvalidateFramebuffer`/`EXT_discard_framebuffer` pro depth/stencil

### O achado cruzado

`docs/gles_code_audit.md` §4.2 confirma, via grep exaustivo em toda a árvore `core/`, **zero** ocorrências de `glInvalidateFramebuffer` ou `DiscardFramebuffer` em qualquer lugar. O framebuffer principal tem depth+stencil limpos no topo de `RenderFrame()` (`gles.cpp:933-937`) e nunca invalidados antes do frame terminar; todo ciclo `BindRTT()`/`ReadRTTBuffer()` anexa um renderbuffer de depth+stencil que é usado só durante aquele sub-render e deletado sem nunca ser invalidado primeiro.

`docs/mali_gles_best_practices.md` §2b cita o blog oficial "Mali Performance 2" da ARM, que descreve **exatamente** esse padrão como o erro mais comum nessa área — e crucialmente aponta que o TIMING importa: invalidar tem que acontecer **antes** do unbind do frame N, não no primeiro uso do FBO no frame N+1 (isso seria tarde demais, o flush já aconteceu). O guia oficial (§7.2) reforça: "no Valhall é especialmente importante minimizar troca de FBO porque o driver faz flush em `glBindFramebuffer()`" — e o mecanismo de "flush no unbind" é o mesmo modelo de renderização por passe, não específico de uma geração.

### Por que isso é o item de maior confiança da lista

É a única combinação onde (a) o código-fonte confirma categoricamente a ausência total (não é "pode ser assim", é "grep exaustivo, zero hits") e (b) a fonte oficial do fabricante da GPU descreve o padrão exato, incluindo o motivo mecânico (banda de escrita de tile desperdiçada) e o erro de timing mais comum ao implementar a correção. Não depende de nenhuma suposição sobre COMO o flycast está sendo usado — é estrutural.

### Risco

Baixo. `glInvalidateFramebuffer` é uma dica pro driver ("não preciso mais desses dados"), não uma operação que muda semântica visível — se usado incorretamente (invalidando um buffer que ainda seria lido), o bug resultante seria uma corrupção visual imediata e óbvia (não sutil como o bug do Gemini), fácil de pegar em QA visual.

### O que medir antes/depois

Mesmo protocolo já estabelecido (`retrorun3 --benchmark`, savestate de Metal Slug 6 e/ou a cena pesada do Shenmue usada no item 4.2) — comparar `core_average`/`video_average` e, já que a hipótese é especificamente sobre banda de memória/tempo de driver, também vale reativar a captura de `perf` no Mali (`libmali-bifrost-g31-rxp0-gbm.so`) antes/depois pra ver se o % de ciclos ali muda.

---

## Item 2 — Fila de render de slot único (`rqueue`) e frameskip — **ENFRAQUECIDO em 2026-09-16**

### Atualização: o descarte temido não está acontecendo nas cenas medidas

Todos os benchmarks desta sessão (mslug6 savestate pesado, kofnw, mbaa)
reportaram `skipped_frames: 0`, `duplicated_frames: 0` e
`missed_deadlines: 0`, e o `rsWait` medido (`FC_REND_SPLIT`) é de apenas
**8,7-12,7 ms/frame** — a thread de emulação termina antes e espera, em vez de
produzir frames que seriam descartados. O gargalo real estava na main thread
(`Process()` = 57ms de um frame de 63ms), não na fila.

Continua sendo um risco arquitetural válido em cenas que não medimos, mas
desce de prioridade: não há evidência de descarte nas três cenas de referência.

### O achado (texto original)

Achado só do `docs/gles_code_audit.md` (§5.2/§5.3) — não tem correspondente na pesquisa externa porque é arquitetura própria do flycast, não um padrão genérico de GL. `QueueRender()` (`core/hw/pvr/ta_ctx.cpp:113-166`) descarta (nunca enfileira) um novo `TA_context` se já existe um `rqueue` pendente, e esse slot só é liberado por `FinishRender()`, chamado só depois que `Render()` (a submissão GL inteira, ~500-600 draw calls numa cena pesada) termina — não depois só do parsing do TA. Combinado com a thread de emulação sendo liberada pra simular o próximo frame **antes** de `Render()` começar (`rend_frame()`, `Renderer_if.cpp:159-179`), isso significa: numa cena pesada, se a thread de emulação terminar de simular um novo frame enquanto a thread de render ainda está no meio de `Render()` do frame anterior, o novo frame é **descartado**, não esperado.

### Por que isso importa mais do que parece

Isso dá uma explicação mecânica alternativa (e provavelmente complementar) pro ganho já medido do item 4.2 (batching de draw call, -7% `core_average`/+7% fps no Shenmue): parte desse ganho pode não ser "menos tempo de CPU gasto desenhando", e sim "menos frames descartados por frameskip", porque um `Render()` mais rápido reduz a janela durante a qual esse slot único fica ocupado. Isso muda qual MÉTRICA conta a história certa — `core_average` sozinho pode estar subestimando o ganho real em fluidez percebida (percentis de frame-time e taxa de frameskip seriam mais informativos que a média).

### Risco de agir aqui

Médio — diferente do item 1, isso é lógica de threading/sincronização, não uma dica opcional de driver. Uma correção mal feita (por exemplo, aumentar a profundidade da fila sem entender as implicações de ordem/latência) pode introduzir problemas de frame pacing ou sincronização de áudio.

### O que fazer primeiro (antes de qualquer correção)

Só instrumentação, seguindo a mesma disciplina do projeto: um contador no branch de descarte de `QueueRender()` (`ta_ctx.cpp:153`, `tactx_Recycle(ctx); return false;`), correlacionado com a duração medida de `Render()`. Se a taxa de descarte for alta em cenas pesadas e cair quase a zero com o binário já otimizado (pós item 4.2), isso confirma a hipótese sem precisar tocar em nada da lógica de fila ainda.

---

## Item 3 — Pool de recursos de RTT (parar de destruir+recriar)

### O achado cruzado

`docs/gles_code_audit.md` §3.4: `BindRTT()` (`core/rend/gles/gltex.cpp:186-257`) deleta e recria o FBO, o renderbuffer de depth e a textura **do zero, toda vez**, sem nenhuma checagem de "já tenho um do mesmo tamanho, só reusar". `docs/mali_gles_best_practices.md` §1e/§4a: a documentação oficial da ARM trata troca de render-target/FBO como a categoria de mudança de estado mais cara (citação do "Beyond Porting": ~60K/s pra render target vs ~1.5M/s pra bind de textura — ordens de magnitude de diferença) e um gatilho de flush documentado.

### Escopo

Só afeta jogos que usam render-to-texture (espelhos, efeitos de tela-dentro-de-tela) — não é um custo universal como o item 1. Vale confirmar primeiro se algum jogo do conjunto de teste atual (Metal Slug 6, kofnw, Shenmue) realmente usa RTT antes de investir esforço aqui.

### Risco

Baixo-médio — adicionar uma checagem de tamanho antes de destruir/recriar é uma mudança localizada, mas precisa cuidado com o ciclo de vida (garantir que o recurso "reusado" realmente está livre pra reescrever, não sendo lido em outro lugar ainda).

---

## Item 4 — Upload de textura em recurso ainda em uso (investigação) — **REFORÇADO em 2026-09-16, agora é o item nº1**

### Atualização 2026-09-16: a metade que faltava está confirmada

O texto original dizia que o audit "não confirmou nem refutou se o flycast faz
round-robin de texture objects". **Agora está confirmado que NÃO faz**:
`TextureCacheData::UploadToGPU` (`core/rend/gles/gltex.cpp`) escreve sempre no
mesmo `texID` do `TextureCacheData`, sem nenhuma proteção contra reescrever
uma textura que a GPU ainda pode estar consumindo.

E a medição (`FC_TA_SPLIT`, mslug6 savestate pesado) é consistente com o
mecanismo que a ARM descreve:

| | |
|---|---|
| upload | **13,1 ms/frame** (era 34,6 antes do fix do item 4.12 do tech_debits) |
| chamadas | ~166 mil em 30s |
| custo por chamada | **~83 µs** |
| dados por textura | **~134 bytes** (sprites 8x8/16x16 paletizados) |
| dados totais | 21,2 MB em 30s = 0,7 MB/s |

83µs para transferir 134 bytes **não é largura de banda**. E **não é
realocação**: trocar `glTexImage2D` por `glTexSubImage2D` quando o layout não
muda (`FC_TEX_SUBIMAGE`, opt-in, implementado) rendeu apenas **-4%**. Sobra
exatamente a hipótese deste item — dreno de pipeline / cópia-fantasma por
escrever em recurso em voo.

**Próximo passo concreto:** testar round-robin de N texture objects por
`TextureCacheData` (escrever no slot que a GPU menos provavelmente está
lendo), ou PBO para upload assíncrono. Medir com os contadores que já existem
(`upload_us_total`).

### O achado (texto original)

Puramente da pesquisa externa (`docs/mali_gles_best_practices.md` §3a, hipótese #1 da seção final) — a lista de "evitar" da própria ARM cita explicitamente `glTexSubImage2D`/`glCopyTexImage2D` numa textura ainda referenciada por um draw call em andamento como gatilho de dreno de pipeline ou cópia-fantasma. **O `docs/gles_code_audit.md` não confirmou nem refutou isso no nosso código** — não foi verificado se o flycast faz round-robin de texture objects ou se pode reescrever uma textura que a GPU ainda está consumindo.

### Por que está como "investigação", não "correção"

Diferente dos itens 1 e 3, aqui não temos confirmação de código de que o padrão realmente existe no flycast — é uma hipótese bem fundamentada externamente, mas precisa de uma leitura de código dedicada (rastrear `TextureCacheData::UploadToGPU`, ver se há qualquer proteção contra reescrever uma textura em voo) antes de decidir se vale a pena medir/corrigir.

---

## Item 5 — Busca de paleta via shader — **RESOLVIDO em 2026-09-16 (e a premissa deste item estava errada)**

### Atualização: a técnica já existia no código, morta por um default

Este item classificava a busca de paleta no shader como **esforço alto**
("mudar o pipeline de shader e o formato de textura enviado à GPU"), a
revisitar com tempo dedicado. **A premissa estava errada:** a técnica já
estava implementada neste fork inteira — `IsGpuHandledPaletted()`
(`TexCache.h`) + o caminho de shader correspondente.

Ela nunca rodava porque `settings.rend.TextureUpscale` ficava **0** numa build
libretro (o único lugar que atribui o campo está sob `#ifdef HAVE_TEXUPSCALE`,
que o Makefile deste fork desliga; o outro candidato estava sob
`#ifndef __LIBRETRO__`). Todo o código lê esse campo com `> 1` e tolera o 0 —
só `IsGpuHandledPaletted()` usa `== 1`, e com 0 retorna sempre falso. Medido
antes do fix: **0 de 166.285** texturas paletizadas usavam o caminho GPU.

**Esforço real da correção: uma linha** (inicializar `TextureUpscale = 1` em
`LoadSettings()`, fora do `#ifndef`). Resultado: mslug6 **+35% fps**,
`core_p99` **-57%**; kofnw +7% fps, `core_p99` -54%; e **corrigiu glitches
visuais** (a paleta deixou de ser assada na textura no momento do upload).
Ver `docs/tech_debits.md` item 4.12 e
`docs/texcache_vram_invalidation_plan.md`.

**Lição para este documento:** antes de estimar esforço de uma técnica, checar
se ela já não está no código desligada. Esta é a segunda vez na sessão — o
`shop_sync_fpscr` também estava implementado e nunca emitido.

### O achado cruzado (texto original)

`docs/tech_debits.md` item 4.7 já tinha catalogado o custo de conversão de textura CPU-side (`texconv*()`, O(texel count) por atualização) e o marcou como "esperado/inevitável, baixa prioridade" — uma conclusão razoável **se a única alternativa fosse não converter**. `docs/mali_gles_best_practices.md` §5a/5b encontrou que essa suposição pode estar incompleta: existe uma técnica estabelecida (enviar índices de paleta crus como textura + uma textura de paleta pequena, fazer a busca no fragment shader) que elimina a conversão CPU-side inteiramente, com precedente direto num emulador real na mesma categoria de problema (Dolphin, GameCube/Wii, formatos de textura igualmente hostis à CPU).

### Por que a prioridade é mais baixa apesar do achado ser interessante

Esforço alto — não é uma mudança de driver/estado, é mudar o pipeline de shader e o formato de textura enviado à GPU pras texturas paletizadas do PowerVR. Risco de regressão visual real se a busca de paleta no shader não replicar exatamente a semântica de conversão atual (incluindo qualquer tratamento especial de transparência/chroma-key que o código de conversão atual já faça). Vale revisitar com tempo dedicado, não como parte de um ciclo de otimização rápido.

---

## Item 6 — Máscara mais justa em `PP_SameGPUState`

### O achado

`docs/gles_code_audit.md` §1.5 — a comparação usa `isp.full == isp.full` (32 bits) quando `SetGPState()` só lê 6 bits específicos (`CullMode`, `DepthMode`, `ZWriteDis`). Duas strips que diferem só em bits nunca lidos (`Reserved`/`CacheBypass`/`DCalcCtrl`/`UV_16b`) são julgadas "diferentes" e perdem uma fusão de batching possível.

### Por que é seguro e de baixa prioridade ao mesmo tempo

Seguro: só pode GANHAR mais fusões que hoje são perdidas por engano — não existe cenário onde apertar a máscara (torná-la mais permissiva sobre bits irrelevantes) faz o código fundir dois estados que deveriam ser diferentes, porque a máscara continuaria cobrindo tudo que `SetGPState` realmente usa. Baixa prioridade: o impacto real depende de esses bits irrelevantes realmente variarem na prática entre strips adjacentes, o que é plausível que quase nunca aconteça (a maioria são reservados/zero).

### Antes de mudar

Instrumentar primeiro (contar quantas vezes `isp.full` difere mas a máscara dos 6 bits usados seria igual) pra confirmar que a mudança vale o esforço antes de tocar em código de batching de novo — dado o histórico do item 4.2 com o Gemini, qualquer mudança nessa área merece essa cautela extra mesmo sendo teoricamente segura.

---

## Item 7 — Risco latente texid/CustomTextures (registro, não correção)

`docs/gles_code_audit.md` §1.6: se `settings.rend.CustomTextures` for ligado, existe uma janela estreita onde um carregamento assíncrono de textura customizada terminando entre duas chamadas `GetTexture()` pro mesmo `tsp`/`tcw` faria `PP_SameGPUState` (que nunca olha `texid`) fundir dois `PolyParam`s que deveriam usar texturas diferentes — mecanismo plausível pra uma regressão visual estilo Gemini reaparecer. **Não reproduzido, não confirmado se `CustomTextures` está ligado na config ativa deste device.** Fica registrado aqui e não vira item de correção até (a) confirmar se a opção é usada por algum perfil deste device e (b) se sim, tentar reproduzir.

---

## Nota final: pode ser parcialmente um piso arquitetural

`docs/mali_gles_best_practices.md` §3c: o Mali-G31 é geração Job-Manager (pré-CSF), e a própria ARM documenta que esse modelo tem custo de CPU por draw call inerentemente mais alto que a geração CSF mais nova, porque revalida mais estado por draw em vez de transmitir deltas incrementais. **Uma fração dos 25-30% de ciclos no driver Mali pode simplesmente ser o custo de fazer negócio nessa geração de GPU, não corrigível do lado da aplicação.** Isso não invalida os itens 1-4 acima (que atacam padrões de uso REALMENTE evitáveis), mas é uma calibração de expectativa honesta: não esperar que zerar esses achados elimine o overhead do driver Mali por completo — o teto de ganho aqui é desconhecido até medir.
