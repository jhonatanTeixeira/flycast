// FC_CTRL_PORT=<porta> (diagnostico/brincadeira, 2026-10-02): servidor TCP
// dentro do core para controlar o jogo de fora -- pegar a tela e mexer no
// controle 1 -- sem ninguem segurando o device. Protocolo de texto, uma linha
// por comando (cliente: tools/fc_ctrl.py; referencia: docs/ctrl_socket.md):
//
//   do SEQUENCIA      -> executa a sequencia e responde "ok N leituras" quando
//                        termina; no modo step responde "PPM <bytes> ok N
//                        leituras\n" + a tela do resultado (jogo ja parado)
//   mode step|live    -> step: o jogo fica pausado; cada "do" despausa, executa
//                        e pausa de novo
//   shot              -> "PPM <bytes>\n" + P6 (RGB, de cima para baixo); pausado,
//                        devolve o quadro em que o jogo parou
//   set tap ON OFF    -> duracao padrao do toque (leituras apertado / solto)
//   status            -> leitura atual, modo, teclas
//   pc N              -> amostra o PC e o PR do SH4 N vezes (1 ms)
//   press/hold/release/stick/wait -> comandos antigos (ver docs)
//
// SEQUENCIA: passos separados por ';', executados em ordem. Passo =
// elementos juntados por '+' (apertados juntos) e modificadores:
//   A, B, X, Y, START (S), UP DOWN LEFT RIGHT (U D L R), LT, RT
//   LT(60) RT(30)                 gatilho com forca em %
//   LS(up,40) LS(-30,80)          analogico esquerdo: direcao + forca em %, ou X,Y em %
//   RS(...)                       analogico direito (mesma sintaxe)
//   wait                          passo vazio (so a duracao)
//   :3s :500ms :20f :20           segura por esse tempo (f = leituras do controle)
//   *5                            repete 5 vezes
//   /200ms                        tempo solto depois de cada repeticao
// Tempo em s/ms e o tempo EMULADO (relogio do SH4), nao o da parede: jogo
// lento segura mais tempo de parede, mas o mesmo tempo de jogo.
// Ex.: "do LS(up,35):2s; A; wait:1s; A+B*3/100ms; LT(100)+RIGHT:20f"
//
// Sem a variavel nada disto liga. O controle de verdade continua valendo:
// o socket soma botoes e, enquanto um passo usa o analogico, sobrepoe o eixo.
#include "types.h"
#include "hw/sh4/sh4_if.h"
#include "hw/sh4/sh4_sched.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <deque>
#include <map>
#include <memory>
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

// duracao: em leituras do controle (polls) ou em ciclos emulados do SH4
struct Dur
{
	bool cycles = false;
	u64 v = 0;
};

struct Step
{
	u32 keys = 0;
	bool ls = false, rs = false;
	int lx = 0, ly = 0, rx = 0, ry = 0;		// -127..127
	int lt = -1, rt = -1;					// 0..255, -1 = nao mexe
	Dur on, off;
	u32 repeat = 1;
};

struct Program
{
	std::vector<Step> steps;
	u64 id = 0;
};

std::atomic<bool> active{false};
std::atomic<u32> pollCount{0};
// comandos antigos (persistentes, somados por cima da sequencia)
std::atomic<u32> holdMask{0};
std::atomic<u32> holdUntil{0};
std::atomic<int> stickX{0}, stickY{0};
std::atomic<u32> tapOn{6}, tapOff{6};

// fila de sequencias: socket -> emu thread
std::mutex seqMx;
std::condition_variable seqCv;		// emu thread espera aqui quando pausada
std::condition_variable doneCv;		// socket espera a sequencia acabar
std::deque<std::shared_ptr<Program>> pending;
u64 nextId = 1, doneId = 0;
std::atomic<bool> stepMode{false};
std::atomic<bool> frozen{false};

// estado da execucao (so a emu thread mexe)
std::shared_ptr<Program> cur;
size_t stepIdx;
u32 rep;
bool phaseOn;
u32 phasePoll;
u64 phaseCyc;

std::mutex shotMx;
std::condition_variable shotCv;
bool shotWanted = false;
bool shotReady = false;
std::vector<u8> shotPpm;
std::vector<u8> lastPpm;			// ultimo quadro capturado (devolvido quando pausado)
bool lastValid = false;
bool freezeFrame = false;		// quadro capturado desde a ultima parada

std::string lower(std::string s)
{
	for (char &c : s)
		c = (char)tolower((unsigned char)c);
	return s;
}

std::string trim(const std::string &s)
{
	size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
	return a == std::string::npos ? "" : s.substr(a, b - a + 1);
}

bool parseDur(const std::string &t, Dur &d)
{
	char *end;
	double v = strtod(t.c_str(), &end);
	if (end == t.c_str() || v < 0)
		return false;
	std::string u = lower(end);
	if (u == "" || u == "f")
	{
		d.cycles = false;
		d.v = (u64)v;
	}
	else if (u == "s" || u == "ms")
	{
		d.cycles = true;
		d.v = (u64)(v * (u == "s" ? 1.0 : 0.001) * SH4_MAIN_CLOCK);
	}
	else
		return false;
	return true;
}

int pct(double p, int full)
{
	p = std::max(-100.0, std::min(100.0, p));
	return (int)lround(p * full / 100.0);
}

// "up,40" / "-30,80" -> eixo em -127..127 (y negativo = cima, como no libretro)
bool parseStick(const std::string &args, int &x, int &y)
{
	size_t c = args.find(',');
	std::string a = lower(trim(args.substr(0, c)));
	double force = 100;
	if (c != std::string::npos)
		force = atof(args.substr(c + 1).c_str());
	static const std::map<std::string, std::pair<double, double>> dirs = {
		{ "up", { 0, -1 } }, { "down", { 0, 1 } }, { "left", { -1, 0 } }, { "right", { 1, 0 } },
		{ "upleft", { -1, -1 } }, { "ul", { -1, -1 } }, { "upright", { 1, -1 } }, { "ur", { 1, -1 } },
		{ "downleft", { -1, 1 } }, { "dl", { -1, 1 } }, { "downright", { 1, 1 } }, { "dr", { 1, 1 } },
		{ "center", { 0, 0 } }, { "c", { 0, 0 } },
	};
	auto it = dirs.find(a);
	if (it != dirs.end())
	{
		double dx = it->second.first, dy = it->second.second;
		double n = std::sqrt(dx * dx + dy * dy);
		if (n > 0)
		{
			dx /= n;
			dy /= n;
		}
		x = pct(dx * force, 127);
		y = pct(dy * force, 127);
		return true;
	}
	char *end;
	double vx = strtod(a.c_str(), &end);
	if (end == a.c_str() || c == std::string::npos)
		return false;
	x = pct(vx, 127);
	y = pct(force, 127);
	return true;
}

bool parseElem(const std::string &e0, Step &s, std::string &err)
{
	std::string e = trim(e0), le = lower(e);
	size_t p = le.find('(');
	std::string name = p == std::string::npos ? le : le.substr(0, p);
	std::string args;
	if (p != std::string::npos)
	{
		size_t q = le.rfind(')');
		if (q == std::string::npos || q < p)
		{
			err = "parentese aberto em '" + e + "'";
			return false;
		}
		args = e.substr(p + 1, q - p - 1);
	}
	if (name == "ls" || name == "rs")
	{
		int x, y;
		if (!parseStick(args, x, y))
		{
			err = "analogico invalido em '" + e + "' (use LS(up,40) ou LS(-30,80))";
			return false;
		}
		if (name == "ls") { s.ls = true; s.lx = x; s.ly = y; }
		else { s.rs = true; s.rx = x; s.ry = y; }
		return true;
	}
	if (name == "lt" || name == "rt")
	{
		int v = pct(args.empty() ? 100 : atof(args.c_str()), 255);
		v = std::max(0, v);
		(name == "lt" ? s.lt : s.rt) = v;
		return true;
	}
	if (name == "wait" || name == "~" || name.empty())
		return true;
	static const std::map<std::string, u32> names = {
		{ "a", K_A }, { "b", K_B }, { "x", K_X }, { "y", K_Y }, { "start", K_START }, { "s", K_START },
		{ "up", K_UP }, { "down", K_DOWN }, { "left", K_LEFT }, { "right", K_RIGHT },
		{ "u", K_UP }, { "d", K_DOWN }, { "l", K_LEFT }, { "r", K_RIGHT },
	};
	auto it = names.find(name);
	if (it != names.end())
	{
		s.keys |= it->second;
		return true;
	}
	// forma antiga colada: "UR", "AB"
	u32 m = 0;
	for (char c : name)
	{
		auto k = names.find(std::string(1, c));
		if (k == names.end())
		{
			err = "botao desconhecido '" + e + "'";
			return false;
		}
		m |= k->second;
	}
	s.keys |= m;
	return true;
}

// passo: elementos + modificadores (:dur, *n, /dur) em qualquer ordem no fim
bool parseStep(const std::string &t, Step &s, std::string &err)
{
	std::string body;
	std::vector<std::pair<char, std::string>> mods;
	int depth = 0;
	char curMod = 0;
	std::string acc;
	for (char c : t)
	{
		if (c == '(') depth++;
		if (c == ')') depth--;
		if (depth == 0 && (c == ':' || c == '*' || c == '/'))
		{
			if (curMod) mods.push_back({ curMod, acc });
			else body = acc;
			curMod = c;
			acc.clear();
			continue;
		}
		acc += c;
	}
	if (curMod) mods.push_back({ curMod, acc });
	else body = acc;

	s = Step();
	s.on.v = tapOn;
	s.off.v = tapOff;
	bool isWait = lower(trim(body)) == "wait" || trim(body) == "~";
	if (isWait)
		s.off.v = 0;
	size_t a = 0;
	std::string b = body;
	while (a <= b.size())
	{
		int d = 0;
		size_t e = a;
		while (e < b.size() && !(d == 0 && b[e] == '+'))
		{
			if (b[e] == '(') d++;
			if (b[e] == ')') d--;
			e++;
		}
		if (!parseElem(b.substr(a, e - a), s, err))
			return false;
		a = e + 1;
	}
	for (auto &m : mods)
	{
		std::string v = trim(m.second);
		if (m.first == '*')
		{
			int n = atoi(v.c_str());
			if (n <= 0 || n > 1000)
			{
				err = "repeticao invalida '*" + v + "'";
				return false;
			}
			s.repeat = (u32)n;
		}
		else if (!parseDur(v, m.first == ':' ? s.on : s.off))
		{
			err = std::string("duracao invalida '") + m.first + v + "' (use 3s, 500ms, 20f)";
			return false;
		}
	}
	return true;
}

bool parseProgram(const std::string &text, Program &p, std::string &err)
{
	size_t a = 0;
	while (a <= text.size())
	{
		size_t e = text.find(';', a);
		if (e == std::string::npos)
			e = text.size();
		std::string t = trim(text.substr(a, e - a));
		if (!t.empty())
		{
			Step s;
			if (!parseStep(t, s, err))
				return false;
			p.steps.push_back(s);
		}
		a = e + 1;
	}
	if (p.steps.empty())
	{
		err = "sequencia vazia";
		return false;
	}
	return true;
}

bool elapsed(const Dur &d)
{
	if (d.cycles)
		return sh4_sched_now64() - phaseCyc >= d.v;
	return pollCount - phasePoll >= d.v;
}

void startPhase(bool on)
{
	phaseOn = on;
	phasePoll = pollCount;
	phaseCyc = sh4_sched_now64();
}

void finishProgram()
{
	std::lock_guard<std::mutex> l(seqMx);
	doneId = cur->id;
	cur.reset();
	doneCv.notify_all();
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

bool sendPpm(int fd, const std::vector<u8> &img)
{
	char h[32];
	snprintf(h, sizeof(h), "PPM %zu\n", img.size());
	return reply(fd, h) && sendAll(fd, img.data(), img.size());
}

u32 parseKeysOld(const char *s)
{
	Step st;
	std::string err;
	std::string t = s;
	t = t.substr(0, t.find_first_of(" \r\n"));
	return parseElem(t, st, err) ? st.keys : 0;
}

bool handle(int fd, const char *line)
{
	char cmd[16] = {0}, a1[32] = {0};
	int n1 = 0, n2 = 0;
	sscanf(line, "%15s", cmd);
	if (!strcmp(cmd, "do"))
	{
		const char *rest = line + 2;
		auto p = std::make_shared<Program>();
		std::string err;
		if (!parseProgram(rest, *p, err))
			return reply(fd, "ERR " + err + "\n");
		u64 id;
		{
			std::lock_guard<std::mutex> l(seqMx);
			id = p->id = nextId++;
			pending.push_back(p);
			seqCv.notify_all();
		}
		u32 start = pollCount;
		std::unique_lock<std::mutex> l(seqMx);
		// no modo step responde so depois de parar (a "shot" seguinte ve o quadro parado)
		if (!doneCv.wait_for(l, std::chrono::seconds(300),
				[id] { return doneId >= id && (frozen || !stepMode || !pending.empty()); }))
			return reply(fd, "ERR sequencia nao terminou em 300 s\n");
		u32 polls = (u32)pollCount - start;
		if (frozen)
		{
			// modo step: a resposta e a propria tela do resultado
			std::vector<u8> img;
			{
				std::lock_guard<std::mutex> sl(shotMx);
				img = lastPpm;
			}
			l.unlock();
			char h[64];
			snprintf(h, sizeof(h), "PPM %zu ok %u leituras\n", img.size(), polls);
			return reply(fd, h) && sendAll(fd, img.data(), img.size());
		}
		char s[64];
		snprintf(s, sizeof(s), "ok %u leituras\n", polls);
		return reply(fd, s);
	}
	if (!strcmp(cmd, "mode"))
	{
		sscanf(line, "%*s %31s", a1);
		if (!strcmp(a1, "step"))
		{
			{
				std::lock_guard<std::mutex> l(shotMx);
				freezeFrame = false;
			}
			stepMode = true;
		}
		else if (!strcmp(a1, "live"))
		{
			stepMode = false;
			std::lock_guard<std::mutex> l(seqMx);
			seqCv.notify_all();
		}
		else
			return reply(fd, "ERR mode step|live\n");
		if (stepMode)
		{
			// espera parar para a proxima "shot" ja ver o quadro parado
			for (int i = 0; i < 300 && !frozen; i++)
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		return reply(fd, std::string("ok ") + (stepMode ? (frozen ? "step (pausado)" : "step") : "live") + "\n");
	}
	if (!strcmp(cmd, "shot"))
	{
		std::unique_lock<std::mutex> l(shotMx);
		if (frozen && lastValid)
		{
			std::vector<u8> img = lastPpm;
			l.unlock();
			return sendPpm(fd, img);
		}
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
		return sendPpm(fd, img);
	}
	if (!strcmp(cmd, "set"))
	{
		if (sscanf(line, "%*s %31s %d %d", a1, &n1, &n2) == 3 && !strcmp(a1, "tap") && n1 > 0 && n2 >= 0)
		{
			tapOn = (u32)n1;
			tapOff = (u32)n2;
			return reply(fd, "ok\n");
		}
		return reply(fd, "ERR set tap ON OFF\n");
	}
	if (!strcmp(cmd, "press"))
	{
		if (sscanf(line, "%*s %31s %d", a1, &n1) < 2 || n1 <= 0)
			return reply(fd, "ERR press TECLAS N\n");
		holdMask = parseKeysOld(a1);
		holdUntil = pollCount + (u32)n1;
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "hold"))
	{
		if (sscanf(line, "%*s %31s", a1) < 1)
			return reply(fd, "ERR hold TECLAS\n");
		holdMask = parseKeysOld(a1);
		holdUntil = 0;
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "release"))
	{
		holdMask = 0;
		holdUntil = 0;
		stickX = stickY = 0;
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
		if (frozen)
			return reply(fd, "ERR pausado (mode live ou do wait:N)\n");
		u32 target = pollCount + (u32)n1;
		auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(60);
		while ((s32)(pollCount - target) < 0 && std::chrono::steady_clock::now() < deadline)
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		return reply(fd, "ok\n");
	}
	if (!strcmp(cmd, "status"))
	{
		char s[160];
		snprintf(s, sizeof(s), "poll %u modo %s%s teclas %03x stick %d %d tap %u/%u fila %zu\n", (u32)pollCount,
				stepMode ? "step" : "live", frozen ? " (pausado)" : "", (u32)holdMask, (int)stickX, (int)stickY,
				(u32)tapOn, (u32)tapOff, pending.size() + (cur ? 1 : 0));
		return reply(fd, s);
	}
	if (!strcmp(cmd, "pc"))
	{
		if (sscanf(line, "%*s %d", &n1) < 1 || n1 <= 0)
			n1 = 500;
		std::map<u32, u32> pcs, prs;
		for (int i = 0; i < n1; i++)
		{
			pcs[p_sh4rcb->cntx.pc]++;
			prs[p_sh4rcb->cntx.pr]++;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		std::string out;
		for (auto *m : { &pcs, &prs })
		{
			std::vector<std::pair<u32, u32>> v;
			for (auto &kv : *m)
				v.push_back({ kv.second, kv.first });
			std::sort(v.rbegin(), v.rend());
			out += m == &pcs ? "pc:" : " | pr:";
			for (size_t i = 0; i < v.size() && i < 8; i++)
			{
				char b[32];
				snprintf(b, sizeof(b), " %08X=%u", v[i].second, v[i].first);
				out += b;
			}
		}
		return reply(fd, out + "\n");
	}
	return reply(fd, "ERR comandos: do mode shot set status pc press hold release stick wait\n");
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
		char tmp[512];
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
				if (!trim(line).empty())
					ok = handle(fd, line.c_str());
			}
		}
		close(fd);
	}
}

// Modo step: pede um quadro ao render e, quando ele chega, para a emu thread
// ate chegar a proxima sequencia (ou "mode live"). Devolve com a fila nao vazia.
void freezeIfIdle()
{
	if (!stepMode)
		return;
	{
		std::lock_guard<std::mutex> l(seqMx);
		if (!pending.empty())
			return;
	}
	{
		std::lock_guard<std::mutex> l(shotMx);
		if (!freezeFrame)
		{
			// ainda sem o quadro desta parada: deixa o jogo andar ate o render
			// entregar (1-3 quadros)
			shotWanted = true;
			return;
		}
	}
	std::unique_lock<std::mutex> l(seqMx);
	frozen = true;
	doneCv.notify_all();
	while (stepMode && pending.empty())
		seqCv.wait_for(l, std::chrono::milliseconds(200));
	frozen = false;
	l.unlock();
	std::lock_guard<std::mutex> sl(shotMx);
	freezeFrame = false;		// a proxima parada pede quadro novo
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

// Chamado depois da leitura do controle (UpdateInputState), na emu thread:
// executa a sequencia corrente e soma o resultado ao controle 1 (kcode e
// ativo-baixo). No modo step, para a emu thread aqui entre sequencias.
void ctrl_socket_apply(u32 port, u32 &kcode, s8 &joyx, s8 &joyy, s8 &joyrx, s8 &joyry, u8 &lt, u8 &rt)
{
	if (!active || port != 0)
		return;
	u32 n = ++pollCount;

	if (!cur)
	{
		bool waitDone = false;
		{
			std::lock_guard<std::mutex> l(seqMx);
			if (!pending.empty())
			{
				cur = pending.front();
				pending.pop_front();
				stepIdx = 0;
				rep = 0;
				startPhase(true);
			}
			else
				waitDone = true;
		}
		if (waitDone)
		{
			freezeIfIdle();
			std::lock_guard<std::mutex> l(seqMx);
			if (!cur && !pending.empty())
			{
				cur = pending.front();
				pending.pop_front();
				stepIdx = 0;
				rep = 0;
				startPhase(true);
			}
		}
	}

	u32 m = 0;
	if (cur)
	{
		// avanca as fases vencidas (um passo de duracao 0 nao trava a fila)
		for (int guard = 0; guard < 64 && cur; guard++)
		{
			const Step &s = cur->steps[stepIdx];
			if (phaseOn)
			{
				if (!elapsed(s.on))
					break;
				startPhase(false);
				continue;
			}
			if (!elapsed(s.off))
				break;
			if (++rep < s.repeat)
			{
				startPhase(true);
				continue;
			}
			rep = 0;
			if (++stepIdx >= cur->steps.size())
			{
				finishProgram();
				break;
			}
			startPhase(true);
		}
		if (cur && phaseOn)
		{
			const Step &s = cur->steps[stepIdx];
			m |= s.keys;
			if (s.ls) { joyx = (s8)s.lx; joyy = (s8)s.ly; }
			if (s.rs) { joyrx = (s8)s.rx; joyry = (s8)s.ry; }
			if (s.lt >= 0) lt = (u8)s.lt;
			if (s.rt >= 0) rt = (u8)s.rt;
		}
	}

	// comandos antigos por cima
	u32 until = holdUntil, hm = holdMask;
	if (hm != 0 && until != 0 && (s32)(n - until) >= 0)
	{
		holdMask = 0;
		holdUntil = 0;
		hm = 0;
	}
	m |= hm;
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
	lastPpm = img;
	lastValid = true;
	freezeFrame = true;
	shotPpm.swap(img);
	shotWanted = false;
	shotReady = true;
	shotCv.notify_all();
}
