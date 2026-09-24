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
