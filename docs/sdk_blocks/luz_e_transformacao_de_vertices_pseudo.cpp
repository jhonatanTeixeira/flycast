// Luz e transformação de vértices (`lightxf`) — pseudo-C++ para todos os jogos
// identificados (docs/sdk_blocks/luz_e_transformacao_de_vertices.md).
//
// Pseudo-código: não compila no core. É a base do `hle_fn` relocável (4.125); a versão
// bit-exata que roda hoje (só no Napple) está em core/rec-ARM64/hle_fn.cpp (lightxf_run).
//
// API usada nos *_pseudo.cpp
//   Sh4 &s            registradores: s.r[16], s.fr[16], s.xf[16] (matriz XMTRX), s.fpscr, s.sr.T, s.pr
//   ram32/ramf/ram16s leitura direta da RAM emulada (endereço & RAM_MASK), sem checagem
//   push32/pop32      pilha do SH4 (r15)
//   sq_write(a, v)    grava na Store Queue; sq_flush(a) = `pref` na área da SQ (envia ao TA)
//   Cycles &cyc       ciclos do SH4: cyc.left() = o que sobra na fatia; cyc.take(n) desconta;
//                     cyc.update() = UpdateSystem na fronteira de bloco em que o JIT faria
//   bail(pc)          devolve ao JIT na entrada do bloco `pc` (caminho não coberto)
//
// Variantes: **nenhuma**. O código é idêntico nos 5 jogos (69 de 69 opcodes comparados);
// só muda o endereço base. As constantes ficam no literal pool e são lidas da RAM.
//
//   Jogo                                   base        2ª cópia
//   Napple Tale                            8C14DDC0    8C152AEC
//   Evolution - The World of Sacred Device 8C1C9B6C
//   Evolution 2 - Far Off Promise          8C1BCF40
//   Resident Evil - Code: Veronica         8C1C00A0
//   Skies of Arcadia (disco 1)             8C2CA500    8C2CE794
//
// Entrada (r4 → struct no jogo):
//   +0  n            número de vértices
//   +4  src          vértices de 24 bytes: posição xyz, normal xyz
//   +8  ocb          linhas que o jogo invalida (`ocbi`, sem efeito no emulador)
//   +12 dst          destino na Store Queue (0xE0000000..)
//   +16 sx, sy, ox, oy   escala e deslocamento de tela
// Literal em base+0x48: ponteiro para a tabela de luzes:
//   +0  máscara (3 bits por luz: ativa, outro tipo, fim)
//   +16 luz k em +16+32k: direção xyz em +0..+8, cor rgb em +20..+28
// Saída por vértice (32 bytes na SQ, um `pref` por vértice):
//   z, sx', sy', 1/z, 1.0, r, g, b        com sx' = ox + (1/z)·(sx·x), sy' idem
// Retorno r0 = quantos vértices têm z < 1 (o jogo usa como contagem de "perto demais").

#pragma GCC optimize("fp-contract=off")   // só as fusões que o JIT faz (ftrv, fmac)
#include <arm_neon.h>

namespace lightxf {

constexpr u32 LIT_LIGHTS = 0x48;            // offset do literal "tabela de luzes"
constexpr u32 OFF_LOOP   = 0x16;            // cabeça do laço (2ª entrada do JIT)
constexpr u32 OFF_OTHER  = 0x4C;            // luz do "outro tipo": não coberta → JIT

// Luzes ativas decodificadas da máscara; montado uma vez por chamada (a máscara só
// muda entre frames). other=true → o jogo usa um tipo de luz que este código não cobre.
struct Plan { int n; u32 off[16]; bool other; };

static Plan decode_mask(u32 m)
{
	Plan p{};
	for (int k = 0; k < 16; k++, m >>= 3)
	{
		const bool active = m & 1, other = m & 2, last = m & 4;
		if (active && other) { p.other = true; return p; }
		if (active) p.off[p.n++] = 16 + 32 * k;
		if (last) return p;
	}
	return p;
}

// ftrv como o JIT: fmul da coluna 0 + três fmla fundidas
static inline float32x4_t ftrv(const float32x4_t m[4], float a, float b, float c, float d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	return vfmaq_n_f32(acc, m[3], d);
}

// fipr como o JIT: produtos sem fusão, soma (p0+p1)+(p2+p3)
static inline float fipr(float32x4_t a, float32x4_t b)
{
	const float32x4_t p = vmulq_f32(a, b);
	const float32x4_t s = vpaddq_f32(p, p);
	return vpadds_f32(vget_low_f32(s));
}

// Chamado pelo JIT ao compilar o bloco em `base` cujos bytes batem com a assinatura.
void run(Sh4 &s, Cycles &cyc, u32 base)
{
	if (s.fpscr.PR || s.fpscr.SZ)            // só precisão simples, fmov de 32 bits
		return bail(base);

	const u32 a = s.r[4];
	const u32 n = ram32(a), src = ram32(a + 4), dst = ram32(a + 12);
	const float sx = ramf(a + 16), sy = ramf(a + 20), ox = ramf(a + 24), oy = ramf(a + 28);
	const u32 lights = ram32(base + LIT_LIGHTS);
	const Plan plan = decode_mask(ram32(lights));
	if (plan.other)
		return bail(base);                   // tipo de luz não coberto: o JIT roda tudo

	// registradores salvos pelo prólogo (fr15..fr12, r8), como o jogo faz
	push32(s, f2u(s.fr[15])); push32(s, f2u(s.fr[14]));
	push32(s, f2u(s.fr[13])); push32(s, f2u(s.fr[12])); push32(s, s.r[8]);

	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };
	float32x4_t ldir[16], lcol[16];          // direções e cores das luzes ativas, em registro
	for (int k = 0; k < plan.n; k++)
	{
		const u32 l = lights + plan.off[k];
		ldir[k] = (float32x4_t){ ramf(l), ramf(l + 4), ramf(l + 8), 0.f };
		lcol[k] = (float32x4_t){ ramf(l + 20), ramf(l + 24), ramf(l + 28), 0.f };
	}
	const s32 perVertex = cost_per_vertex(plan);   // soma dos blocos do caminho (tabela do JIT)

	u32 near = 0, v = src, out = dst;
	for (u32 i = 0; i < n; i++, v += 24, out += 32)
	{
		if (cyc.left() < perVertex)          // a fatia acaba neste vértice: devolve ao JIT
		{                                    // na cabeça do laço com o estado em dia
			save_loop_state(s, n - i, v, out, near);
			return bail(base + OFF_LOOP);
		}
		const float32x4_t p = ftrv(m, ramf(v), ramf(v + 4), ramf(v + 8), 1.f);
		const float32x4_t nrm = ftrv(m, ramf(v + 12), ramf(v + 16), ramf(v + 20), 0.f);
		const float32x4_t n3 = vsetq_lane_f32(0.f, nrm, 3);

		float r = 0.f, g = 0.f, b = 0.f;
		for (int k = 0; k < plan.n; k++)     // só as luzes ativas (o SH4 varre a máscara)
		{
			const float d = fipr(n3, ldir[k]);
			if (d < 0.f)                     // luz de frente: cor × (−N·L), fmac como o JIT
			{
				r = __builtin_fmaf(-d, vgetq_lane_f32(lcol[k], 0), r);
				g = __builtin_fmaf(-d, vgetq_lane_f32(lcol[k], 1), g);
				b = __builtin_fmaf(-d, vgetq_lane_f32(lcol[k], 2), b);
			}
		}

		const float x = vgetq_lane_f32(p, 0), y = vgetq_lane_f32(p, 1), z = vgetq_lane_f32(p, 2);
		near += 1.f > z;
		const float iz = 1.f / z;
		sq_write(out + 0, f2u(z));
		sq_write(out + 4, f2u(__builtin_fmaf(iz, sx * x, ox)));
		sq_write(out + 8, f2u(__builtin_fmaf(iz, sy * y, oy)));
		sq_write(out + 12, f2u(iz));
		sq_write(out + 16, f2u(1.f));
		sq_write(out + 20, f2u(r));
		sq_write(out + 24, f2u(g));
		sq_write(out + 28, f2u(b));
		sq_flush(out);                       // `pref`: 32 bytes para o TA
		cyc.take(perVertex);
	}

	s.r[0] = near;
	s.r[8] = pop32(s);
	s.fr[12] = u2f(pop32(s)); s.fr[13] = u2f(pop32(s));
	s.fr[14] = u2f(pop32(s)); s.fr[15] = u2f(pop32(s));
	return_to(s.pr);                         // rts
}

} // namespace lightxf
