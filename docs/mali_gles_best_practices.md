# OpenGL ES em ARM Mali (Bifrost / Mali-G31): referência com fontes

> Gerado por subagente de pesquisa (WebSearch/WebFetch, fontes primárias lidas
> diretamente — inclusive o PDF de 135 páginas do guia oficial da ARM via
> `pdftotext`), 2026-09-15. Contexto de hardware: RK3326, Mali-G31 MP2, OpenGL
> ES 3.2, sem Vulkan usável. Mali-G31 é geração **Bifrost** — usa o modelo de
> submissão de comando **Job Manager (JM)**, não o **Command Stream Frontend
> (CSF)** introduzido no Mali-G710/Valhall-gen2. Essa distinção importa muito
> pro ponto 3 abaixo.
>
> Ver `docs/gles_code_audit.md` (auditoria do nosso código) e
> `docs/rendering_improvement_plan.md` (plano cruzando os dois).

**Tags:**
- **[MALI-ESPECÍFICO]** — da documentação oficial ARM/Mali/Bifrost
- **[TBDR-GERAL]** — se aplica a renderizadores tile-based em geral (Mali, PowerVR, Adreno), não GPUs desktop de modo imediato
- **[GENÉRICO-GL]** — orientação geral de OpenGL/arquitetura de driver, não específica de TBDR ou Mali

---

## 1. Boas práticas oficiais da ARM

Fonte primária: **Arm® GPU Best Practices Developer Guide**, ID `101897_0304_10_en`, Rev 3.4, Issue 10, publicado 2025-01-31. https://developer.arm.com/documentation/101897/latest (PDF completo lido em https://documentation-service.arm.com/static/67a62b17091bfc3e0a947695).

**1a. Minimização de draw calls — meta numérica [MALI-ESPECÍFICO]**
> "Para OpenGL ES, busque menos de 500 draw calls por frame." — §3.2

O flycast, com "dezenas a poucas centenas" de draw calls por frame, está confortavelmente abaixo dessa meta — pela própria orientação da ARM, contagem de draw call por si só provavelmente não é o gargalo. Mas o guia é explícito que o custo real é overhead de despacho de CPU, não processamento de GPU: *"Draw calls com poucos vértices/fragmentos são rápidas de processar na GPU, comparado ao tempo de CPU necessário pra despachar o trabalho."*

**1b. Modelo de custo de mudança de estado [MALI-ESPECÍFICO]**
> "Tente minimizar mudanças de estado da API... rastreando o estado atual, só fazendo mudanças necessárias. Pra OpenGL ES, remova chamadas glEnable()/glDisable() redundantes se nenhuma mudança de estado ocorreu." — §3.1

Corroborado pelo blog da ARM "Mali Performance 5": *"A maioria das mudanças de estado tem custo baixo, mas não zero, porque o driver precisa fazer checagem de erro e configurar estado numa estrutura de dados interna."* — ou seja, até uma mudança de estado redundante/no-op custa tempo de CPU em validação do lado do driver.

**1c. Métodos de upload de textura [MALI-ESPECÍFICO]**, §3.6/§8.1:
> "Evite as seguintes operações OpenGL ES síncronas: ... glCopyTexImage(), enquanto a textura de destino ainda é referenciada por um draw call em andamento. glTexSubImage(), enquanto a textura de destino ainda é referenciada por um draw call em andamento." — §3.6

Alocação imutável (`glTexStorage2D`) é preferida porque deixa o driver começar a cópia host→device imediatamente em vez de esperar até o momento do draw.

**1d. Custo de amostragem/formato de textura [MALI-ESPECÍFICO]**, §8.4:
> "Pra recursos estáticos, use compressão de textura offline, como ETC2 ou idealmente ASTC." Também: "formatos FP32 têm custo 2x," "formatos 3D têm custo 2x," "use samplers mediump — samplers highp podem ser a metade da velocidade."

**1e. Dicas de "início/fim de frame" TBDR — modelo de passe de render [MALI-ESPECÍFICO]**, §7.2:
> "Ao iniciar um passe de render, limpe ou invalide todo attachment... No fim do passe, invalide qualquer attachment não necessário fora do passe, antes de trocar o binding de framebuffer pro próximo FBO."
> "Não alterne de volta pra renderizar no mesmo FBO múltiplas vezes num frame. Complete cada passe de render numa única chamada glBindFramebuffer() antes de seguir pro próximo. **No Valhall, é especialmente importante minimizar troca de FBO porque o driver faz um flush em glBindFramebuffer().**"
> "Não divida um passe de render usando glFlush() ou glFinish()."

**A afirmação mais diretamente utilizável de toda a pesquisa: trocar de FBO é um gatilho de flush documentado, independente de qualquer chamada de sync explícita.**

---

## 2. Armadilhas específicas de TBDR

**2a. Clears parciais derrotam o caminho de fast-clear [TBDR-GERAL, documentado pela ARM]**
Blog "Mali Performance 2": *"Um erro comum é limpar só parte do framebuffer — chamar glClear() com um scissor rect que só cobre parte da tela. Só conseguimos descartar completamente o estado de render quando ele se aplica a superfícies inteiras, então uma limpeza da superfície inteira deveria ser feita quando possível."* Confirmado de novo no guia oficial §7.2.

**2b. glInvalidateFramebuffer/EXT_discard_framebuffer — o TIMING importa, não só a presença [TBDR-GERAL/MALI-ESPECÍFICO]**
Mesmo post "Mali Performance 2":
> "O erro mais comum nesse ponto é tratar glInvalidateFramebuffer() como equivalente a glClear() e colocar a chamada de invalidação do estado do frame N no primeiro uso daquele FBO no frame N+1. **Isso é tarde demais!** ... buffers transitórios no frame N devem ser indicados chamando glInvalidateFramebuffer() **antes** de desvincular o FBO no frame N."
> "O Mali libera (flush) o trabalho de renderização quando um render target é 'desvinculado', exceto pra superfície principal da janela, que é liberada quando o driver vê uma chamada a eglSwapBuffers()."

Isso explica diretamente *por que* `glInvalidateFramebuffer`/`EXT_discard_framebuffer` existem pra depth/stencil no Mali: se o driver não é avisado ANTES do flush que um buffer é transitório, ele escreve de volta na memória do sistema por nada — banda pura desperdiçada todo frame que não chama isso corretamente.

**2c. glReadPixels/leituras de framebuffer no meio do tile drenam o pipeline [TBDR-GERAL]**
Samsung Developer, "OpenGL ES Usage Recommendations": leituras como `glReadPixels` "drenam" o pipeline; recomenda PBOs pra evitar a espera síncrona. O guia oficial da ARM lista `glReadPixels()` explicitamente na lista de "evitar" junto com `glFinish()`.

**2d. Dependência de ordem de blending/early-Z (custo de overdraw em GPU tile) [MALI-ESPECÍFICO]**
Guia oficial §3.4: *"Todas as GPUs Arm desde a Mali-T620 incluem a otimização Forward Pixel Kill (FPK)... Situações onde o FPK comumente falha incluem draw calls usando: transparência com alpha blending. Acesso programático ao framebuffer no shader. Teste ZS tardio. Triângulos pequenos."* E §7.9: *"Blending tem impacto significativo na performance porque desabilita muitas otimizações importantes de remoção de overdraw."* Relevante pro flycast porque os passes de "modifier volume"/translucente e polígonos punch-through do PVR são exatamente esse padrão de falha.

---

## 3. Padrões de sincronização CPU/GPU pra renderização threaded em mobile

**Seção mais relevante pro call graph "cheio de poll/mutex/semaphore" que já medimos.**

**3a. Lista explícita de "evitar" da própria ARM [MALI-ESPECÍFICO]** — §3.6:
> "OpenGL ES expõe um modelo de renderização síncrono aos usuários da API, apesar de usar execução assíncrona na GPU... evite operações que fazem o driver drenar o pipeline da GPU e privar a GPU de trabalho."
> "Evite: glFinish(), glReadPixels(), glCopyTexImage() enquanto a textura ainda está em uso, glTexSubImage() enquanto a textura ainda está em uso, glMapBufferRange() sem GL_MAP_UNSYNCHRONIZED enquanto o buffer ainda está em uso."
> "Drenos de pipeline aparecem como períodos de tempo ocupado oscilando entre CPU e GPU, sem CPU ou GPU estarem totalmente utilizados." (orientação de debug)

Essa última frase é uma assinatura de diagnóstico genuinamente útil: **tempo ocupado oscilando entre CPU/GPU, nenhum totalmente utilizado** é o que a própria ARM diz que um dreno de pipeline parece — vale conferir contra qualquer captura de profiling do flycast.

**3b. Modelo de threading cliente/servidor do driver, e por que MAP_UNSYNCHRONIZED pode ser pior que nada [GENÉRICO-GL]**
Cass Everitt & John McDonald (NVIDIA), "Beyond Porting: How Modern OpenGL can Radically Reduce Driver Overhead," Steam Dev Days 2014:
> "MAP_UNSYNCHRONIZED evita um ponto de sync CPU-GPU, **mas faz as threads Cliente e Servidor serializarem.** Isso força todo trabalho pendente na thread servidor a completar. É bem caro (quase sempre precisa ser evitado)."

Arquitetura de driver desktop (NVIDIA), mas a divisão cliente/servidor é arquiteturalmente geral, e o guia da ARM corrobora o mesmo modo de falha pro Mali sob outro nome (a lista de "evitar" do §3.6 é o equivalente Mali exato desse mesmo stall).

**3c. Mali é dividido arquiteturalmente em JM (pré-CSF, o que o G31 usa) vs CSF (G710+) — e isso muda o custo base de CPU por draw [MALI-ESPECÍFICO, diretamente relevante pro G31]**
Blog "Mali-G710: a developer overview": *"O Command Stream Frontend (CSF) substitui o Job Manager encontrado em produtos anteriores... o driver agora só precisa transmitir as mudanças de estado de cada draw, em vez de reemitir o estado inteiro... A maior melhoria que o CSF dá é uma redução significativa no uso de CPU do driver Mali."*

Implicação clara (dita pela ARM sobre o chip mais NOVO, mas informativa por contraste sobre o mais velho): GPUs Job-Manager pré-CSF — Midgard e **todo Bifrost, incluindo Mali-G31** — pagam um custo base de CPU por draw call mais alto no driver de userspace porque mais estado precisa ser revalidado/reemitido por draw, em vez de incrementalmente. Isso é arquitetural, não bug — significa que o overhead de driver por-draw-call do G31 é inerentemente pior que um Mali moderno fazendo a mesma carga de trabalho.

**3d. Como o caminho de submissão de job do kbase realmente funciona (por que poll/mutex/semaphore aparece)** [MALI-ESPECÍFICO, nível de interface de kernel, de documentação comunitária de engenharia reversa — sinalizado como tal]
`ioctl(KBASE_IOCTL_JOB_SUBMIT) → kbase_jd_submit()` com um array de descritores `base_jd_atom`; `read()`/`poll()` recupera registros de conclusão `base_jd_event` da fila de eventos por contexto. Isso confirma o mecanismo: no Mali de era JM (G31 incluso), o driver de userspace submete trabalho via `ioctl` e legitimamente usa `poll()`/leituras bloqueantes pra saber quando jobs da GPU terminam — então **algum tempo de poll/mutex/semaphore no call graph do driver é simplesmente espera de GPU normal e necessária, não um bug.**

**Heurística pra distinguir (a) espera genuína de (b) sincronização excessiva induzida pela app**, sintetizando 3a-3d: espera genuína de GPU (a) deveria correlacionar com eventos de fronteira de frame (`eglSwapBuffers`, ou readback que a app realmente precisa) e escalar com o tempo de frame da GPU. Sincronização excessiva induzida pela app (b) aparece como esperas *extras* correlacionadas com chamadas de API específicas que a própria lista de "evitar" da ARM nomeia explicitamente — `glTexSubImage2D`/`glCopyTexImage2D` numa textura ainda em uso, `glMapBufferRange` sem `UNSYNCHRONIZED`, troca de FBO no meio do frame, ou qualquer `glFinish`/`glReadPixels` fora de um readback de fim-de-frame de verdade. **Isso não é algo determinável só por documentação** — precisa correlacionar os hotspots de poll/mutex do call graph contra uma linha do tempo das chamadas GL reais do flycast.

---

## 4. Batching de draw call / minimização de mudança de estado (geral)

**4a. O custo relativo de diferentes mudanças de estado NÃO é uniforme [GENÉRICO-GL — números desktop/NVIDIA, citados pelo princípio *relativo*, não como throughput literal do Mali]**
"Beyond Porting", slide "Relative costs of State Changes" (driver desktop NVIDIA, 2014, "não em escala"):
```
Render Target        ~60K / s
Programa              ~300K / s
Bind de Textura       ~1.5M / s
Update de Uniform     ~10M / s
```
**Preciso sinalizar claramente: esses números de throughput são de um driver desktop NVIDIA de 2014, não Mali.** Só o *princípio de ordenação* se transfere — mudanças de render-target/FBO são muito mais caras que rebind de textura, que são muito mais caras que update de uniform — e esse princípio é corroborado independentemente pelo próprio guia da ARM tratando troca de FBO como gatilho de flush (§7.2) enquanto trata bind de textura/uniform como "não gratuito mas baixo custo".

**4b. Mudanças de estado redundantes ainda custam algo mesmo sendo no-ops [GENÉRICO-GL]**
Khronos OpenGL Wiki, "Common Mistakes" — referência padrão da comunidade pra essa classe de erro. A razão mecânica, do próprio §3.1 da ARM: *"o driver vai fazer checagem de erro e configurar estado numa estrutura de dados interna"* mesmo pra um set redundante — ou seja, rastreamento de estado "sujo" do lado da *aplicação* (pular a chamada inteiramente se o estado não mudou) é estritamente melhor que confiar no driver pra detectar o no-op.

---

## 5. Custo de upload/conversão de textura (paletizada/4-bit → RGBA na CPU vs GPU-nativo)

**5a. A solução histórica dedicada existiu e foi deliberadamente descontinuada [GENÉRICO-GL/histórico]**
`GL_OES_compressed_paletted_texture`: *"Formatos paletizados são texturas que guardam um número pequeno de cores como paleta, e o array de texel é um array simples de índices nessa paleta... o benefício é que a paleta pode ser cacheada, e os índices têm custo de banda menor que uma cor completa."* *"Em versões seguintes do OpenGL ES, essa funcionalidade pode ser facilmente replicada com shaders programáveis usando uma textura de paleta pequena, então a extensão foi descontinuada."*

**Citação mais diretamente relevante pro ponto 5.** Confirma: (1) o conceito de evitar expansão de paleta no lado da CPU enviando índices crus + uma textura de paleta pequena é uma técnica GL *estabelecida*, não exótica; (2) originalmente era extensão de função fixa (só GLES 1.x, legado/descontinuada) mas (3) é totalmente replicada hoje enviando os dados de índice como textura simples de 8-bit (ou 4-bit compactado) mais uma textura de paleta pequena (ex. 16×1 ou 256×1 RGBA), fazendo a busca de paleta no fragment shader com `texelFetch`/`texture()`. **Bate direto com o problema do flycast**: texturas PowerVR CLUT4/CLUT8 atualmente precisam de expansão CPU-side pra RGBA em algum lugar do pipeline, e o padrão de busca-por-shader é o jeito documentado e sem dependência de extensão de evitar esse custo de CPU inteiramente.

**5b. Um emulador real já validou exatamente essa troca [GENÉRICO-GL/precedente de emulador]**
Guia de performance do Dolphin: *"GPU Texture Decoding move a decodificação de textura da CPU pra GPU... vai dar um benefício maior em CPUs mais fracas... voltado pra quem tem CPU mais lenta, pra aliviar parte do overhead movendo esse trabalho pra GPU."* Dolphin emula GameCube/Wii, que — como o PowerVR do Dreamcast — tem formatos de textura nativos hostis à CPU (paletizados, tiled/swizzled). O caminho de otimização do próprio Dolphin foi exatamente "tirar a decodificação de formato da CPU," implementado via compute shaders no caso deles. Confirmação independente, específica de emulador, de que conversão de formato de textura CPU-side é uma classe de gargalo real e conhecida nessa categoria exata de software, e decodificação GPU-side (ou no mínimo shader-side) é a correção aceita. Dado que a CPU fraca Cortex-A53 do RK3326/Mali-G31 é exatamente o caso de "CPU mais fraca" que os próprios docs do Dolphin apontam como o que mais se beneficia, isso generaliza bem pro flycast.

**5c. Formatos comprimidos GPU-nativos (ASTC/ETC2) como alternativa — mas provavelmente N/A pras texturas *vivas* do flycast [MALI-ESPECÍFICO]**
O guia da ARM recomenda ETC2/ASTC "pra recursos estáticos". **Ressalva que importa aqui:** ASTC/ETC2 são formatos comprimidos em bloco pra assets *estáticos, comprimidos offline* — não servem pra texturas do Dreamcast/Naomi que chegam vivas, quadro a quadro, direto da VRAM do próprio jogo em formato nativo do PowerVR (frequentemente paletizado), já que o flycast não tem chance de codificar offline. **Provavelmente não aplicável** ao caminho de cache de textura vivo do flycast — a abordagem CLUT/busca-por-shader do ponto 5a é a que realmente cabe nesse formato de problema.

---

## Se eu tivesse que apostar: top 3 candidatos pra explicar os 25-30% de CPU no driver Mali, com padrão poll/mutex/semaphore

**Esta seção é explicitamente uma hipótese, não conclusão medida — deveria ser validada com o mesmo rigor de qualquer outro achado do `docs/tech_debits.md` antes de agir sobre ela.**

1. **Updates de textura acertando recursos ainda em uso** (lista de "evitar" do §3.6 — `glTexSubImage2D`/`glCopyTexImage2D` numa textura ainda referenciada). O flycast reenvia texturas PVR do cache CPU-side toda vez que o conteúdo muda; se algum desses uploads visa um objeto de textura GL ainda referenciado por um draw call que a GPU não terminou de consumir, o driver, pela documentação da ARM, drena o pipeline ou faz cópia-fantasma do recurso — ambos se manifestariam exatamente como o padrão poll/mutex/semaphore-pesado descrito, e ambos são evitáveis fazendo round-robin de objetos de textura.
2. **Troca de FBO/render-target no meio do frame** — o pipeline PVR do flycast comumente precisa de múltiplos passes de render por frame (opaco, punch-through, translucente/modifier-volume, e qualquer render-to-texture pra efeitos). A ARM documenta explicitamente `glBindFramebuffer` como ponto de flush (pior confirmado no Valhall, mas o modelo de inferência de passe de render do §7.2 é genérico pra toda a família de driver GLES Mali incluindo Bifrost) — cada bind-e-volta extra é um flush síncrono real, não uma dica. Isso é o tipo de coisa que aparece como espera interna do driver em vez de tempo de shader da GPU, batendo melhor com a descrição "poll/mutex-pesado" do que uma explicação crua de fill-rate/shader-bound.
3. **Overhead base de driver por-draw da era JM** — separado de qualquer bug, o próprio contraste da ARM entre Job-Manager (geração do G31) e CSF (§3c acima) diz que o custo de CPU por draw call *arquiteturalmente esperado* no driver Mali é mais alto no G31 do que seria num Mali moderno fazendo a mesma cena idêntica, porque o modelo JM revalida mais estado por draw em vez de transmitir deltas incrementais. Se isso for o contribuinte dominante, os 25-30% podem ser substancialmente "o custo de fazer negócio" nessa geração de GPU em vez de um padrão do lado da app corrigível — nesse caso a alavanca é upstream do driver: reduzir contagem de draw call e de mudança de estado no nosso próprio backend OpenGL (batching de polígonos PVR que compartilham estado de textura/blend), não perseguir nada dentro do driver em si.

Distinguir 1-2 (corrigível, induzido pela app) de 3 (piso arquitetural) exige exatamente o tipo de correlação que o próprio CLAUDE.md deste projeto já prescreve: checar se o tempo de poll/mutex/semaphore escala com a frequência de update de textura e contagem de troca de FBO especificamente, ou se escala linearmente com a contagem crua de draw call independente do resto — o primeiro aponta pra 1-2, o segundo pra 3.

---

## Fontes citadas (todas buscadas/lidas diretamente nesta sessão)

- Arm® GPU Best Practices Developer Guide, Rev 3.4 (101897_0304_10_en) — https://developer.arm.com/documentation/101897/latest
- Mali Performance 2: How to Correctly Handle Framebuffers — https://developer.arm.com/community/arm-community-blogs/b/mobile-graphics-and-gaming-blog/posts/mali-performance-2-how-to-correctly-handle-framebuffers
- Mali Performance 5: An Application's Performance Responsibilities — https://developer.arm.com/community/arm-community-blogs/b/mobile-graphics-and-gaming-blog/posts/mali-performance-5-an-application-s-performance-responsibilities
- Mali Performance 6: Efficiently Updating Dynamic Resources — https://developer.arm.com/community/arm-community-blogs/b/mobile-graphics-and-gaming-blog/posts/mali-performance-6-efficiently-updating-dynamic-resources
- The Mali GPU: An Abstract Machine, Part 2 — Tile-based rendering — https://developer.arm.com/community/arm-community-blogs/b/mobile-graphics-and-gaming-blog/posts/the-mali-gpu-an-abstract-machine-part-2---tile-based-rendering
- Mali-G710: a developer overview (JM vs CSF) — https://developer.arm.com/community/arm-community-blogs/b/mobile-graphics-and-gaming-blog/posts/mali-g710-developer-overview
- Mali-G31 product page — https://www.arm.com/products/silicon-ip-multimedia/gpu/mali-g31
- Khronos OpenGL Wiki, Common Mistakes — https://www.khronos.org/opengl/wiki/Common_Mistakes
- Samsung Developer, OpenGL ES Usage Recommendations — https://developer.samsung.com/galaxy-gamedev/resources/articles/opengl.html
- Cass Everitt & John McDonald, "Beyond Porting", Steam Dev Days 2014 — http://media.steampowered.com/apps/steamdevdays/slides/beyondporting.pdf
- GL_OES_compressed_paletted_texture — https://docs.imgtec.com/reference-manuals/open-gl-es-extensions/html/topics/GL_OES/compressed-paletted-texture.html
- Dolphin Emulator Performance Guide — https://dolphin-emu.org/docs/guides/performance-guide/
- mali-kbase kernel interface — https://deepwiki.com/96boards-akebi96/mali-kbase, https://github.com/BradenM/mali-bifrost/blob/main/mali_kbase_core_linux.c
- The Mali CSF Command Stream Instruction Set — https://icecream95.gitlab.io/the-mali-csf-command-stream-instruction-set.html
- Collabora, An overview of the Panfrost driver — https://www.collabora.com/news-and-blog/blog/2019/03/13/an-overview-of-the-panfrost-driver/

**Nota de método:** o site de documentação da ARM é uma SPA que WebFetch puro não renderiza — contornado buscando o PDF completo direto e rodando `pdftotext` localmente. A página do Dolphin retornou 403 (Cloudflare) pro WebFetch, então essa citação vem de cache de mecanismo de busca em vez de fetch direto.
