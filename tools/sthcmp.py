import sys
# compara o estado por ciclo emulado (2o campo), ignorando o numero do frame e o
# 3o campo (ciclos desde a marca anterior, que depende de onde a rodada comecou)
def load(p):
    d={}
    for l in open(p):
        if l.startswith('L'): d={}; continue
        if l.startswith('F '):
            x=l.split()
            if len(x) >= 12: d[int(x[2])]=' '.join(x[4:])
    return d
a=load(sys.argv[1]); b=load(sys.argv[2]); com=sorted(set(a)&set(b))
dif=[c for c in com if a[c]!=b[c]]
print('frames comuns %d, diferentes %d%s'%(len(com),len(dif),(' primeiro ciclo '+str(dif[0])+(' (ultimo)' if dif[0]==com[-1] else '')) if dif else ''))
