// FC_CTRL_PORT=<porta> (diagnostico/brincadeira, 2026-10-02): servidor TCP
// dentro do core para controlar o jogo de fora -- pegar a tela e apertar
// botoes do controle 1 -- sem ninguem segurando o device. Protocolo de texto,
// uma linha por comando (cliente: tools/fc_ctrl.py):
//
//   shot              -> "PPM <bytes>\n" + P6 (RGB, de cima para baixo)
//   press TECLAS N    -> segura TECLAS por N leituras do controle (~N quadros)
//   hold TECLAS       -> segura ate o proximo release
//   release           -> solta tudo
//   stick X Y         -> analogico esquerdo, -128..127 (0 0 = centro)
//   wait N            -> responde depois de N leituras do controle
//   status            -> leitura atual e teclas seguras
//
// TECLAS: A B X Y S(start) U D L R, juntas (ex.: UR, A). O controle de verdade
// continua valendo; o socket so soma botoes. Sem a variavel nada disto liga.
#include "types.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {

enum : u32
{
	K_C = 1, K_B = 1 << 1, K_A = 1 << 2, K_START = 1 << 3,
	K_UP = 1 << 4, K_DOWN = 1 << 5, K_LEFT = 1 << 6, K_RIGHT = 1 << 7,
	K_Y = 1 << 9, K_X = 1 << 10,
};

std::atomic<bool> active{false};
std::atomic<u32> pollCount{0};
std::atomic<u32> holdMask{0};
std::atomic<u32> holdUntil{0};		// 0 = sem prazo (hold)
std::atomic<int> stickX{0}, stickY{0};

std::mutex shotMx;
std::condition_variable shotCv;
bool shotWanted = false;
bool shotReady = false;
std::vector<u8> shotPpm;

u32 parseKeys(const char *s)
{
	u32 m = 0;
	for (; *s && *s != ' ' && *s != '\n' && *s != '\r'; s++)
		switch (*s)
		{
		case 'A': case 'a': m |= K_A; break;
		case 'B': case 'b': m |= K_B; break;
		case 'X': case 'x': m |= K_X; break;
		case 'Y': case 'y': m |= K_Y; break;
		case 'S': case 's': m |= K_START; break;
		case 'U': case 'u': m |= K_UP; break;
		case 'D': case 'd': m |= K_DOWN; break;
		case 'L': case 'l': m |= K_LEFT; break;
		case 'R': case 'r': m |= K_RIGHT; break;
		}
	return m;
}

bool sendAll(int fd, const void *p, size_t n)
{
	const u8 *b = (const u8 *)p;
	while (n > 0)
	{
		ssize_t w = send(fd, b, n, MSG_NOSIGNAL);
		if (w <= 0)
			return false;
		b += w;
		n -= (size_t)w;
	}
	return true;
}

bool reply(int fd, const std::string &s)
{
	return sendAll(fd, s.data(), s.size());
}

bool handle(int fd, const char *line)
{
	char cmd[16] = {0}, a1[32] = {0};
	int n1 = 0, n2 = 0;
	sscanf(line, "%15s", cmd);
	if (!strcmp(cmd, "shot"))
	{
		std::unique_lock<std::mutex> l(shotMx);
		shotReady = false;
		shotWanted = true;
		if (!shotCv.wait_for(l, std::chrono::seconds(3), [] { return shotReady; }))
		{
			shotWanted = false;
			return reply(fd, "ERR sem quadro em 3 s\n");
		}
		std::vector<u8> img;
		img.swap(shotPpm);
		l.unlock();
		char h[32];
		snprintf(h, sizeof(h), "PPM %zu\n", img.size());
		return reply(fd, h) && sendAll(fd, img.data(), img.size());
	}
	if (!strcmp(cmd, "press"))
	{
		if (sscanf(line, "%*s %31s %d", a1, &n1) < 2 || n1 <= 0)
			return reply(fd, "ERR press TECLAS N\n");
		holdMask = parseKeys(a1);
		holdUntil = pollCount + (u32)n1;
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "hold"))
	{
		if (sscanf(line, "%*s %31s", a1) < 1)
			return reply(fd, "ERR hold TECLAS\n");
		holdMask = parseKeys(a1);
		holdUntil = 0;
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "release"))
	{
		holdMask = 0;
		holdUntil = 0;
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "stick"))
	{
		if (sscanf(line, "%*s %d %d", &n1, &n2) < 2)
			return reply(fd, "ERR stick X Y\n");
		stickX = std::max(-128, std::min(127, n1));
		stickY = std::max(-128, std::min(127, n2));
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "wait"))
	{
		if (sscanf(line, "%*s %d", &n1) < 1 || n1 < 0)
			return reply(fd, "ERR wait N\n");
		u32 target = pollCount + (u32)n1;
		auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
		while ((s32)(pollCount - target) < 0 && std::chrono::steady_clock::now() < deadline)
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "status"))
	{
		char s[96];
		snprintf(s, sizeof(s), "poll %u teclas %03x ate %u stick %d %d\n", (u32)pollCount, (u32)holdMask,
				(u32)holdUntil, (int)stickX, (int)stickY);
		return reply(fd, s);
	}
	return reply(fd, "ERR comandos: shot press hold release stick wait status\n");
}

void serve(int port)
{
	int ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls < 0)
		return;
	int one = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	sockaddr_in a{};
	a.sin_family = AF_INET;
	a.sin_addr.s_addr = htonl(INADDR_ANY);
	a.sin_port = htons((u16)port);
	if (bind(ls, (sockaddr *)&a, sizeof(a)) < 0 || listen(ls, 2) < 0)
	{
		fprintf(stderr, "ctrl socket: porta %d indisponivel\n", port);
		close(ls);
		return;
	}
	fprintf(stderr, "ctrl socket: ouvindo na porta %d\n", port);
	for (;;)
	{
		int fd = accept(ls, nullptr, nullptr);
		if (fd < 0)
			continue;
		std::string buf;
		char tmp[256];
		bool ok = true;
		while (ok)
		{
			ssize_t r = recv(fd, tmp, sizeof(tmp), 0);
			if (r <= 0)
				break;
			buf.append(tmp, (size_t)r);
			size_t nl;
			while (ok && (nl = buf.find('\n')) != std::string::npos)
			{
				std::string line = buf.substr(0, nl);
				buf.erase(0, nl + 1);
				if (!line.empty())
					ok = handle(fd, line.c_str());
			}
		}
		close(fd);
	}
}

} // namespace

void ctrl_socket_init()
{
	static bool started = false;
	if (started)
		return;
	started = true;
	const char *p = getenv("FC_CTRL_PORT");
	if (p == nullptr || atoi(p) <= 0)
		return;
	active = true;
	std::thread(serve, atoi(p)).detach();
}

// Chamado depois da leitura do controle (UpdateInputState): soma os botoes do
// socket ao controle 1. kcode e ativo-baixo.
void ctrl_socket_apply(u32 port, u32 &kcode, s8 &joyx, s8 &joyy)
{
	if (!active || port != 0)
		return;
	u32 n = ++pollCount;
	u32 until = holdUntil, m = holdMask;
	if (m != 0 && until != 0 && (s32)(n - until) >= 0)
	{
		holdMask = 0;
		holdUntil = 0;
		m = 0;
	}
	kcode &= ~m;
	if (stickX != 0 || stickY != 0)
	{
		joyx = (s8)stickX;
		joyy = (s8)stickY;
	}
}

// Render thread, quadro pronto no framebuffer atual: o socket quer a tela?
bool ctrl_socket_shot_wanted()
{
	if (!active)
		return false;
	std::lock_guard<std::mutex> l(shotMx);
	return shotWanted;
}

// px = RGBA de baixo para cima (glReadPixels)
void ctrl_socket_shot_deliver(const u8 *px, int w, int h)
{
	std::vector<u8> img;
	char hdr[32];
	int hl = snprintf(hdr, sizeof(hdr), "P6\n%d %d\n255\n", w, h);
	img.reserve(hl + (size_t)w * h * 3);
	img.insert(img.end(), hdr, hdr + hl);
	for (int y = h - 1; y >= 0; y--)
	{
		const u8 *row = px + (size_t)y * w * 4;
		for (int x = 0; x < w; x++)
		{
			img.push_back(row[x * 4]);
			img.push_back(row[x * 4 + 1]);
			img.push_back(row[x * 4 + 2]);
		}
	}
	std::lock_guard<std::mutex> l(shotMx);
	shotPpm.swap(img);
	shotWanted = false;
	shotReady = true;
	shotCv.notify_all();
}
