// Cabeçalho de polígono a partir do estado de render (Kamui2, provável
// `kmProcessVertexRenderState`) — pseudo-C++ para os 13 jogos do grupo 012
// (docs/sdk_find/auto/012_switch.md).
//
// Pseudo-código: não compila no core. É a base para um `hle_fn` relocável (4.125), no
// mesmo modelo de core/rec-ARM64/hle_fn.cpp (lightxf_run / stripemit_run / doa2_run).
//
// API usada nos *_pseudo.cpp (a mesma de docs/sdk_find/pseudo/idct_do_macrobloco_pseudo.cpp)
//   Sh4 &s            registradores: s.r[16], s.sr.T, s.pr
//   ram32/ram16s      leitura direta da RAM emulada (endereço & RAM_MASK); ram32w grava; is_ram(a)
//   Cycles &cyc       cyc.left() = o que sobra na fatia; cyc.take(n) desconta; cyc.update() =
//                     UpdateSystem na fronteira de bloco em que o JIT faria (true = interrupção
//                     ou CPU parada: o JIT assume dali)
//   bail(pc)          devolve ao JIT na entrada do bloco `pc` (estado já gravado, ciclos do
//                     bloco ainda não descontados)
//   leave_to_interrupt(pc)  saída do ENTER quando o UpdateSystem pede interrupção
//   clk_sh4()         g_lutSh4Clock / settings.dreamcast.sh4clock
//   return_to(pc)     rts
//
// ===================================================================================
// O QUE A FUNÇÃO FAZ
// ===================================================================================
// Recebe r4 = um "contexto de vértice" da biblioteca gráfica (a struct que o jogo mantém
// por material/objeto) e recompila, a partir dos campos que mudaram, as 4 palavras de
// cabeçalho de polígono do TA que ficam no próprio contexto:
//
//   ctx+0x90 PCW  (Parameter Control Word)   ctx+0x98 TSP instruction word
//   ctx+0x94 ISP/TSP instruction word        ctx+0x9C TCW (Texture Control Word)
//
// A palavra ctx+0 é a máscara de "o que mudou" (bits KM_* abaixo): cada bit ligado faz a
// função ler um campo do contexto, traduzir o enum da biblioteca para os bits do PowerVR
// e encaixar com máscara-e-ou na palavra certa. Os dois `braf` (o "switch" do grupo) são
// a tradução do modo de blending de origem e de destino: 12 valores de enum → código de
// 3 bits do TSP (e os modos "BOTH*", que escrevem origem E destino). No fim grava as 4
// palavras, devolve r0 = 0 (KMSTATUS_SUCCESS). Não toca no TA nem na Store Queue — quem
// manda o cabeçalho é a função seguinte no executável (base+0x7C0, que copia ctx+0x90..
// e as cores de face ctx+0x70.. para o "estado corrente" e escolhe o tipo de vértice:
// provável `kmSetVertexRenderState`).
//
// Identificação: os offsets dos campos, a ordem dos bits da máscara e o retorno 0 batem
// com a KMVERTEXCONTEXT da Kamui2 (RenderState, ParamType, ListType, ColorType, UVFormat,
// DepthMode, CullingMode, ..., pTextureSurfaceDesc). Os NOMES abaixo são da Kamui2 de
// memória (não há símbolo no dump); os offsets e bits são os do código.
//
// Máscara ctx+0 (bit → campo → destino):
//   0x00000001 KM_DEPTHMODE        +0x14       ISP[31:29] = v << 29
//   0x00000002 KM_CULLINGMODE      +0x18       ISP[28:27] = v << 27
//   0x00000020 KM_ZWRITE           +0x28       ISP[26]: v == 0 limpa; v != 0 NÃO VISTO
//   0x00000008 KM_SHADINGMODE      +0x20       bit1 = textura → ISP[25] e PCW[3];
//                                              bit0 = gouraud → ISP[23] e PCW[1]
//   0x00000200 KM_USESPECULAR      +0x40       ISP[24] = v << 24 (sem máscara de v) e PCW[2]
//   0x00800000 KM_UVFORMAT         +0x10       ISP[22] = v << 22 e PCW[0] = v
//   0x04000000 ? (provável DCALC)  +0x1C?      NÃO VISTO (ISP[20] sobrevive à máscara final)
//   0x00000040 KM_SRCBLENDINGMODE  +0x2C       TSP[31:29] (switch 1)
//   0x00000080 KM_DSTBLENDINGMODE  +0x30       TSP[28:26] (switch 2)
//   0x01000000 KM_SRCSELECT        +0x34       TSP[25]: v == 0 limpa; v != 0 NÃO VISTO
//   0x02000000 KM_DSTSELECT        +0x38       TSP[24]: v == 0 limpa; v != 0 NÃO VISTO
//   0x00000100 KM_FOGMODE          +0x3C       TSP[23:22] = (v & 3) << 22
//   0x00040000 KM_COLORCLAMP       +0x64       TSP[21]
//   0x00000400 KM_USEALPHA         +0x44       TSP[20]
//   0x00000800 KM_IGNORETEXTUREALPHA +0x48     TSP[19]
//   0x00001000 KM_CLAMPUV          +0x4C       TSP[16:15] = v << 15 (sem máscara de v)
//   0x00002000 KM_FLIPUV           +0x50       TSP[18:17] = v << 17 (sem máscara de v)
//   0x00004000 KM_FILTERMODE       +0x54       TSP[14:13] = tabela[v] << 13 (tabela de 4
//                                              palavras do executável, copiada para a pilha)
//   0x00008000 KM_SUPERSAMPLE      +0x58       TSP[12]: v == 0 limpa; v != 0 NÃO VISTO
//   0x00010000 KM_MIPMAPDADJUST    +0x5C       TSP[11:8] = (v & 15) << 8; v&15 == 0 NÃO VISTO
//   0x00020000 KM_TEXTURESHADINGMODE +0x60     TSP[7:6] = (v & 3) << 6
//   (sempre, se textura e +0x6C != 0)  surface  TSP[5:3]/[2:0] = log2(largura/altura) − 3
//                                              e o TCW (ver abaixo)
//   0x00100000 KM_PARAMTYPE        +0x04       PCW[31:29]: 0 → 4 (polígono), 2 → 5 (sprite);
//                                              1 NÃO VISTO; outros: não muda
//   0x00200000 KM_LISTTYPE         +0x08       PCW[26:24] = v << 24
//   (sempre)                                   PCW[23] = 1 (group_en), PCW[19:18] =
//                                              *global << 18 (provável strip length)
//   0x08000000 ?                               NÃO VISTO (código em base+0x608)
//   0x10000000 KM_USERCLIPMODE     +0xBC       PCW[17:16] = v << 16
//   (sempre)                       +0xA0       PCW[7:6] limpos; (v & 0x60000000) != 0 NÃO VISTO
//   0x00000010 KM_MODIFIER         +0x24       (v & 1) == 0 nada; (v & 1) != 0 NÃO VISTO
//   0x00400000 KM_COLORTYPE        +0x0C       PCW[5:4] = v << 4
//   No fim: ISP &= 0xFFD00000, PCW &= 0xE78F00FF.
//
// Switch do blending (tabela int16 lida da RAM: base+0x16C origem, base+0x248 destino;
// alvo = base+0x164+d / base+0x246+d). Corpos vistos, por offset do alvo, e o código PVR:
//   origem  0x184 BOTHINVSRCALPHA  src 5, dst 4      destino 0x260 BOTHINVSRCALPHA
//           0x190 BOTHSRCALPHA     src 4, dst 5              0x26E BOTHSRCALPHA
//           0x1B0 INVDESTCOLOR?    src 3                     0x28A INVSRCALPHA   dst 5
//           0x1BC SRCALPHA         src 4                     0x290 INVSRCCOLOR   dst 3
//           0x1C2 ONE              src 1                     0x2A2 ONE           dst 1
//           0x220 ZERO             src 0                     0x2A8 ZERO          dst 0
// A ordem dos corpos na memória bate com o enum alfabético da Kamui2 (BOTHINVSRCALPHA,
// BOTHSRCALPHA, DESTALPHA, DESTCOLOR, INVDESTALPHA, INVDESTCOLOR, INVSRCALPHA,
// INVSRCCOLOR, SRCALPHA, SRCCOLOR, ONE, ZERO), com os corpos de 6 bytes nos buracos certos;
// por isso o nome dos casos é provável, mas o índice → caso NÃO foi visto (a tabela é
// dado, não está no dump). A versão nativa lê a tabela da RAM: não depende disso.
// Corpos NÃO vistos (inferidos pela posição, ficam no JIT): origem 0x19E DESTALPHA (6),
// 0x1A4 DESTCOLOR (2), 0x1AA INVDESTALPHA (7), 0x1B6 INVSRCALPHA (5); destino 0x27E
// DESTALPHA (6), 0x284 INVDESTALPHA (7), 0x296 SRCALPHA (4), 0x29C SRCCOLOR (2); e o
// "default" (enum inválido para aquele lado), em algum lugar de 0x1C8..0x21F / 0x2??.
//
// Textura (ShadingMode >> 1 == 1 e ctx+0x6C = surface != 0), surface:
//   +8 formato (TCW[29:27] = s8 & 0x38000000), +12 largura, +16 altura, +24 flags
//   (bit0 → TCW[31] mipmap, bit3 → TCW[30] VQ, bit2 == 0 → TCW[26] não-twiddled,
//   bit4 → TCW[25] stride), +28 endereço na VRAM (TCW[20:0] = s28 >> 3).
//   Formato paletizado (4bpp 0x28000000 / 8bpp 0x30000000): NÃO VISTO (base+0x524,
//   provavelmente usa o banco de paleta em ctx+0x68).
//   Sub-rotina base−0x300 (bsr): 8,16,...,1024 → 0..7; qualquer outro valor volta igual.
//
// Pilha (sp = r15 depois de 8 push + 36): sp+0 rascunho, sp+4 = máscara & UVFORMAT,
// sp+8 = TCW em montagem, sp+12 = máscara & USESPECULAR, sp+16 = surface, sp+20..35 =
// cópia da tabela do FilterMode. Tudo isso é RAM do jogo: a versão nativa grava igual.
//
// ===================================================================================
// JOGOS (entrada = `base`) — todos com a função de 0x780 bytes (base .. base+0x77F)
// ===================================================================================
//   Jogo                                       base      perf    *(base+0x84) *(base+0x728)
//   Elemental Gimmick Gear                     8C086420  4,18%   8C0A475C     8C3F5880
//   Shenmue (disco 1)                          0C03B960  0,12%   0C08A6BC     (não dobrado)
//   Napple Tale                                8C12C720  0,03%   8C1B57B8     8C362C1C
//   Evolution - The World of Sacred Device     8C1CF840  0       8C201EC8     8C358B8C
//   Evolution 2 - Far Off Promise              8C19E5E0  0       8C2112C0     8C3D6034
//   Grandia II                                 8C0BA2E0  0       8C112470     8C267AB4
//   King of Fighters - Evolution               8C35FE40  0       8C3BA8DC     8C7C77BC
//   Macross M3                                 8C13B260  0       8C202100     8C63EA58
//   Phantasy Star Online Ver. 2                8C3C80A0  0       8C405470     8C5B8F48
//   Resident Evil - Code: Veronica (disco 1)   8C1C9740  0       8C1F8630     8C38E270
//   Skies of Arcadia (disco 1)                 8C2E19A0  0       8C36BA30     8C5D7C6C
//   Sonic Adventure 2                          8C14F920  0       8C17C5C4     (não dobrado)
//   Soulcalibur                                8C227420  0       8C24615C     8C379CA8
// (perf = % da thread de emulação no dump de cada jogo; "não dobrado" = o SHIL desse
// dump leu o literal da RAM em vez de dobrar a constante; o código é o mesmo.)
//
// Quente (EGG, jit_lite_report): ~44 blocos do JIT por chamada, todos curtos (2..9 instr.),
// o maior é o prólogo (8C086420, 0,49%). O caminho quente é máscara = 0xC0 (só os dois
// blendings) com origem SRCALPHA (0x1BC) e destino INVSRCALPHA (0x28A): o jogo troca o
// modo de alfa a cada sprite. Custo desse caminho no JIT: ~230 ciclos SH4 em 44 blocos
// (o prólogo + 43 blocos de 2..12 ciclos, cada um com despacho e checagem de fatia). Na
// versão nativa vira uma chamada só, com um `sub`+`b.lt` por fronteira de bloco.
//
// ===================================================================================
// VARIANTES
// ===================================================================================
// **Nenhuma no código.** Os 13 jogos têm a função com o mesmo tamanho (0x780) e, na
// união das listagens (o que cada dump executou), 0 conflito de opcode no mesmo offset;
// a sub-rotina em base−0x300 também é igual nos 12 jogos que a executaram (Soulcalibur
// nunca entrou no caminho de textura; o `bsr` tem o mesmo deslocamento). As constantes
// do literal pool vistas no SHIL são as mesmas nos 13; só mudam 2 ponteiros por jogo,
// lidos da RAM pela versão nativa (valores na tabela acima):
//   base+0x084  → tabela de 4 palavras do FilterMode (dado do executável)
//   base+0x728  → global que vai para PCW[19:18]
// As "10 variantes" do sdk_find são só COBERTURA diferente (cada jogo passou por campos
// diferentes): cor clamp = 1 só KOF; user clip só Napple/PSO/RE:CV; ParamType = 2 em
// Grandia/KOF/Macross/Napple/PSO/SA2/Skies; BOTH* em Evolution 1/2 (BOTHSRC) e Skies
// (BOTHINVSRC); origem INVDESTCOLOR só PSO; origem ZERO Napple/RE:CV; destino INVSRCCOLOR
// Napple/PSO; IgnoreTextureAlpha = 1 EGG/Shenmue; UseSpecular = 1 em todos menos
// Skies/SA2/Soulcalibur.
//
// ===================================================================================
// O QUE NÃO FOI DETERMINADO (sai para o JIT — nenhum jogo passou por aí nos dumps)
// ===================================================================================
//   base+0x076 ZWRITE != 0          base+0x10C máscara 0x04000000     base+0x2BA SRCSELECT != 0
//   base+0x2CC DSTSELECT != 0       base+0x420 SUPERSAMPLE != 0       base+0x476 MIPMAPADJ & 15 == 0
//   base+0x524 textura paletizada   base+0x5D4 PARAMTYPE == 1         base+0x608 máscara 0x08000000
//   base+0x68C ctx+0xA0 & 0x60000000 base+0x6A4 MODIFIER & 1          casos do switch não vistos
// e: o significado exato dos bits 0x04000000/0x08000000 e de ctx+0x1C/0x68/0xA0; o
// mapeamento índice → caso dos dois switches (lido da RAM, não precisa); o que é a global
// de base+0x728 (o valor vai cru, sem máscara, para PCW << 18).
//
// Custos: o número de cada ENTER é a coluna guest_cycles do dump (clock 1,0), igual nos
// 13 jogos. Blocos pequenos só de leitura que terminam em `bf` levam ×3 (truque de
// espera do decoder: 0x49C, 0x5BE, 0x51E e os testes da sub-rotina). O bloco de entrada
// (36) já foi descontado pelo JIT antes da chamada, como no hle_fn.cpp.

namespace cabecalho_poligono {

constexpr u32 LIT_FILTRO   = 0x084;   // → tabela de 4 palavras do FilterMode (por jogo)
constexpr u32 LIT_STRIPLEN = 0x728;   // → global copiada para PCW[19:18] (por jogo)
constexpr u32 TAB_SRC = 0x16C, BRAF_SRC = 0x164;   // tabela int16 do switch 1, base do braf
constexpr u32 TAB_DST = 0x248, BRAF_DST = 0x246;   // idem, switch 2

// Campos do contexto (nomes da KMVERTEXCONTEXT, inferidos)
enum : u32 {
	CTX_FLAGS = 0x00, CTX_PARAMTYPE = 0x04, CTX_LISTTYPE = 0x08, CTX_COLORTYPE = 0x0C,
	CTX_UVFORMAT = 0x10, CTX_DEPTH = 0x14, CTX_CULL = 0x18, CTX_SHADING = 0x20,
	CTX_MODIFIER = 0x24, CTX_ZWDIS = 0x28, CTX_SRCBLEND = 0x2C, CTX_DSTBLEND = 0x30,
	CTX_SRCSEL = 0x34, CTX_DSTSEL = 0x38, CTX_FOG = 0x3C, CTX_SPECULAR = 0x40,
	CTX_USEALPHA = 0x44, CTX_IGNTEXA = 0x48, CTX_CLAMPUV = 0x4C, CTX_FLIPUV = 0x50,
	CTX_FILTER = 0x54, CTX_SUPERS = 0x58, CTX_MIPADJ = 0x5C, CTX_TEXSHADE = 0x60,
	CTX_CCLAMP = 0x64, CTX_SURFACE = 0x6C,
	CTX_PCW = 0x90, CTX_ISP = 0x94, CTX_TSP = 0x98, CTX_TCW = 0x9C,
	CTX_A0 = 0xA0, CTX_USERCLIP = 0xBC,
};

// Sub-rotina base−0x300: {bloco do teste, custo, valor, bloco do acerto, custo}
struct Passo { s32 t; u32 ct; u32 v; s32 a; u32 ca; };
static constexpr Passo LOG2[8] = {
	{-0x300, 3,     8, -0x2FA, 2}, {-0x2F6, 2,    16, -0x2F2, 2},
	{-0x2EE, 3,    32, -0x2E8, 2}, {-0x2E4, 2,    64, -0x2E0, 2},
	{-0x2DC, 9,  0x80, -0x2D6, 2}, {-0x2A0, 9, 0x100, -0x29A, 2},
	{-0x296, 9, 0x200, -0x290, 2}, {-0x28C, 9, 0x400, -0x286, 3} };

void run(Sh4 &s, Cycles &cyc, u32 base)
{
	if (!is_ram(s.r[4]) || !is_ram(s.r[4] + 0xBC) || !is_ram(s.r[15] - 68))
		return bail(base);                   // ponteiro estranho: o JIT faz tudo

	// Registradores do SH4 em locais; voltam ao contexto em flush() (fronteira de fatia,
	// saída para o JIT, fim). r0..r7 e T terminam com os valores que o SH4 deixaria.
	u32 r0 = s.r[0], r1 = s.r[1], r2 = s.r[2], r3 = s.r[3], r4 = s.r[4], r5 = s.r[5],
	    r6 = s.r[6], r7 = s.r[7], r8 = s.r[8], r9 = s.r[9], r10 = s.r[10], r11 = s.r[11],
	    r12 = s.r[12], r13 = s.r[13], r14 = s.r[14], sp = s.r[15], pr = s.pr;
	bool T = s.sr.T;
	const float clk = clk_sh4();

	auto flush = [&] {
		s.r[0] = r0; s.r[1] = r1; s.r[2] = r2; s.r[3] = r3; s.r[4] = r4; s.r[5] = r5;
		s.r[6] = r6; s.r[7] = r7; s.r[8] = r8; s.r[9] = r9; s.r[10] = r10; s.r[11] = r11;
		s.r[12] = r12; s.r[13] = r13; s.r[14] = r14; s.r[15] = sp; s.pr = pr; s.sr.T = T;
	};
	// Entrada do bloco do JIT em base+off (custo n do dump): o ENTER do hle_fn.cpp. Só
	// grava o estado quando a fatia acaba ali (UpdateSystem); false = interrupção.
	auto enter = [&](s32 off, u32 n) -> bool {
		cyc.take(std::max<s32>(1, (s32)(n * clk)));
		if (__builtin_expect(cyc.left() >= 0, 1))
			return true;
		flush();
		if (cyc.update()) { leave_to_interrupt(base + off); return false; }
		return true;
	};
#define ENTER(off, n) do { if (!enter(off, n)) return; } while (0)
#define BAIL(off)     do { flush(); return bail(base + (off)); } while (0)
	// T como o SH4 deixa: tst (bt pula se o AND dá 0), cmp/eq, cmp/hs
#define TST(x)        (T = ((x) == 0))
#define CMPEQ(a, b)   (T = ((a) == (b)))
#define CMPHS(a, b)   (T = ((u32)(a) >= (u32)(b)))
	auto w32 = [](u32 a, u32 v) { ram32w(a, v); };

	// Sub-rotina log2 (bsr base−0x300). Entra com r4 = tamanho e pr = retorno.
	auto log2tam = [&]() -> bool {
		for (u32 k = 0; k < 8; k++)
		{
			if (!enter(LOG2[k].t, LOG2[k].ct)) return false;
			switch (k) {                     // o que cada bloco de teste carrega
			case 0: case 2: r0 = r4; break;  // mov r4,r0
			case 4: r3 = 0x80; break;        // mov.w @(lit),rN
			case 5: r2 = 0x100; break;
			case 6: r1 = 0x200; break;
			case 7: r3 = 0x400; break;
			}
			if (!CMPEQ(r4, LOG2[k].v)) continue;
			if (!enter(LOG2[k].a, LOG2[k].ca)) return false;
			r4 = k;                          // mov #k,r4
			if (k < 7 && !enter(-0x284, 2)) return false;   // rts; mov r4,r0
			r0 = r4;
			return true;
		}
		if (!enter(-0x284, 2)) return false; // nenhum: devolve o próprio valor
		r0 = r4;
		return true;
	};

	// ---- 0x000 prólogo (bloco já descontado pelo JIT) --------------------------------
	for (u32 v : { r14, r13, r12, r11, r10, r9, r8, pr }) { sp -= 4; w32(sp, v); }
	sp -= 36;
	r13 = r4;
	r2 = sp + 20;
	r11 = ram32(r13 + CTX_ISP);
	r14 = ram32(r13 + CTX_TSP);
	r3 = ram32(r13 + CTX_TCW);
	r0 = CTX_PCW;
	w32(sp + 8, r3);
	r3 = ram32(base + LIT_FILTRO);           // cópia da tabela do FilterMode para sp+20
	r10 = ram32(r13 + CTX_PCW);
	r1 = ram32(r3);      r0 = ram32(r3 + 4);
	w32(r2, r1);         w32(r2 + 4, r0);
	r0 = ram32(r3 + 12); r1 = ram32(r3 + 8);
	r9 = 0x1FFFFFFF;
	w32(r2 + 8, r1);     w32(r2 + 12, r0);
	r2 = 1;
	r12 = ram32(r13 + CTX_FLAGS);            // máscara do que mudou
	r3 = 2;                                  // slot do bt.s

	// ---- ISP[31:27]: DEPTHMODE, CULLINGMODE --------------------------------------------
	if (!TST(r12 & 1)) {
		ENTER(0x048, 9);
		r1 = ram32(r13 + CTX_DEPTH); r0 = r11 & r9; r2 = 0x1D; r1 <<= 29; r11 = r0 | r1;
	} else
		ENTER(0x056, 2);
	if (!TST(r12 & r3)) {
		ENTER(0x05A, 11);
		r1 = 0xE7FFFFFF & r11; r3 = 0x1B; r2 = ram32(r13 + CTX_CULL) << 27; r11 = r1 | r2;
	} else
		ENTER(0x068, 4);
	r3 = 0x20; r7 = 0x04000000;

	// ---- ISP[26]: ZWRITE ------------------------------------------------------------------
	if (!TST(r12 & r3)) {
		ENTER(0x070, 3);
		r1 = ram32(r13 + CTX_ZWDIS);
		if (!TST(r1)) BAIL(0x076);           // ZWriteDisable != 0: NÃO VISTO
		ENTER(0x0A0, 6);
		r2 = 0xFBFFFFFF; r11 &= r2;
	} else
		ENTER(0x0A4, 4);
	r8 = r12 & 8;                            // SHADINGMODE mudou (vale até o fim)

	// ---- ISP[25] textura, [24] specular, [23] gouraud, [22] UV16 --------------------------
	if (!TST(r8)) {
		ENTER(0x0AC, 15);
		r3 = ram32(r13 + CTX_SHADING); r2 = 0xFDFFFFFF & r11; r11 = r2;
		r3 = (r3 >> 1) << 25;                // shlr; shll16; shll8; shll
		r11 |= r3;
	} else
		ENTER(0x0BE, 6);
	r3 = 0x200 & r12; r6 = 0xFEFFFFFF;
	TST(r3);
	w32(sp + 12, r3);                        // slot do bt.s
	if (!T) {
		ENTER(0x0CA, 10);
		r0 = 0x40; r1 = r11 & r6; r3 = ram32(r13 + CTX_SPECULAR) << 24; r11 = r1 | r3;
	} else
		ENTER(0x0DA, 2);
	if (!TST(r8)) {
		ENTER(0x0DE, 13);
		r0 = (ram32(r13 + CTX_SHADING) & 1) << 23; r3 = 0x17;
		r2 = 0xFF7FFFFF & r11; r11 = r2 | r0;
	} else
		ENTER(0x0EE, 5);
	r2 = 0x800000 & r12;
	TST(r2);
	w32(sp + 4, r2);
	if (!T) {
		ENTER(0x0F8, 10);
		r1 = 0xFFBFFFFF & r11; r3 = 0x16; r2 = ram32(r13 + CTX_UVFORMAT) << 22; r11 = r1 | r2;
	} else
		ENTER(0x106, 3);
	r3 = r12;
	if (!TST(r3 & r7)) BAIL(0x10C);          // máscara 0x04000000: NÃO VISTO

	// ---- TSP[31:29]: SRCBLENDINGMODE (switch 1) ----------------------------------------
	ENTER(0x144, 6);
	r3 = 0x40; r5 = 0xE3FFFFFF; r4 = r14 & r9;
	bool src_feito = false;                  // true = o caso já incluiu o bloco 0x222
	if (!TST(r12 & r3)) {
		ENTER(0x150, 4);
		r0 = ram32(r13 + CTX_SRCBLEND); r1 = 12;
		if (!CMPHS(r0, r1)) {
			ENTER(0x158, 6);
			T = (r0 >> 31) != 0;             // shll (índice < 12: T = 0)
			r0 <<= 1; r1 = r0;
			r0 = (u32)(s32)ram16s(base + TAB_SRC + r1);   // mova + mov.w @(r0,r1)
			const u32 alvo = BRAF_SRC + r0;  // braf: offset do caso a partir de base
			switch (alvo) {
			case 0x184:                      // BOTHINVSRCALPHA: src 5, dst 4
				ENTER(0x184, 6);
				r3 = 0xA0000000; r2 = 0x10000000; r4 |= r3; r14 = r4 & r5;
				ENTER(0x19A, 2);
				r14 |= r2; break;
			case 0x190:                      // BOTHSRCALPHA: src 4, dst 5
				ENTER(0x190, 7);
				r3 = 0x80000000; r2 = 0x14000000; r4 |= r3; r14 = (r4 & r5) | r2; break;
			case 0x1B0:                      // src 3 (INVDESTCOLOR?)
				ENTER(0x1B0, 3); r14 = 0x60000000; ENTER(0x1C4, 2); r14 |= r4; break;
			case 0x1BC:                      // SRCALPHA: src 4 (EGG, quente)
				ENTER(0x1BC, 3); r14 = 0x80000000; ENTER(0x1C4, 2); r14 |= r4; break;
			case 0x1C2:                      // ONE: src 1
				ENTER(0x1C2, 3); r14 = 0x20000000 | r4; break;
			case 0x220:                      // ZERO: src 0, cai direto no 0x222
				ENTER(0x220, 4); r14 = r4; src_feito = true; break;
			default:
				BAIL(alvo);                  // corpo não visto (ou default): JIT
			}
		}
	}
	if (!src_feito)
		ENTER(0x222, 3);
	r3 = 0x80;

	// ---- TSP[28:26]: DSTBLENDINGMODE (switch 2) ----------------------------------------
	bool dst_feito = false;
	if (!TST(r12 & r3)) {
		ENTER(0x228, 9);
		r0 = r14 & r9; w32(sp, r0);          // TSP sem a origem (usado pelos BOTH*)
		r4 = r14 & r5;                       // TSP sem o destino
		r0 = ram32(r13 + CTX_DSTBLEND); r1 = 12;
		if (!CMPHS(r0, r1)) {
			ENTER(0x23A, 6);
			T = (r0 >> 31) != 0;
			r0 <<= 1; r1 = r0;
			r0 = (u32)(s32)ram16s(base + TAB_DST + r1);
			const u32 alvo = BRAF_DST + r0;
			switch (alvo) {
			case 0x260:                      // BOTHINVSRCALPHA
				ENTER(0x260, 7);
				r4 = ram32(sp); r3 = 0xA0000000; r2 = 0x10000000; r4 |= r3; r14 = r4 & r5;
				ENTER(0x27A, 2);
				r14 |= r2; break;
			case 0x26E:                      // BOTHSRCALPHA
				ENTER(0x26E, 8);
				r4 = ram32(sp); r3 = 0x80000000; r2 = 0x14000000; r4 |= r3;
				r14 = (r4 & r5) | r2; break;
			case 0x28A:                      // INVSRCALPHA: dst 5 (EGG, quente)
				ENTER(0x28A, 3); r14 = 0x14000000; ENTER(0x29E, 2); r14 |= r4; break;
			case 0x290:                      // INVSRCCOLOR: dst 3
				ENTER(0x290, 3); r14 = 0x0C000000; ENTER(0x29E, 2); r14 |= r4; break;
			case 0x2A2:                      // ONE: dst 1 (r7 = 0x04000000)
				ENTER(0x2A2, 3); r14 = r4 | r7; break;
			case 0x2A8:                      // ZERO: cai direto no 0x2AA
				ENTER(0x2A8, 6); r14 = r4; dst_feito = true; break;
			default:
				BAIL(alvo);
			}
		}
	}
	if (!dst_feito)
		ENTER(0x2AA, 5);
	r4 = 0x01000000; r3 = r12; r5 = 0x02000000;

	// ---- TSP[25] SRCSELECT, [24] DSTSELECT, [23:22] FOGMODE ----------------------------
	if (!TST(r3 & r4)) {
		ENTER(0x2B4, 3);
		r1 = ram32(r13 + CTX_SRCSEL);
		if (!TST(r1)) BAIL(0x2BA);           // SRCSelect != 0: NÃO VISTO
		ENTER(0x2BE, 4);
		r2 = 0xFDFFFFFF; r14 &= r2;
	} else
		ENTER(0x2C2, 2);
	if (!TST(r12 & r5)) {
		ENTER(0x2C6, 3);
		r2 = ram32(r13 + CTX_DSTSEL);
		if (!TST(r2)) BAIL(0x2CC);           // DSTSelect != 0: NÃO VISTO
		ENTER(0x320, 5);
		r14 &= r6;                           // r6 = 0xFEFFFFFF desde 0x0AC/0x0BE
	} else
		ENTER(0x322, 4);
	r2 = 0x100; r5 = 3;
	if (!TST(r12 & r2)) {
		ENTER(0x32A, 11);
		r2 = 0xFF3FFFFF & r14; r3 = 0x16; r4 = (ram32(r13 + CTX_FOG) & r5) << 22; r14 = r2 | r4;
	} else
		ENTER(0x33A, 3);
	r2 = 0x40000;

	// ---- TSP[21] COLORCLAMP, [20] USEALPHA, [19] IGNORETEXTUREALPHA ----------------------
	if (!TST(r12 & r2)) {
		ENTER(0x340, 4);
		r0 = CTX_CCLAMP; r1 = ram32(r13 + CTX_CCLAMP);
		if (!TST(r1)) { ENTER(0x348, 3); r3 = 0x200000; r14 |= r3; ENTER(0x352, 3); }
		else          { ENTER(0x34E, 5); r1 = 0xFFDFFFFF; r14 &= r1; }
	} else
		ENTER(0x352, 3);
	r3 = 0x400;
	if (!TST(r12 & r3)) {
		ENTER(0x358, 4);
		r0 = CTX_USEALPHA; r1 = ram32(r13 + CTX_USEALPHA);
		if (!TST(r1)) { ENTER(0x360, 3); r2 = 0x100000; r14 |= r2; ENTER(0x36A, 3); }
		else          { ENTER(0x366, 5); r1 = 0xFFEFFFFF; r14 &= r1; }
	} else
		ENTER(0x36A, 3);
	r3 = 0x800;
	if (!TST(r12 & r3)) {
		ENTER(0x370, 4);
		r0 = CTX_IGNTEXA; r1 = ram32(r13 + CTX_IGNTEXA);
		if (!TST(r1)) { ENTER(0x378, 3); r2 = 0x80000; r14 |= r2; ENTER(0x3C4, 4); }
		else          { ENTER(0x3C0, 6); r1 = 0xFFF7FFFF; r14 &= r1; }
	} else
		ENTER(0x3C4, 4);
	r6 = 0x1000; r3 = r12;

	// ---- TSP[16:15] CLAMPUV, [18:17] FLIPUV, [14:13] FILTER, [12] SS, [11:8] MIPADJ, [7:6] --
	if (!TST(r3 & r6)) {
		ENTER(0x3CC, 11);
		r0 = CTX_CLAMPUV; r1 = 0xFFFE7FFF & r14; r2 = ram32(r13 + CTX_CLAMPUV) << 15;
		r3 = 0x0F; r14 = r1 | r2;
	} else
		ENTER(0x3DC, 3);
	r3 = 0x2000;
	if (!TST(r12 & r3)) {
		ENTER(0x3E2, 11);
		r0 = CTX_FLIPUV; r1 = 0xFFF9FFFF & r14; r3 = ram32(r13 + CTX_FLIPUV) << 17; r14 = r1 | r3;
	} else
		ENTER(0x3F2, 3);
	r3 = 0x4000;
	if (!TST(r12 & r3)) {
		ENTER(0x3F8, 16);
		r0 = CTX_FILTER; r3 = sp + 20; r2 = 0xFFFF9FFF & r14; r1 = 0x0D;
		r4 = ram32((ram32(r13 + CTX_FILTER) << 2) + r3) << 13;   // índice sem limite, como o jogo
		r14 = r2 | r4;
	} else
		ENTER(0x412, 3);
	r3 = 0x8000;
	if (!TST(r12 & r3)) {
		ENTER(0x418, 4);
		r0 = CTX_SUPERS; r1 = ram32(r13 + CTX_SUPERS);
		if (!TST(r1)) BAIL(0x420);           // SuperSample != 0: NÃO VISTO
		ENTER(0x460, 5);
		r2 = 0xFFFFEFFF; r14 &= r2;
	} else
		ENTER(0x464, 3);
	r3 = 0x10000;
	if (!TST(r12 & r3)) {
		ENTER(0x46A, 6);
		r0 = ram32(r13 + CTX_MIPADJ) & 15; r4 = r0;
		if (TST(r4)) BAIL(0x476);            // MipMapAdjust & 15 == 0: NÃO VISTO (1 instr.)
		ENTER(0x478, 8);
		r2 = 0xFFFFF0FF & r14; r4 <<= 8; r14 = r2 | r4;
	} else
		ENTER(0x482, 3);
	r3 = 0x20000;
	if (!TST(r12 & r3)) {
		ENTER(0x488, 14);
		r0 = CTX_TEXSHADE; r3 = 0xFFFFFF3F & r14; r4 = (ram32(r13 + CTX_TEXSHADE) & r5) << 6;
		r14 = r3 | r4;
	} else
		ENTER(0x49C, 12);
	r0 = (u32)((s32)ram32(r13 + CTX_SHADING) >> 1);

	// ---- TSP[5:0]: log2 da largura e da altura da textura ------------------------------
	bool tam_feito = false;
	if (CMPEQ(r0, 1)) {                      // textura
		ENTER(0x4A4, 4);
		r0 = CTX_SURFACE; r3 = ram32(r13 + CTX_SURFACE);
		if (!TST(r3)) {
			ENTER(0x4AC, 5);
			r4 = ram32(r13 + CTX_SURFACE); r3 = 0xFFFFFFC7; r14 &= r3;
			pr = base + 0x4B6; r4 = ram32(r4 + 12);           // bsr; slot: largura
			if (!log2tam()) return;
			ENTER(0x4B6, 10);
			r0 <<= 2; T = (r0 >> 31) != 0; r0 <<= 1;           // shll2; shll
			r14 |= r0; r0 = CTX_SURFACE; w32(sp, r14);
			r4 = ram32(r13 + CTX_SURFACE); r3 = 0xFFFFFFF8; r14 &= r3;
			pr = base + 0x4CA; r4 = ram32(r4 + 16);           // bsr; slot: altura
			if (!log2tam()) return;
			ENTER(0x4CA, 6);
			r14 |= r0;
			tam_feito = true;
		}
	}
	if (!tam_feito)
		ENTER(0x4CC, 5);
	r0 = (u32)((s32)ram32(r13 + CTX_SHADING) >> 1);
	CMPEQ(r0, 1);
	r5 = 0x10;                               // slot do bt.s: vale até o fim

	// ---- TCW a partir da surface -------------------------------------------------------
	if (T) {
		ENTER(0x4DA, 4);
		r0 = CTX_SURFACE; r3 = ram32(r13 + CTX_SURFACE);
		if (!TST(r3)) {
			ENTER(0x4E2, 30);
			r0 = CTX_SURFACE; r3 = 0x7FFFFFFF; r4 = ram32(r13 + CTX_SURFACE);
			r7 = ram32(sp + 8); r1 = 8; r6 = ram32(r4 + 24); r2 = 0xBFFFFFFF;
			r7 &= r3;
			r0 = (1 & r6) << 31;             // and #1; rotr → TCW[31] mipmap
			r6 &= r1; r3 = 0x1B; r7 |= r0;
			r0 = 0x28000000; r6 <<= 27;      // flags bit3 → TCW[30] VQ
			r3 = ram32(r4 + 8); r7 &= r2; r2 = 0xC7FFFFFF; r7 |= r6;
			r6 = 0x38000000; r7 &= r2; r4 = r7; r3 &= r6; r4 |= r3;   // TCW[29:27] formato
			r7 = r4 & r6;
			if (CMPEQ(r7, r0)) BAIL(0x524);  // 4bpp paletizado: NÃO VISTO
			ENTER(0x51E, 9);
			r1 = 0x30000000;
			if (CMPEQ(r7, r1)) BAIL(0x524);  // 8bpp paletizado: NÃO VISTO
			ENTER(0x580, 31);
			r0 = CTX_SURFACE; r3 = 0xFBFFFFFF; r7 = ram32(r13 + CTX_SURFACE);
			r6 = 0xFDFFFFFF; r4 &= r3; w32(sp + 16, r7); r2 = 0x15;
			r7 = ram32(r7 + 24);
			r0 = ((~(r7 & 4)) & 4) << 24;    // flags bit2 == 0 → TCW[26] não-twiddled
			r7 = (r7 & r5) << 21;            // flags bit4 → TCW[25] stride
			r4 |= r0; r6 &= r4; r6 |= r7;
			r0 = CTX_SURFACE; r3 = 0xFFE00000; r2 = ram32(r13 + CTX_SURFACE);
			r6 &= r3;
			r1 = ram32(r2 + 28) >> 3;        // TCW[20:0] = endereço / 8
			r6 |= r1;
			w32(sp + 8, r6);
			goto pcw;                        // o bloco 0x580 já inclui o 0x5B8
		}
	} else
		ENTER(0x4D6, 2);                     // bra 0x5B8
	ENTER(0x5B8, 3);
pcw:
	r3 = 0x100000;

	// ---- PCW[31:29] PARAMTYPE, [26:24] LISTTYPE, [23] group_en, [19:18], [17:16] --------
	if (!TST(r12 & r3)) {
		ENTER(0x5BE, 12);
		r4 = ram32(r13 + CTX_PARAMTYPE); TST(r4); r0 = r4;
		if (T) {
			ENTER(0x5C6, 7);
			r2 = r10 & r9; r10 = 0x80000000 | r2;   // tipo 4 (polígono)
		} else
			ENTER(0x5CE, 3);
		CMPEQ(r0, 1); r0 = r4;
		if (T) BAIL(0x5D4);                  // ParamType 1: NÃO VISTO
		ENTER(0x5DC, 2);
		if (CMPEQ(r0, 2)) {
			ENTER(0x5E0, 6);
			r9 &= r10; r10 = 0xA0000000 | r9;       // tipo 5 (sprite); r9 fica sujo até o pop
		} else
			ENTER(0x5E6, 3);
	} else
		ENTER(0x5E6, 3);
	r2 = 0x200000;
	if (!TST(r12 & r2)) {
		ENTER(0x5EC, 14);
		r3 = ram32(r13 + CTX_LISTTYPE) << 24; r1 = 0xF8FFFFFF & r10; r10 = r1 | r3;
	} else
		ENTER(0x5FA, 7);
	r3 = 0x08000000; r2 = 0x800000;
	TST(r12 & r3);
	r10 |= r2; r6 = 0xFFF3FFFF & r10;
	if (!T) BAIL(0x608);                     // máscara 0x08000000: NÃO VISTO

	ENTER(0x660, 8);
	r3 = ram32(base + LIT_STRIPLEN); r2 = 0x10000000;
	r4 = ram32(r3) << 18;                    // global crua → PCW[19:18]
	TST(r12 & r2);
	r4 |= r6;
	if (!T) {
		ENTER(0x670, 14);
		r0 = CTX_USERCLIP; r1 = 0xFFFCFFFF & r4; r3 = ram32(r13 + CTX_USERCLIP) << 16;
		r4 = r1 | r3;
	} else
		ENTER(0x67E, 7);
	r0 = CTX_A0; r3 = 0x60000000; r2 = ram32(r13 + CTX_A0);
	TST(r2 & r3);
	r6 = 0xFFFFFF3F & r4;                    // PCW[7:6] limpos
	if (!T) BAIL(0x68C);                     // ctx+0xA0 & 0x60000000: NÃO VISTO

	// ---- MODIFIER, PCW[5:4] COLORTYPE, [3] textura, [1] gouraud, [2] specular, [0] UV16 --
	ENTER(0x698, 3);
	r4 = r6;
	if (!TST(r12 & r5)) {                    // r5 = 0x10
		ENTER(0x69E, 3);
		r0 = ram32(r13 + CTX_MODIFIER);
		if (!TST(r0 & 1)) BAIL(0x6A4);       // Modifier & 1: NÃO VISTO
	}
	ENTER(0x6C0, 3);
	r3 = 0x400000;
	if (!TST(r3 & r12)) {
		ENTER(0x6C6, 9);
		r2 = ram32(r13 + CTX_COLORTYPE); r1 = 0xFFFFFFCF & r4; r2 <<= 4; r4 = r1 | r2;
	} else
		ENTER(0x6D4, 2);
	if (!TST(r8)) {
		ENTER(0x6D8, 10);
		r3 = ram32(r13 + CTX_SHADING); r2 = 0xFFFFFFF7 & r4; r3 = (r3 >> 1) << 3; r4 = r2 | r3;
		ENTER(0x6EC, 10);
		r0 = (ram32(r13 + CTX_SHADING) & 1) << 1; r2 = 0xFFFFFFFD & r4; r4 = r2 | r0;
	} else
		ENTER(0x6FA, 3);
	r3 = ram32(sp + 12);                     // máscara & USESPECULAR
	if (!TST(r3)) {
		ENTER(0x700, 4);
		r0 = CTX_SPECULAR; r3 = ram32(r13 + CTX_SPECULAR);
		if (!TST(r3)) { ENTER(0x708, 3); r3 = 4; r4 |= r3; ENTER(0x744, 3); }
		else          { ENTER(0x740, 5); r1 = 0xFFFFFFFB; r4 &= r1; }
	} else
		ENTER(0x744, 3);
	r3 = ram32(sp + 4);                      // máscara & UVFORMAT
	if (!TST(r3)) {
		ENTER(0x74A, 28);
		r3 = 0xFFFFFFFE & r4; r4 = ram32(r13 + CTX_UVFORMAT) | r3;
	} else
		ENTER(0x752, 24);

	// ---- grava as 4 palavras e volta (T fica = UVFORMAT não mudou) ---------------------
	r3 = 0xFFD00000; r11 &= r3;
	r2 = 0xE78F00FF; r4 &= r2;
	w32(r13 + CTX_ISP, r11);
	w32(r13 + CTX_TSP, r14);
	r2 = ram32(sp + 8);
	w32(r13 + CTX_TCW, r2);
	w32(r13 + CTX_PCW, r4);
	r0 = 0;                                  // KMSTATUS_SUCCESS
	sp += 36;
	pr = ram32(sp); sp += 4;
	r8 = ram32(sp); sp += 4;  r9 = ram32(sp); sp += 4;  r10 = ram32(sp); sp += 4;
	r11 = ram32(sp); sp += 4; r12 = ram32(sp); sp += 4; r13 = ram32(sp); sp += 4;
	r14 = ram32(sp); sp += 4;
	flush();
	return_to(pr);
#undef ENTER
#undef BAIL
#undef TST
#undef CMPEQ
#undef CMPHS
}

} // namespace cabecalho_poligono
