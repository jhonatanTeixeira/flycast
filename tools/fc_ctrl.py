#!/usr/bin/env python3
"""Cliente do socket de controle do core (FC_CTRL_PORT, core/libretro/ctrl_socket.cpp).

  fc_ctrl.py [--host 192.168.0.14] [--port 5555] shot tela.png
  fc_ctrl.py press UR 90            segura cima+direita por 90 leituras (~1,5 s)
  fc_ctrl.py hold A | release | stick 0 -127 | wait 60 | status
  fc_ctrl.py run "press UR 90; wait 90; press A 6; wait 30; shot a.png"
  fc_ctrl.py                        modo interativo (mesmos comandos, um por linha)

Teclas: A B X Y S(start) U D L R. "shot" salva PNG se o Pillow existir, senao PPM.
"""
import argparse
import shlex
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
        p = shlex.split(line)
        if not p:
            return ''
        if p[0] == 'shot':
            return self.shot(p[1] if len(p) > 1 else 'tela.png')
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
        for part in ' '.join(a.args[1:]).split(';'):
            if part.strip():
                print('%s -> %s' % (part.strip(), c.do(part)), flush=True)
        return
    print(c.do(' '.join(shlex.quote(x) for x in a.args)))


if __name__ == '__main__':
    main()
