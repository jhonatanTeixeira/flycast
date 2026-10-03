#!/usr/bin/env python3
"""Cliente do socket de controle do core (FC_CTRL_PORT, core/libretro/ctrl_socket.cpp).

  fc_ctrl.py [--host 192.168.0.14] [--port 5555] shot tela.png
  fc_ctrl.py do "LS(up,35):2s; A; wait:1s; A+B*3/100ms"   sequencia (docs/ctrl_socket.md)
  fc_ctrl.py mode step              pausa; cada "do" anda, pausa e devolve a tela
  fc_ctrl.py do "A; DOWN > menu.png"   no modo step salva a tela do resultado
                                       (sem "> arquivo": passo_001.png, passo_002.png...)
  fc_ctrl.py mode live
  fc_ctrl.py run "mode step | do A > a.png | do DOWN; A > b.png | mode live"
  fc_ctrl.py mem 8C200000 20000 > sofdec.bin   le a RAM do jogo (hex)
  fc_ctrl.py                        modo interativo (um comando por linha)

"run" separa comandos com '|' (o ';' e da sequencia do "do").
"shot" salva PNG se o Pillow existir, senao PPM.
"""
import argparse
import socket
import sys


class Ctrl:
    def __init__(self, host, port):
        self.s = socket.create_connection((host, port), timeout=70)
        self.f = self.s.makefile('rb')
        self.step = 0

    def cmd(self, line):
        self.s.sendall((line.strip() + '\n').encode())
        return self.f.readline().decode().rstrip('\n')

    def shot(self, path, line='shot'):
        self.s.sendall((line + '\n').encode())
        head = self.f.readline().decode().split()
        if len(head) < 2 or head[0] != 'PPM':
            return ' '.join(head)
        data = self.f.read(int(head[1]))
        extra = (' '.join(head[2:]) + ' ') if len(head) > 2 else ''
        try:
            import io
            from PIL import Image
            img = Image.open(io.BytesIO(data))
            if not path.lower().endswith('.ppm'):
                img.save(path)
                return '%s-> %s %dx%d' % (extra, path, img.width, img.height)
        except ImportError:
            if not path.lower().endswith('.ppm'):
                path = path.rsplit('.', 1)[0] + '.ppm'
        open(path, 'wb').write(data)
        return '%s-> %s' % (extra, path)

    def do(self, line):
        line = line.strip()
        if not line:
            return ''
        p = line.split(None, 1)
        if p[0] == 'shot':
            return self.shot(p[1].strip() if len(p) > 1 else 'tela.png')
        if p[0] == 'mem':
            # "mem ADDR LEN > arquivo.bin"
            req, _, path = line.partition('>')
            self.s.sendall((req.strip() + '\n').encode())
            head = self.f.readline().decode().split()
            if len(head) != 2 or head[0] != 'BIN':
                return ' '.join(head)
            data = self.f.read(int(head[1]))
            path = path.strip() or 'mem_%s.bin' % req.split()[1]
            open(path, 'wb').write(data)
            return 'ok %s (%d bytes)' % (path, len(data))
        if p[0] == 'do':
            # "do SEQ > arquivo.png": no modo step a resposta traz a tela
            seq, _, path = line.partition('>')
            self.step += 1
            path = path.strip() or 'passo_%03d.png' % self.step
            return self.shot(path, seq.strip())
        return self.cmd(line)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--host', default='192.168.0.14')
    ap.add_argument('--port', type=int, default=5555)
    ap.add_argument('args', nargs=argparse.REMAINDER)
    a = ap.parse_args()
    c = Ctrl(a.host, a.port)
    if not a.args:
        for line in sys.stdin:
            print(c.do(line), flush=True)
        return
    if a.args[0] == 'run':
        for part in ' '.join(a.args[1:]).split('|'):
            if part.strip():
                print('%s -> %s' % (part.strip(), c.do(part)), flush=True)
        return
    print(c.do(' '.join(a.args)))


if __name__ == '__main__':
    main()
