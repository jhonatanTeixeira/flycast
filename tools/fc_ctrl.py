#!/usr/bin/env python3
"""Cliente do socket de controle do core (FC_CTRL_PORT, core/libretro/ctrl_socket.cpp).

  fc_ctrl.py [--host 192.168.0.14] [--port 5555] shot tela.png
  fc_ctrl.py do "LS(up,35):2s; A; wait:1s; A+B*3/100ms"   sequencia (docs/ctrl_socket.md)
  fc_ctrl.py mode step              pausa; cada "do" anda e pausa de novo
  fc_ctrl.py mode live
  fc_ctrl.py run "mode step | do A | shot a.png | do DOWN; A | shot b.png"
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

    def cmd(self, line):
        self.s.sendall((line.strip() + '\n').encode())
        return self.f.readline().decode().rstrip('\n')

    def shot(self, path):
        self.s.sendall(b'shot\n')
        head = self.f.readline().decode().split()
        if len(head) != 2 or head[0] != 'PPM':
            return ' '.join(head)
        data = self.f.read(int(head[1]))
        try:
            import io
            from PIL import Image
            img = Image.open(io.BytesIO(data))
            if not path.lower().endswith('.ppm'):
                img.save(path)
                return 'ok %s %dx%d' % (path, img.width, img.height)
        except ImportError:
            if not path.lower().endswith('.ppm'):
                path = path.rsplit('.', 1)[0] + '.ppm'
        open(path, 'wb').write(data)
        return 'ok %s' % path

    def do(self, line):
        line = line.strip()
        if not line:
            return ''
        p = line.split(None, 1)
        if p[0] == 'shot':
            return self.shot(p[1].strip() if len(p) > 1 else 'tela.png')
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
