import sys,re,glob
for d in sys.argv[1:]:
    st={}
    for l in open(d+'/stat.txt'):
        m=re.match(r'\s*(\S+)-(\d+)\s+([\d,.]+)\s+(?:msec\s+)?(\S+)',l)
        if m and m.group(3).replace(',','').replace('.','').isdigit():
            st.setdefault(m.group(2),{'name':m.group(1)})[m.group(4)]=float(m.group(3).replace(',',''))
    emu=max(st.values(), key=lambda x:x.get('instructions',0))
    ss=open(glob.glob(d+'/sync-stats-*.txt')[0]).read()
    vel=float(re.search(r'vel_sh4_cycles\s+([\d.]+)',ss).group(1)); nf=re.search(r'new_fps\s+([\d.]+)',ss).group(1)
    q=re.search(r'queued\s+(\d+)\s+dropped\s+(\d+)',ss).groups()
    fr=[l.split() for l in open(d+'/freq.txt')]; mhz=sum(int(x[0]) for x in fr)/len(fr)/1000; gpu=sum(int(x[2]) for x in fr)/len(fr)
    emusec=12*vel/100
    i=emu['instructions']; c=emu['cycles']
    print('%-8s VEL %5.1f  fps novos %5s  enfil/desc %s/%s  CPU %4.0f MHz GPU %2.0f%% | emu: tempo %5.0f ms, instr/s-emu %5.0fM, ciclos/s-emu %5.0fM, IPC %.3f, L2 refill/s-emu %5.1fM, L1I refill/s-emu %5.1fM, iTLB refill/s-emu %5.2fM, faults %d, ctxsw %d'%(
      d.split('rtpmu_')[-1],vel,nf,q[0],q[1],mhz,gpu,emu.get('task-clock',0),i/emusec/1e6,c/emusec/1e6,i/c,emu.get('r17',0)/emusec/1e6,emu.get('r01',0)/emusec/1e6,emu.get('r02',0)/emusec/1e6,emu.get('page-faults',0),emu.get('context-switches',0)))
