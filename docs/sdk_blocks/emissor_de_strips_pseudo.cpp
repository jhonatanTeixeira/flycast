// Emissor de strips de triângulo (`stripemit`) — pseudo-C++ para todos os jogos
// identificados (docs/sdk_blocks/emissor_de_strips.md). API: ver
// luz_e_transformacao_de_vertices_pseudo.cpp. Versão bit-exata de hoje (só Napple):
// core/rec-ARM64/hle_fn.cpp (stripemit_run).
//
// Variantes: **nenhuma** (134 de 134 opcodes iguais nos 5 jogos); muda só a base.
//
//   Jogo                                   base
//   Napple Tale                            8C14D440
//   Evolution - The World of Sacred Device 8C1C91EC
//   Evolution 2 - Far Off Promise          8C1BC5C0
//   Resident Evil - Code: Veronica         8C1BF720
//   Skies of Arcadia (disco 1)             8C2C9B80
//
// Literais (offset a partir da base): +0x144 shift do índice (int16), +0x168 máscara
// da contagem de strips, +0x16C PCW de vértice, +0x170 PCW de fim de strip,
// +0x174/+0x178 ponteiros de dois contadores globais (strips/vértices emitidos).
//
// Entrada:
//   r4   lista de strips (int16): por strip {count (negativo = tipo invertido), idx0,
//        e por vértice u, v, idx_próximo, mais `extra` int16 de enchimento}
//   r5   base dos registros de 32 bytes da lightxf (registro = r5 + (idx << shift))
//   r6   flags: (r6 & lit[0x168]) = número de strips; ((r6 >> 14) & 3) × 2 = `extra`
//   r7   destino na Store Queue
//   fr4  escala do alfa; fr5..fr7 ajuste de cor (o prólogo faz −1.0); fr8 escala de u,v
// Saída por vértice: 2 rajadas de 32 bytes na SQ (vértice do TA com cor base + offset):
//   [PCW, sx, sy, 1/z, u, v, (resto da SQ), (resto)]  e  [a, r+1, g+1, b+1, a, r, g, b]
//   onde a = rec.alpha·fr4 e r,g,b = rec.cor + fr5..fr7 (com o −1 do prólogo).

#pragma GCC optimize("fp-contract=off")

namespace stripemit {

constexpr u32 LIT_SHIFT = 0x144, LIT_CNTMASK = 0x168, LIT_PCW = 0x16C, LIT_PCW_END = 0x170,
              LIT_CNT_STRIPS = 0x174, LIT_CNT_VERTS = 0x178;

// O registro de 32 bytes que a lightxf gravou: z, sx, sy, 1/z, 1.0, r, g, b
struct Rec { float z, sx, sy, iz, one, r, g, b; };

static inline void emit_vertex(u32 &out, const Rec &v, float u, float vv, u32 pcw,
                               float aScale, float dr, float dg, float db)
{
	// rajada 1: posição + uv (o JIT grava de trás para frente; o efeito é este)
	sq_write(out + 0, pcw);
	sq_write(out + 4, f2u(v.sx));
	sq_write(out + 8, f2u(v.sy));
	sq_write(out + 12, f2u(v.iz));
	sq_write(out + 16, f2u(u));
	sq_write(out + 20, f2u(vv));
	sq_flush(out);
	out += 32;
	// rajada 2: cor base e cor offset
	const float a = v.one * aScale, r = v.r + dr, g = v.g + dg, b = v.b + db;
	sq_write(out + 0, f2u(a));
	sq_write(out + 4, f2u(r + 1.f));
	sq_write(out + 8, f2u(g + 1.f));
	sq_write(out + 12, f2u(b + 1.f));
	sq_write(out + 16, f2u(a));
	sq_write(out + 20, f2u(r));
	sq_write(out + 24, f2u(g));
	sq_write(out + 28, f2u(b));
	sq_flush(out);
	out += 32;
}

void run(Sh4 &s, Cycles &cyc, u32 base)
{
	if (s.fpscr.PR || s.fpscr.SZ)            // o prólogo liga SZ (fschg); entrada com SZ=0
		return bail(base);
	const u32 mask = ram32(base + LIT_CNTMASK);
	u32 strips = s.r[6] & mask;
	if (strips == 0)
		return bail(base);                   // nenhuma strip: caso raro, fica no JIT

	push32(s, s.r[8]); push32(s, s.r[9]); push32(s, s.r[10]); push32(s, s.r[11]); push32(s, s.r[12]);

	const u32 shift = (u32)ram16s(base + LIT_SHIFT);   // 5 = registro de 32 bytes
	const u32 extra = ((s.r[6] >> 14) & 3) * 2;        // bytes de enchimento por vértice
	const u32 pcw = ram32(base + LIT_PCW), pcwEnd = ram32(base + LIT_PCW_END);
	const float aScale = s.fr[4], uvScale = s.fr[8];
	const float dr = s.fr[5] - 1.f, dg = s.fr[6] - 1.f, db = s.fr[7] - 1.f;
	u32 list = s.r[4], out = s.r[7];
	const u32 recs = s.r[5];
	u32 totalA = 0, totalB = 0;              // os dois contadores globais (r2/r3 no SH4)

	while (strips--)
	{
		s32 count = (s16)ram16(list); list += 2;
		const u32 first = (u32)(s16)ram16(list); list += 2;
		if (count < 0) count = -count;       // tipo invertido: só muda o caminho do JIT
		totalA += count; totalB += count;    // (os dois somam a contagem da strip)
		if (cyc.left() < cost_strip(count))  // não cabe na fatia: o JIT faz esta strip
			return bail_at_strip(s, base, list - 4, out, strips + 1, totalA, totalB);

		u32 idx = first;
		for (s32 k = 0; k < count; k++)
		{
			const float u = (float)(s16)ram16(list) * uvScale;
			const float v = (float)(s16)ram16(list + 2) * uvScale;
			const Rec &rec = *(const Rec *)ram_ptr(recs + (idx << shift));
			const u32 next = (u32)(s16)ram16(list + 4);
			list += 6 + extra;
			emit_vertex(out, rec, u, v, k + 1 == count ? pcwEnd : pcw, aScale, dr, dg, db);
			idx = next;
		}
		cyc.take(cost_strip(count));
	}

	const u32 ca = ram32(base + LIT_CNT_STRIPS), cb = ram32(base + LIT_CNT_VERTS);
	write32(ca, read32(ca) + totalA);
	write32(cb, read32(cb) + totalB);
	s.r[0] = out;
	s.r[12] = pop32(s); s.r[11] = pop32(s); s.r[10] = pop32(s); s.r[9] = pop32(s); s.r[8] = pop32(s);
	return_to(s.pr);
}

} // namespace stripemit
