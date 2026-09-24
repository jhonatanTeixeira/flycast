# Protótipo do `jit_armv8_a`: laço de vértices do Shenmue II

Compara o código que o JIT atual gera para o laço `8C1D8BDA`–`8C1D8C82` (11
blocos) com a versão escrita à mão no estilo do `jit_armv8_a`:
- registradores do SH4 fixos;
- T em registrador;
- `bf/bt +curto` como `csel`/`fcsel`;
- XMTRX em registradores NEON;
- uma checagem de ciclos por volta.

As duas versões rodam sobre o mesmo estado real capturado do jogo.

## Passos

1. **Capturar o estado** na entrada do laço (no device, core com
   `FC_JIT_DUMP`):
   ```
   FC_JIT_DUMP=<dir> FC_CAPTURE_BLOCK=8C1D8BDA:20000
   ```
   Gera `cap-8C1D8BDA-ctx.bin`, `cap-8C1D8BDA-ram.bin` e `jit-<pid>.txt`.
2. **Gerar `blocks.json`** a partir do `jit-<pid>.txt`: extração do corpo de
   cada bloco entre `subs w27` e o início do relink. O script usado está na
   sessão de 2026-09-25; o `blocks.json` desta pasta é o dessa captura.
3. **Montar e rodar:**
   ```
   python3 gen_cur.py        # cur.S: corpo real + saídas como depois de ligado
   aarch64-linux-gnu-gcc-13 -O2 -static -o proto harness.c cur.S new.S
   ./proto <dir da captura> 200000
   ```

O harness confere que registradores, T, FPUL, a memória da SQ e os dados
enviados ao "TA" saem **idênticos**, e só então cronometra.

## Resultado (2026-09-25)

| Estado capturado | Atual | `jit_armv8_a` | Ganho |
|---|---|---|---|
| strip de 14 vértices | 3036 ns | 1781 ns | **1,70×** |
| strip de 2 vértices | 322 ns | 220 ns | 1,46× |

O código do laço fica 3,8× menor (2740 → 724 bytes).

## Limites

- **Cache quente, laço isolado:** não mede o ganho de L1I que o código 3,8×
  menor daria no jogo.
- **SQ:** as escritas na SQ vão direto para a memória nos dois lados. No
  emulador, o código atual usa trampolins nesses pontos, o que é mais caro.
- **Carga e descarga do estado:** a versão nova carrega e descarrega o estado
  a cada entrada. Num JIT de verdade, com registradores fixos, isso sairia do
  caminho quente.

## Correção de 2026-09-24: o protótipo do Shenmue não conferia ciclos

O harness agora também compara o contador de ciclos na saída, o contexto
inteiro e a RAM. Com isso, o `new.S` do Shenmue **falha**: 602 ciclos usados
contra 257 do JIT atual (uma checagem de 86 por volta não é a soma dos blocos
executados). Registradores, TA e SQ continuam iguais; os números de tempo
acima valem só para o código, não para a contabilidade de ciclos. Falta
refazer com a regra 1 do DOA2 abaixo.

## Região do DOA2 (nível 2): laço de vértices, 12 blocos

`doa2/`: `blocks.json` (extraído com `extract_blocks.py`) e `new.S` (região
escrita à mão). Captura no início da strip (8C101BC2), core com `FC_JIT_DUMP`:
```
FC_JIT_DUMP=<dir> FC_CAPTURE_BLOCK=8C101BC2:3000
python3 extract_blocks.py <dir>/jit-<pid>.txt doa2/blocks.json 8C101BC2 8C101BC4 8C101BE2 \
    8C101BEC 8C101BFE 8C101C0C 8C101C20 8C101C26 8C101C2E 8C101C38 8C101C3A 8C101C4E
python3 gen_cur.py doa2/blocks.json 8C101BC2 cur_doa2.S
aarch64-linux-gnu-gcc-13 -O2 -static -o proto_doa2 harness.c cur_doa2.S doa2/new.S
./proto_doa2 <dir> 200000 8C101BC2
```

| Captura | Atual | Região | Ganho |
|---|---|---|---|
| strip de 18 vértices | 4625 ns | 2499 ns | **1,85×** |
| strip de 5 vértices | 1167 ns | 662 ns | 1,76× |

**IDÊNTICO** nas duas: registradores, contexto inteiro, RAM, SQ, dados do TA e
ciclos (341 × 341, 87 × 87). Código quente 2000 → 564 bytes (3,5× menor),
mais 180 de saídas e os caminhos frios da SQ fora da linha.

Regras usadas (candidatas ao gerador do nível 2; cada uma só usa o que se vê
na compilação):
1. **Ciclos:** guarda em cada ponto de entrada com o custo do caminho mais
   longo até a próxima guarda; passou, cada bloco original só subtrai; falhou,
   sai antes de executar para o bloco do JIT antigo. Exatamente equivalente a
   checar por bloco.
2. **Registradores:** SH4 vivo através de chamada C em callee-saved (x19-x26,
   s8-s15 para floats só lidos); o resto em caller-saved; XMTRX (só leitura)
   recarregada com um `ld1` depois de cada chamada.
3. **T vira desvio;** o valor de T em cada saída é conhecido estaticamente.
4. **Registrador derivado** (r7 = r6 da cabeça = r6 − 32 no ponto de uso) não
   ocupa registrador nem é gravado na volta, só na saída.
5. **Stores com pré-decremento consecutivos** (4× `fmov @-r6`) numa base só,
   com guarda contra wrap de 32 bits.
6. **Descarga da SQ = chamada C completa;** carga de RAM independente pode
   passar para depois dela (a descarga só lê a SQ).
7. **Blocos com as mesmas ops** (8C101BFE+8C101C26 ≡ 8C101C20; 8C101C38 =
   `fldi0` + 8C101C3A) compartilham código; os ciclos diferentes ficam antes
   da junção.
8. **Leitura rara de registrador só lido** (r8) direto do contexto.
9. **`pref` de RAM:** checagem de SQ em linha, caminho da SQ fora da linha
   (salva só o que está vivo em caller-saved).

Limites: cache quente e laço isolado (sem o ganho de L1I); o stub da SQ do
harness é barato (no jogo a descarga chama o TA de verdade, o mesmo custo nos
dois lados, o que dilui o ganho); as escritas na SQ vão direto para a memória
nos dois lados (no emulador o JIT atual usa trampolim).

