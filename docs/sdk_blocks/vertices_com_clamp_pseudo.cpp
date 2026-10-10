// Laço de vértices com clamp de cor (tipo DOA2) — pseudo-C++ para todos os jogos
// identificados (docs/sdk_blocks/vertices_com_clamp.md). API: ver
// luz_e_transformacao_de_vertices_pseudo.cpp. Versão bit-exata de hoje (só DOA2):
// core/rec-ARM64/hle_fn.cpp (doa2_run) — a de produção parte dela, trocando os
// endereços fixos por head + offset.
//
// Variantes: **nenhuma** no código (DOA2 = referência; MvC2 81 de 81, Shenmue II 65 de
// 65 opcodes iguais). Muda a base; os três jogos têm mais de uma cópia do laço.
//
//   Jogo            cabeça do laço (BC4)   outra cópia (mesmo código)
//   DOA2            8C101BC4               8C102592
//   MvC2            8C12AA84               8C12B452
//   Shenmue II      8C1D8D24               8C1D9E62
//
// Roda com FPSCR.SZ=1 (fmov.s move PARES de 8 bytes).
// Registro de vértice (32 bytes): x, y, z, n0, n1, n2, u, v.
// Stream de controle (r4), por vértice: flags (bit 0 = formato) e um deslocamento
// relativo até o próximo registro (formato 0) ou o próximo registro embutido (formato 1).
// Luzes: fv12 = luz difusa (fr15 = ambiente), fv8 = segunda luz (índice da tabela).
// Tabela de cor em r8, limiar em r2. Saída: 32 bytes por vértice na SQ (metade alta de
// cada janela de 64 bytes), com `pref` a cada vértice:
//   [PCW = endereço da janela na SQ (0xE...: tipo 7, vértice), sx, sy, 1/w, u, v,
//    intensidade, intensidade offset]

#pragma GCC optimize("fp-contract=off")
#include <arm_neon.h>

namespace vtxclamp {

constexpr u32 OFF_OUTER = 0x98;          // 8C101C5C - 8C101BC4: laço externo (fica no JIT)

static inline float32x4_t ftrv(const float32x4_t m[4], float a, float b, float c, float d)
{
	float32x4_t acc = vmulq_n_f32(m[0], a);
	acc = vfmaq_n_f32(acc, m[1], b);
	acc = vfmaq_n_f32(acc, m[2], c);
	return vfmaq_n_f32(acc, m[3], d);
}
static inline float fipr(float32x4_t a, float32x4_t b)
{
	const float32x4_t p = vmulq_f32(a, b);
	const float32x4_t s = vpaddq_f32(p, p);
	return vpadds_f32(vget_low_f32(s));
}

// `head` = endereço da cabeça do laço (BC4) neste jogo; (x, y) do 1º vértice já estão
// em fr4/fr5 (lidos pelo bloco anterior) e r14 aponta para o z.
void run(Sh4 &s, Cycles &cyc, u32 head)
{
	if (s.fpscr.PR || s.fpscr.SZ != 1)
		return bail(head);

	const float32x4_t m[4] = { vld1q_f32(&s.xf[0]), vld1q_f32(&s.xf[4]),
	                           vld1q_f32(&s.xf[8]), vld1q_f32(&s.xf[12]) };
	const float32x4_t light1 = { s.fr[12], s.fr[13], s.fr[14], s.fr[15] };
	const float32x4_t light2 = { s.fr[8], s.fr[9], s.fr[10], 0.f };
	const float ambient = s.fr[15];
	const u32 table = s.r[8];
	const s32 threshold = (s32)s.r[2];
	u32 rec = s.r[14], ctl = s.r[4], out = s.r[6], left = s.r[3];
	float x = s.fr[4], y = s.fr[5];

	for (;;)
	{
		if (cyc.left() < COST_VERTEX_MAX)    // a fatia acaba aqui: o JIT segue da cabeça
			return bail_with_state(s, head, rec, ctl, out, left, x, y);
		if (--left == 0)                     // dt r3; bt.s → laço externo
			return exit_at(s, head + OFF_OUTER);

		const float z = ramf(rec), n0 = ramf(rec + 4), n1 = ramf(rec + 8), n2 = ramf(rec + 12);
		const float u = ramf(rec + 16), v = ramf(rec + 20);
		const float32x4_t p = ftrv(m, x, y, z, 1.f);
		const u32 win = out, vtx = out + 32;
		out += 64;

		const u32 flags = ram32(ctl), rel = ram32(ctl + 4);
		ctl += 8;
		const float32x4_t nrm = { n1, n2, n0, 0.f };
		const float diffuse = fipr(nrm, light1);
		const float iw = 1.f / vgetq_lane_f32(p, 0);

		u32 next;
		if (flags & 1) { ctl += 24; next = ctl - 32; }   // formato 1: registro no stream
		else           next = ctl + rel;                 // formato 0: deslocamento relativo

		float intensity = ambient, offset = 0.f;
		const float l2 = fipr(light2, vsetq_lane_f32(diffuse, nrm, 3));
		if (diffuse > 0.f)
		{
			intensity = ambient + diffuse;
			const s32 idx = (s32)vcvts_s32_f32(l2);      // ftrc
			if (idx > threshold)
				offset = ramf(table + ((u32)idx << 2));   // único fmov de 4 bytes (fschg)
		}

		sq_write(vtx + 0, win);              // PCW: o endereço da janela na SQ
		sq_write(vtx + 4, f2u(vgetq_lane_f32(p, 1) * iw));
		sq_write(vtx + 8, f2u(vgetq_lane_f32(p, 2) * iw));
		sq_write(vtx + 12, f2u(iw));
		sq_write(vtx + 16, f2u(u));
		sq_write(vtx + 20, f2u(v));
		sq_write(vtx + 24, f2u(intensity));
		sq_write(vtx + 28, f2u(offset));
		sq_flush(vtx);
		cyc.take(cost_vertex(flags & 1, diffuse > 0.f, offset != 0.f));

		x = ramf(next); y = ramf(next + 4);
		rec = next + 8;
	}
}

} // namespace vtxclamp
