# Gera cur.S: corpo ARM64 real de cada bloco (do dump do JIT atual), com as
# saidas reescritas como o JIT deixa depois de ligar os blocos (sem os
# contadores do FC_JIT_DUMP nem os stubs de ligacao).
# Uso: gen_cur.py [blocks.json] [bloco de entrada] [saida.S]
import json
import sys
arg = sys.argv[1:] + [None] * 3
B = json.load(open(arg[0] or 'blocks.json'))
ENTRY = arg[1] or '8C1D8BDA'
inset = set(B)
out = ['\t.text', '\t.align 4', '\t.global cur_run', '\t.global cur_blocks_begin', '\t.global cur_blocks_end']
# cur_run(ctx, cycles): salva callee-saved, x28=ctx, w27=cycles, entra no bloco
out += ['cur_run:',
        '\tstp x29, x30, [sp, #-112]!', '\tstp x19, x20, [sp, #16]', '\tstp x21, x22, [sp, #32]',
        '\tstp x23, x24, [sp, #48]', '\tstp x25, x26, [sp, #64]', '\tstp x27, x28, [sp, #80]',
        '\tstp d8, d9, [sp, #96]',
        '\tsub sp, sp, #64', '\tstp d10, d11, [sp]', '\tstp d12, d13, [sp, #16]', '\tstp d14, d15, [sp, #32]',
        '\tmov x28, x0', '\tmov w27, w1', '\tb blk_' + ENTRY,
        'cur_exit:',
        '\tadrp x9, g_cycles', '\tstr w27, [x9, :lo12:g_cycles]',
        '\tldp d10, d11, [sp]', '\tldp d12, d13, [sp, #16]', '\tldp d14, d15, [sp, #32]', '\tadd sp, sp, #64',
        '\tldp d8, d9, [sp, #96]', '\tldp x27, x28, [sp, #80]', '\tldp x25, x26, [sp, #64]', '\tldp x23, x24, [sp, #48]',
        '\tldp x21, x22, [sp, #32]', '\tldp x19, x20, [sp, #16]', '\tldp x29, x30, [sp], #112', '\tret',
        '\t.align 6', 'cur_blocks_begin:']
def tgt(va):
    return 'blk_' + va if va in inset else 'out_' + va
outs = set()
for va, b in B.items():
    out.append('blk_%s:' % va)
    h = b['hex']
    for i in range(0, len(h), 8):
        w = bytes.fromhex(h[i:i + 8])
        out.append('\t.inst 0x%08x' % int.from_bytes(w, 'little'))
    ex = b['exit']
    if b.get('cond'):
        br, nx = b['cond']
        ldr = ex[0].split('\t')[1]            # w11, [x28, #268]
        cmp = ex[1].split('\t')[1]            # w11, #0x1
        out += ['\tldr ' + ldr, '\tcmp ' + cmp, '\tb.ne ' + tgt(nx), '\tb ' + tgt(br)]
        for x in (br, nx):
            if x not in inset: outs.add(x)
    else:
        nx = b.get('next', '8C1D8BDA')        # Shenmue: 8C58/8C5A bra 8BDA
        out.append('\tb ' + tgt(nx))
        if nx not in inset: outs.add(nx)
out.append('cur_blocks_end:')
for x in sorted(outs):
    out += ['out_%s:' % x, '\tmov w0, #0x%s' % x[4:], '\tmovk w0, #0x%s, lsl #16' % x[:4], '\tb cur_exit']
open(arg[2] or 'cur.S', 'w').write('\n'.join(out) + '\n')
print('blocos', len(B), 'saidas para fora', sorted(outs))
