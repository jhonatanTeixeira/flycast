#!/usr/bin/env python3
"""Conjunto quente do JIT por jogo (docs/jit_hot_path.md): blocos que somam 50%/90% do tempo
amostrado em blocos, bytes, fração fria/literais/checagem, linhas de 64 B e páginas de 4 KB.

  jit_hotset.py <dir_com_relatorios.rep> <pasta_de_sessao...>

O .rep de cada sessão é a saída de jit_lite_report.py --top 1000000 (o sdk_find.py guarda
em SDK_FIND_CACHE com o nome <pasta>.rep)."""
import sys, re, glob, os
def rep(path):
    r={}
    for l in open(path):
        m=re.match(r'\s+([0-9A-F]{8})\s+([\d.]+)%',l)
        if m: r[int(m.group(1),16)]=r.get(int(m.group(1),16),0)+float(m.group(2))
    return r
def load(jit):
    blocks={}; cur=None
    for l in open(jit,errors='ignore'):
        if l.startswith('B '):
            p=l.split(); cur=dict(host=int(p[1],16), pc=int(p[2],16), size=int(p[7]), ro='ro=1' in l, E=None, H=None)
        elif l.startswith('E ') and cur: cur['E']=tuple(int(x) for x in l.split()[1:4])
        elif l.startswith('H ') and cur:
            cur['H']=bytes.fromhex(l.split()[1]); blocks[cur['pc']]=cur; cur=None
    return blocks
def words(h): return [int.from_bytes(h[i:i+4],'little') for i in range(0,len(h)-3,4)]
def analyse(b):
    h=b['H']; w=words(h); n=len(w)
    # literal pool: alvos de ldr literal
    lit=[]
    for i,x in enumerate(w):
        if (x & 0x3b000000)==0x18000000:   # LDR (literal) w/x/prfm-lit-ish
            imm=(x>>5)&0x7ffff
            if imm & 0x40000: imm-=0x80000
            t=i+imm
            if 0<=t<n and t>i: lit.append(t)
    litstart = min(lit) if lit else n
    # o pool fica no fim: só conta se depois do último código executável conhecido
    entry_end = b['E'][0]//4 if b['E'] else 0
    # prólogo: achar subs w27
    isub=None
    for i in range(min(n,litstart)):
        if (w[i] & 0xff8003ff)==0x7100037b: isub=i; break
    check_exec=cold=0
    if isub is not None:
        # checagem antes do subs: primeira b incondicional = fim do caminho executado
        jb=None
        for i in range(isub):
            if (w[i]>>26)==0b000101: jb=i; break
        if jb is not None and isub>0:
            imm=w[jb]&0x3ffffff
            if imm & 0x2000000: imm-=0x4000000
            tgt=jb+imm
            check_exec=(jb+1)*4
            cold+=max(0,tgt-(jb+1))*4
        # b.pl depois do subs
        bpl=isub+1
        if bpl<n and (w[bpl]&0xff00001f)==0x54000005:
            imm=(w[bpl]>>5)&0x7ffff
            if imm & 0x40000: imm-=0x80000
            cold+=max(0,(bpl+imm)-(bpl+1))*4
    litbytes=(n-litstart)*4 if litstart<n else 0
    return dict(check=check_exec, cold=cold, lit=litbytes, total=len(h))
for d in sys.argv[2:]:
    jit=glob.glob(d+'/jit-*.txt')[0]; repf=os.path.join(sys.argv[1],os.path.basename(d)+'.rep')
    if not os.path.exists(repf): continue
    pr=rep(repf); blocks=load(jit)
    tot=sum(v for k,v in pr.items() if k in blocks)
    hot=sorted(((v,k) for k,v in pr.items() if k in blocks), reverse=True)
    for target in (0.5,0.9):
        acc=0; sel=[]
        for v,k in hot:
            if acc>=target*tot: break
            acc+=v; sel.append(k)
        a=[analyse(blocks[k]) for k in sel]
        T=sum(x['total'] for x in a); C=sum(x['check'] for x in a); K=sum(x['cold'] for x in a); L=sum(x['lit'] for x in a)
        lines=set(); pages=set()
        for k in sel:
            b=blocks[k]
            for o in range(0,b['size'],64): lines.add((b['host']+o)//64)
            lines.add((b['host']+b['size']-1)//64)
            pages.update({b['host']//4096,(b['host']+b['size']-1)//4096})
        packed=(T-K-L)
        ro0=sum(1 for k in sel if not blocks[k]['ro'])
        print(f"{os.path.basename(d)[16:40]:24} {int(target*100)}%: blocos={len(sel):4d} (ro=0: {ro0:3d}) bytes={T/1024:6.1f}K  checagem={100*C/max(1,T):4.1f}%  frio={100*K/max(1,T):4.1f}%  literais={100*L/max(1,T):4.1f}%  linhas64={len(lines):5d} ({len(lines)*64/1024:6.1f}K)  paginas4K={len(pages):4d}  so_quente={packed/1024:6.1f}K")
    print(f"  blocos JIT no total: {len(blocks)}  bytes {sum(b['size'] for b in blocks.values())/1024/1024:.1f} MB  amostras em blocos={tot:.1f}% da emu")
