# Auditoria do backend OpenGL ES (flycast, fork metallic77) — RK3326/Mali-G31

> Gerado por subagente de investigação (read-only, sem medição), 2026-09-15.
> Constrói sobre `docs/tech_debits.md` itens 3.7, 4.1-4.8 e `docs/profiling_plan.md` §4
> (não redriva o que já estava medido/confirmado ali). Achados novos marcados **[NOVO]**.
> Ver `docs/rendering_improvement_plan.md` para o plano de ação que cruza isto com
> `docs/mali_gles_best_practices.md`.

---

## 1. Padrão de draw call — cobertura e lacunas do batching (item 4.2)

**1.1 O que o batching cobre.** `core/rend/gles/gldraw.cpp:262-353` (`DrawList<Type,SortingEnabled>`). `PolyParam`s consecutivos de uma "strip" são fundidos num único `glDrawElements(GL_TRIANGLE_STRIP, ...)` via `GL_PRIMITIVE_RESTART_FIXED_INDEX`, condicionado por `const bool canBatch = gl.is_gles && gl.gl_major >= 3;` (`gldraw.cpp:282`). É o caminho usado para Opaque, Punch-Through e Translucent-com-`SortPParams` (sort por strip) — confirmado como o *único* caminho de renderização de listas ativo neste device (item 4.1: `GenSorted`/`DrawSorted`, sort por triângulo, está desligado na config). Resultado já medido no item 4.2: -7% `core_average`, +7% fps no Shenmue.

**1.2 O que NÃO cobre — `DrawSorted()`.** `gldraw.cpp:383-445`. Caminho de sort por triângulo (`AlphaSortMode==0`), ainda um `glDrawElements(GL_TRIANGLES, ...)` por grupo de triângulo ordenado, sem batching algum. Item 4.1 diz que esse caminho nunca dispara na config deste device — risco dormente, não custo ativo.

**1.3 O que NÃO cobre — `DrawModVols()`.** `gldraw.cpp:576-654`. Um `glDrawArrays(GL_TRIANGLES, ...)` por `ModifierVolumeParam` (linha 620), mais um segundo pra soma de inclusão/exclusão (linha 626) — nunca batchado. Volumes modificadores (sombras) geralmente são poucos por frame, custo absoluto baixo, mas estruturalmente o mesmo padrão "um draw por param" que o item 4.2 atacou.

**1.4 `PP_SameGPUState()` duplica `PP_EQ()` byte a byte.** `gldraw.cpp:252-259` vs `core/rend/sorter.cpp:192-196` — mesmos campos comparados. O comentário no código (`gldraw.cpp:248-251`) diz que a duplicação é deliberada, por segurança de reversão. Confirmado via `git show d0c4b5027`.

**1.5 [NOVO] `PP_SameGPUState`/`PP_EQ` é mais conservador do que `SetGPState()` precisa.** Comparando o que `SetGPState()` (`gldraw.cpp:102-238`) realmente *lê* de `isp` contra o layout de bits de `ISP_TSP` (`core/hw/pvr/ta_structs.h:75-104`):
- `SetGPState` só lê `isp.CullMode` (linha 215), `isp.DepthMode` (linha 224), `isp.ZWriteDis` (linhas 233-236) — 6 bits no total.
- `ISP_TSP` tem 32 bits: esses 6, mais campos não usados pelo renderer, mais 20 bits `Reserved`.
- A comparação usa `isp.full == isp.full` (todos os 32 bits), não uma máscara dos 6 bits que importam.

Duas strips adjacentes que diferem só em bits que `SetGPState` nunca inspeciona (`Reserved`/`CacheBypass`/`DCalcCtrl`/`UV_16b`) serão julgadas "diferentes" e NÃO fundidas, mesmo que o estado GL resultante fosse idêntico. **Não é um risco de correção** (só pode causar fusões *perdidas*, nunca erradas) — é dinheiro deixado na mesa. Impacto real não medido (depende de esses bits realmente diferirem na prática, o que é plausível que não aconteça quase nunca).

**1.6 [NOVO] `texid` é excluído da comparação por design, e a suposição que justifica isso tem um buraco estreito.** Comentário em `gldraw.cpp:248-251`: *"texid não é comparado separadamente porque é derivado só de tsp+tcw."* Rastreado: `PolyParam::texid` vem de `renderer->GetTexture(tsp, tcw)` → `gl_GetTexture(tsp, tcw)` (`gltex.cpp:361-390`) → `TexCache.getTextureCacheData(tsp, tcw)`, cuja chave de cache é um hash determinístico de bits mascarados de `tsp`/`tcw`. Dado `tsp.full`/`tcw.full` iguais (que `PP_SameGPUState` já exige), os dois `PolyParam`s *deveriam* sempre resolver pra mesma entrada de cache — **exceto** em `gltex.cpp:378-386`:
```cpp
if (tf->IsCustomTextureAvailable())
{
    glcache.DeleteTextures(1, &tf->texID);
    tf->texID = glcache.GenTexture();
    tf->CheckCustomTexture();
}
```
Se `settings.rend.CustomTextures` estiver ligado e um carregamento assíncrono de textura customizada terminar *entre* duas chamadas `GetTexture()` pro mesmo `tsp`/`tcw` dentro do mesmo `ta_parse_vdrc()`, a segunda chamada recebe um `texID` **diferente** da primeira pro mesmo `tsp`/`tcw`. Se esses dois `PolyParam`s ficarem adjacentes na lista batchada, `PP_SameGPUState` diria "mesmo estado" (nunca olha `texid`), `SetGPState()` só seria chamado uma vez (pro primeiro param), e o draw fundido vincularia a textura do primeiro strip pros dois. **Mecanismo plausível pra exatamente o tipo de corrupção visual que a tentativa anterior do "Gemini" causou** — não reproduzido, não confirmado que `CustomTextures` está ligado nesta config ativa. Gatilho estreito (custom textures + timing assíncrono).

**1.7 [NOVO, menor] Caminho morto no branch de índice 16-bit do batching.** `gldraw.cpp:330-337` trata `gl.index_type == GL_UNSIGNED_SHORT` dentro do branch de fusão — mas `gl.index_type` só é `GL_UNSIGNED_SHORT` no branch GLES2 de `findGLVersion()`, e `canBatch` exige `gl.gl_major >= 3` (GLES3, onde `index_type` fica `GL_UNSIGNED_INT`). Inalcançável no fluxo atual — inofensivo, mas um cheiro de manutenção (assume uma correlação silenciosa entre duas variáveis de config).

---

## 2. Mudanças de estado — cache

**2.1 Existe um cache de estado real, e ele filtra quase tudo.** `core/rend/gles/glcache.h` (`class GLCache`, instância `glcache`). Toda chamada `Enable`/`Disable`/`BlendFunc`/`DepthFunc`/`DepthMask`/`StencilFunc`/`StencilOp`/`StencilMask`/`Scissor`/`UseProgram`/`BindTexture`/`CullFace`/`ClearColor`/`TexParameteri` compara contra um valor sombra em cache antes de emitir a chamada real. `DrawList()`/`DrawSorted()` chamam `glcache.Enable(GL_STENCIL_TEST)`/etc. incondicionalmente no topo, mas como passam pelo cache, valores repetidos são no-ops no nível do driver real — só algumas comparações de inteiro. **O código já rastreia estado e pula no-ops; não chama GL incondicionalmente.**

**2.2 Uma exceção: o novo toggle de primitive-restart contorna o cache.** `gldraw.cpp:283-287,351-352` — `(glEnable)(GL_PRIMITIVE_RESTART_FIXED_INDEX)` vai direto pro driver real (deliberado, porque o shadow-state do `glsm` não tem entrada pra esse enum). Descacheado e incondicional, mas dispara só uma vez por `DrawList()`, não por draw call — impacto negligível.

**2.3 `std::map` pra parâmetros de textura permanece o design (já sinalizado, item 4.3).** `glcache.h:186-215,274-282,316` — árvore rubro-negra por id de textura, consultada em toda chamada `TexParameteri`. O batching reduz a *contagem* de chamadas `SetGPState()` (menos chamadas fundidas rodam uma vez em vez de N), então esse custo escala pra baixo junto, mas a busca em árvore em si não foi endereçada.

**2.4 `glCheck()` é uma macro vazia.** `gles.h:38`: `#define glCheck()`. Confirma o "descartado" do tech_debits — custo zero em runtime.

---

## 3. Manuseio de textura

**3.1 Upload: `glTexImage2D` no caso comum; `glTexStorage2D`/`glTexSubImage2D` só pra texturas com mipmap na primeira atualização.** `gltex.cpp:40-172` (`TextureCacheData::UploadToGPU`):
- Caminho sem mipmap (linha 136): `glTexImage2D(...)` — **toda** atualização respecifica o armazenamento completo, sem fallback pra `glTexSubImage2D` em atualizações do mesmo tamanho.
- Caminho com mipmap, GLES3+ (linhas 83-121): `glTexStorage2D` só `if (Updates == 1)`, depois `glTexSubImage2D` por nível nas atualizações seguintes — esse caminho evita realocar.
- Caminho com mipmap, fallback pré-GLES3 (linhas 122-132): volta pra `glTexImage2D` por nível, toda atualização.

`NeedsUpdate()` usa uma flag `dirty` da proteção de escrita da VRAM, não por-frame — então uma textura só é re-enviada se o jogo realmente reescrever aquela região de VRAM. Custo real só pra texturas que o jogo anima ativamente, não universal.

**3.2 Custo de conversão CPU-side é O(w×h) por textura atualizada, já documentado (item 4.7).** `TexCache.cpp:527-745`. Loops de geração de mipmap fazem uma chamada `texconv*()` por nível, cada uma O(contagem de texel). Não redrivado — o item 4.7 já marcou "esperado/inevitável, baixa prioridade". Adiciono só: uma textura com mipmap paga ~4/3× o trabalho de conversão do nível base, em cima de qualquer upload — pior caso é textura com mipmap frequentemente "dirty".

**3.3 Cache de textura evita re-upload de texturas inalteradas — confirmado.** `gl_GetTexture()` só chama `tf->Update()` quando `tf->NeedsUpdate()` é true. `ComputeHash()` só é chamado dentro do bloco `DumpTextures`, não no caminho normal.

**3.4 [NOVO] `BindRTT()` recria FBO + renderbuffer + textura do zero em toda invocação de render-to-texture, sem cache/pool entre frames.** `gltex.cpp:186-257`:
```cpp
void BindRTT(u32 addy, u32 fbw, u32 fbh, u32 channels, u32 fmt)
{
    if (gl.rtt.fbo) glDeleteFramebuffers(1,&gl.rtt.fbo);
    if (gl.rtt.tex) glcache.DeleteTextures(1,&gl.rtt.tex);
    if (gl.rtt.depthb) glDeleteRenderbuffers(1,&gl.rtt.depthb);
    ...
    glGenRenderbuffers(...); gl.rtt.tex = glcache.GenTexture(); ...
    glTexImage2D(...); glGenFramebuffers(...); ...
```
Roda uma vez por `TA_context` marcado RTT — jogos com efeitos de render-to-texture (espelhos, mini-jogos dentro do gabinete) podem disparar isso mais de uma vez por frame visível. Cada passagem destrói e reconstrói o trio FBO/renderbuffer/textura completo em vez de reusar um cache do mesmo tamanho. Só afeta jogos que usam RTT (minoria da biblioteca DC/Naomi) — não medido quantos passes/frame algum jogo específico faz.

**3.5 `glReadPixels` existe, mas só opt-in e só pra frames RTT.** `gltex.cpp:296,304` dentro de `ReadRTTBuffer()`, condicionado por `settings.rend.RenderToTextureBuffer` (default `false`, `core/nullDC.cpp:537`, só ligável via opção libretro `_enable_rttb`). Quando desligado (padrão), reusa a textura já renderizada na GPU diretamente (`gltex.cpp:339-346`) — sem readback de CPU nenhum. **Por padrão, nenhum `glReadPixels` acontece.**

---

## 4. Uso de framebuffer/render target

**4.1 Uso de máscara em `glClear` já é boa prática, não defeito.** `gles.cpp:933-937` — cor deliberadamente não limpa ali (o plano de fundo do PVR já sobrescreve tudo), depth+stencil combinados num único `glClear` (padrão recomendado pra GPU tile-based: uma operação de limpeza de tile em vez de duas). Sidebar (non-widescreen) corretamente com scissor só na região pequena. **Achado positivo, não problema.**

**4.2 [NOVO] Zero uso de `glInvalidateFramebuffer`/`GL_EXT_discard_framebuffer` em toda a árvore.** Grep exaustivo em `core/` — zero ocorrências. Concretamente:
- O framebuffer principal (depth+stencil), limpo no topo de todo `RenderFrame()` e escrito o frame inteiro, nunca é avisado "descarte, não escreva de volta" antes do frame terminar/ser apresentado — em GPU tile-based Mali isso significa que o conteúdo de depth+stencil na memória de tile é resolvido/gravado de volta na memória do sistema mesmo que nada nunca leia isso de volta (depth é limpo de novo no próximo frame).
- Todo ciclo `BindRTT()`/`ReadRTTBuffer()` anexa um renderbuffer de depth+stencil fresco só necessário durante aquele sub-render e deletado logo depois — de novo, nunca invalidado antes da deleção.

**Resposta direta à pergunta explícita sobre dicas específicas de GPU tile-based: está completamente ausente.** Certo sobre a ausência (grep exaustivo, zero hits); custo real no Mali-G31 não medido nesta sessão.

**4.3 Contagem de FBO por frame.** Um ciclo FBO de RTT por `TA_context` RTT (recriado do zero, ver 3.4); o caminho de "postprocess" (ver 4.4) é código morto, contribui zero FBOs; fora isso, a renderização vai pro framebuffer que o callback `hw_render` do frontend libretro fornece — flycast não é dono/cria o framebuffer de apresentação principal. **0 FBOs extras/frame pra jogos sem RTT, 1 (recriado) por sub-render RTT pra jogos com RTT.**

**4.4 Pós-processamento é código morto completo, confirmando a nota do tech_debits.** `postprocess.cpp:242-355` — todo corpo de função está em bloco de comentário. `settings.rend.PowerVR2Filter = false;` hardcoded (`libretro.cpp:899`). Os call sites em `gles.cpp` são portanto permanentemente inalcançáveis neste fork.

---

## 5. Sincronização/threading

**5.1 Nenhuma chamada `glFinish`/`eglSwapBuffers`/`SwapBuffers`/`glFlush` em lugar nenhum do código de renderização.** Grep exaustivo — zero call sites reais (só declarações de typedef/extern não invocadas). Apresentação é inteiramente dono do frontend libretro via `video_cb()`. `Renderer::Present()` pro backend GLES é a implementação base no-op — só faz algo (`dc_stop()`) quando `ThreadedRendering` está DESLIGADO; com ele ligado (a config em questão), `Present()` é puro no-op.

**5.2 [NOVO] A thread de emu é liberada da espera de "render pronto" assim que o *parsing* do TA termina, não quando a submissão GL termina — no caso comum.** `core/hw/pvr/Renderer_if.cpp:159-179` (`rend_frame`):
```cpp
bool rend_frame(TA_context* ctx, bool draw_osd)
{
   bool proc = renderer->Process(ctx);              // parsing do TA + sort, SEM chamadas GL
   if (settings.rend.ThreadedRendering &&
       (!proc || (!ctx->rend.isRenderFramebuffer && !ctx->rend.isRTT)))
      re.Set();                                      // <-- dispara AQUI, antes de Render()
   bool do_swp = proc && renderer->Render();          // submissão real das chamadas de draw GL
   return do_swp;
}
```
Pra uma cena típica (não RTT, não escrita direta em framebuffer), `re.Set()` desbloqueia a thread de emulação **antes** de `renderer->Render()` — a função que emite os ~500-600 `glDrawElements` por frame pesado (item 4.2) — sequer começar. Isso significa a thread de emulação pode começar a simular o *próximo* frame enquanto a thread de render ainda está submetendo comandos GL do frame *atual* — sobreposição real de CPU/submissão-GPU. Pra contextos RTT, `re.Set()` é deliberadamente postergado pra depois que `Render()` termina — corretamente, já que RTT precisa que a CPU veja o render terminado.

**5.3 [NOVO] Mas a fila de render tem capacidade exatamente 1, e um `Render()` lento causa o próximo frame ser silenciosamente descartado, não só atrasado.** `core/hw/pvr/ta_ctx.cpp`:
- `QueueRender()` (linhas 113-166): `if (rqueue) { tactx_Recycle(ctx); return false; }` — se um contexto já está na fila e não foi *totalmente terminado*, o novo é descartado (frameskip), não enfileirado atrás dele.
- `FinishRender()` (linhas 189-199) é a única função que limpa `rqueue = NULL`, chamada de `rend_single_frame()` só *depois* de `rend_frame()` completo — ou seja, depois de `Process()` **e** `Render()` (a submissão GL completa).

Então o "slot" que decide se o próximo frame produzido pela thread de emu sobrevive ao enfileiramento fica ocupado durante TODO o Process+Render, não só o Process. Combinado com 5.2: a thread de emu ganha a chance de correr adiante (bom pra throughput de CPU), mas se ela terminar de simular o frame N+1 e chamar `rend_start_render()` → `QueueRender()` enquanto a thread de render ainda está dentro de `Render()` do frame N, o `TA_context` do frame N+1 é descartado como frameskip. **Isso dá uma explicação mecânica direta pra por que a redução de draw-call do item 4.2 mediu ganho de FPS**: não é só cortar tempo de CPU dentro de `Render()`, é também cortar a *janela* durante a qual frames entrantes são descartados por essa fila de slot único, ou seja, reduzir a taxa de frameskip em cenas pesadas. Handoff genuinamente single-buffered (não double/triple) pra contextos TA — o `ctx_pool` de 2 (item 3.6) é sobre reuso de alocação de objetos `TA_context`, não sobre profundidade de fila; a fila real de render pendente (`rqueue`) tem profundidade 1.

**5.4 Bind/unbind de contexto (`glsm_ctl`) é um shim de save/restore de estado, não primitiva de sincronização.** `gles.cpp:1136-1149`, `libretro.cpp:1276,1282` — wrapper porque o contexto GL é compartilhado com o frontend libretro (RetroArch) via o shim `glsm`. Bookkeeping, não sincronização — mencionado só porque poderia ser confundido com uma.

---

## 6. Catálogo de fragilidade da área de risco conhecida (batching)

Dado o histórico documentado do item 4.2 (uma tentativa anterior do "Gemini" quebrou a renderização de personagens do Shenmue enquanto genuinamente melhorava FPS, depois corrigida nesta sessão com a abordagem de primitive-restart e revalidada visualmente):

1. **§1.6 (corrida texid/CustomTextures)** — a lacuna latente mais concreta: `PP_SameGPUState` assume que `texid` acompanha `tsp`/`tcw` deterministicamente, verdade exceto através de uma fronteira de carregamento assíncrono de textura customizada. Gatilho estreito, não reproduzido, mas estruturalmente exatamente o padrão "suposição sobre estado que não é realmente invariante".
2. **§1.5 (comparação `isp.full` ampla demais)** — não é risco de correção (só pode causar fusões *perdidas*), mas sinaliza que a checagem de igualdade de estado foi copiada de outro call site sem ser rederivada do que o novo chamador realmente consome — o tipo de atalho que, se alguém "corrigir" afrouxando a máscara sem reverificar cada leitura de `SetGPState`, poderia reintroduzir um bug estilo Gemini.
3. **Recursos de scratch compartilhados entre instanciações de template e entre funções**: `gl.vbo.idxs2` é usado tanto por `SortTriangles()` quanto pelo novo caminho de fusão do batching — seguro hoje porque os dois caminhos são mutuamente exclusivos por passe de render, mas é um invariante implícito não documentado nem verificado em lugar nenhum.
4. **Correção do primitive-restart através de fronteiras de strip é afirmada por comentário, não por checagem em runtime** — consistente com a spec GLES3 e validado visualmente pelo usuário, mas sem asserção/teste no próprio código.
5. **Tratamento de strip degenerada (`count <= 2`) interage com a construção de "runs" de forma correta mas não óbvia** — fácil de quebrar se o loop for refatorado.

---

## Lista priorizada pra medição real (do próprio subagente)

1. **§5.3 — comportamento de frameskip da fila `rqueue` de slot único sob `Render()` lento.** Reformula o ganho do item 4.2 de "menos tempo de CPU" pra possivelmente também "menos frames descartados" — muda qual métrica importa (taxa de frameskip/percentis de tempo de frame, não só `core_average`). Barato de instrumentar: um contador no branch de descarte de `QueueRender()`, correlacionado com a duração de `Render()`.
2. **§1.6 — segurança de fusão texid/CustomTextures.** Se `settings.rend.CustomTextures` for ligado algum dia neste device, é o mecanismo mais plausível pra uma regressão visual estilo Gemini reemergir do código de batching *atual*.
3. **§4.2 — ausência total de `glInvalidateFramebuffer`.** A lacuna mais direta e nomeada explicitamente na pergunta, e inequivocamente verdadeira (zero ocorrências) em vez de probabilística. Vale um A/B real.
4. **§3.4 — churn de FBO/renderbuffer/textura de RTT.** Só importa pra jogos com RTT, mas se algum jogo do conjunto de teste desta sessão usa RTT, é um padrão concreto, delimitado, fácil de instrumentar e corrigir.
5. **§1.5 — comparação `isp.full` ampla demais em `PP_SameGPUState`.** Prioridade mais baixa das cinco (só pode perder oportunidade de fusão, nunca correção), mas mais barato de *medir*.
