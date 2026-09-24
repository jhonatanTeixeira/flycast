#include "gles.h"
#include <unordered_set>
#include <chrono>

// FC_REND_SPLIT: draw calls issued per frame (every glDraw* below is counted).
// Comma form so it also works as the body of an unbraced if.
u32 g_glDrawCalls;
#define FC_COUNT_DRAW g_glDrawCalls++,

// FC_REND_SPLIT (2026-09-24, KOF Evolution): onde vao os ~14ms do Render.
// Tempos de CPU (a render thread e limitada pela CPU/driver, nao pela GPU).
// idx: 0 setup (inicio do RenderFrame ate o upload), 1 laco de uniforms em
// todos os shaders, 2 upload vert/idx, 3 opacos, 4 punch-through, 5 modvols,
// 6 translucidos (sort+draw), 7 depois do DrawStrips ate o fim.
u64 g_rsUs[12];
// Medicao 2 do render (2026-09-24): quebras de lote por tcw que sao a MESMA
// textura GL (texid) com o resto do estado igual -- lotes que poderiam ser
// fundidos -- e o custo de SetGPState x draw por draw, pelo contador de
// hardware (cntvct_el0, barato; clock_gettime e caro neste kernel).
u32 g_rsBreakSameTex, g_rsBreakTcwOnly;
u64 g_rsStateTicks, g_rsDrawTicks, g_rsTickFreq;
static inline u64 rs_ticks()
{
#if defined(__aarch64__)
	u64 v;
	asm volatile("mrs %0, cntvct_el0" : "=r"(v));
	return v;
#else
	return 0;	// so medido em aarch64
#endif
}
u32 g_rsShaders;
u32 g_rsSingleDraws, g_rsBatchDraws, g_rsProgramSwitches, g_rsTexBinds;
static inline u64 rs_now_us()
{
	return (u64)std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
}

// FC_REND_SPLIT (opt-in, DOA2 2026-09-22): por que o batching (PP_SameGPUState)
// nao funde mais strips. Conta, para cada par adjacente que interrompe uma
// sequencia, qual campo difere. O item 6 do rendering_improvement_plan suspeita
// que a comparacao de 32 bits cheia rejeita pares que SetGPState() trataria
// igual (bits reservados/irrelevantes). So mede; nenhum custo sem FC_REND_SPLIT.
extern bool g_rendSplitEnabled;
u32 g_batchBreakPcw, g_batchBreakIsp, g_batchBreakTcw, g_batchBreakTsp, g_batchBreakTileclip;
u32 g_batchBreakTotal;
// Prêmio de reordenar OPACOS por estado (depth test torna a ordem irrelevante):
// runs de estado por frame (pós-batching) vs chaves de estado DISTINTAS por
// frame. Se distinct << runs, ordenar por estado funde muito mais; ~igual,
// quase toda strip tem estado próprio e não há o que fundir.
u32 g_opaqueRuns;
u32 g_opaqueDistinctSum, g_opaqueFramesCounted;
static std::unordered_set<u64> s_opaqueKeys;
static u32 s_opaqueKeyFrame;
static bool s_opaqueKeyFrameValid;


/*

Drawing and related state management
Takes vertex, textures and renders to the currently set up target
*/

const static u32 CullModes[]= 
{
	GL_NONE, //0    No culling          No culling
	GL_NONE, //1    Cull if Small       Cull if ( |det| < fpu_cull_val )

	GL_FRONT, //2   Cull if Negative    Cull if ( |det| < 0 ) or ( |det| < fpu_cull_val )
	GL_BACK,  //3   Cull if Positive    Cull if ( |det| > 0 ) or ( |det| < fpu_cull_val )
};
const u32 Zfunction[] =
{
	GL_NEVER,       //0 Never
	GL_LESS,        //1 Less
	GL_EQUAL,       //2 Equal
	GL_LEQUAL,      //3 Less Or Equal
	GL_GREATER,     //4 Greater
	GL_NOTEQUAL,    //5 Not Equal
	GL_GEQUAL,      //6 Greater Or Equal
	GL_ALWAYS,      //7 Always
};

/*
0   Zero                  (0, 0, 0, 0)
1   One                   (1, 1, 1, 1)
2   Other Color           (OR, OG, OB, OA)
3   Inverse Other Color   (1-OR, 1-OG, 1-OB, 1-OA)
4   SRC Alpha             (SA, SA, SA, SA)
5   Inverse SRC Alpha     (1-SA, 1-SA, 1-SA, 1-SA)
6   DST Alpha             (DA, DA, DA, DA)
7   Inverse DST Alpha     (1-DA, 1-DA, 1-DA, 1-DA)
*/

const u32 DstBlendGL[] =
{
	GL_ZERO,
	GL_ONE,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA
};

const u32 SrcBlendGL[] =
{
	GL_ZERO,
	GL_ONE,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA
};

extern int screen_width;
extern int screen_height;

PipelineShader* CurrentShader;
extern u32 gcflip;
GLuint vmuTextureId[4]={0,0,0,0};
GLuint lightgunTextureId[4]={0,0,0,0};

void SetCull(u32 CullMode)
{
	if (CullModes[CullMode] == GL_NONE)
		glcache.Disable(GL_CULL_FACE);
	else
	{
		glcache.Enable(GL_CULL_FACE);
		glcache.CullFace(CullModes[CullMode]); //GL_FRONT/GL_BACK, ...
	}
}

static void SetTextureRepeatMode(GLuint dir, u32 clamp, u32 mirror)
{
	if (clamp)
		glcache.TexParameteri(GL_TEXTURE_2D, dir, GL_CLAMP_TO_EDGE);
	else
		glcache.TexParameteri(GL_TEXTURE_2D, dir, mirror ? GL_MIRRORED_REPEAT : GL_REPEAT);
}

static void SetBaseClipping()
{
	if (ShaderUniforms.base_clipping.enabled)
	{
		glcache.Enable(GL_SCISSOR_TEST);
		glcache.Scissor(ShaderUniforms.base_clipping.x, ShaderUniforms.base_clipping.y, ShaderUniforms.base_clipping.width, ShaderUniforms.base_clipping.height);
	}
	else
		glcache.Disable(GL_SCISSOR_TEST);
}

template <u32 Type, bool SortingEnabled>
__forceinline
	void SetGPState(const PolyParam* gp,u32 cflip=0)
{
	if (gp->pcw.Texture && gp->tsp.FilterMode > 1 && Type != ListType_Punch_Through && gp->tcw.MipMapped == 1)
	{
		ShaderUniforms.trilinear_alpha = 0.25 * (gp->tsp.MipMapD & 0x3);
		if (gp->tsp.FilterMode == 2)
			// Trilinear pass A
			ShaderUniforms.trilinear_alpha = 1.0 - ShaderUniforms.trilinear_alpha;
	}
	else
		ShaderUniforms.trilinear_alpha = 1.f;

	bool color_clamp = gp->tsp.ColorClamp && (pvrrc.fog_clamp_min != 0 || pvrrc.fog_clamp_max != 0xffffffff);
	int fog_ctrl = settings.rend.Fog ? gp->tsp.FogCtrl : 2;

	int clip_rect[4] = {};
	TileClipping clipmode = GetTileClip(gp->tileclip, ViewportMatrix, clip_rect);
	// 0 = paleta na CPU, 1 = paleta na GPU nearest, 2 = paleta na GPU bilinear
	int palette = 0;
	if (BaseTextureCacheData::IsGpuHandledPaletted(gp->tsp, gp->tcw))
		palette = gp->tsp.FilterMode + 1;

	CurrentShader = GetProgram(Type == ListType_Punch_Through ? true : false,
								  clipmode == TileClipping::Inside,
								  gp->pcw.Texture,
								  gp->tsp.UseAlpha,
								  gp->tsp.IgnoreTexA,
								  gp->tsp.ShadInstr,
								  gp->pcw.Offset,
								  fog_ctrl,
								  gp->pcw.Gouraud,
								  gp->tcw.PixelFmt == PixelBumpMap,
								  color_clamp,
								  ShaderUniforms.trilinear_alpha != 1.f,
								  palette);

	glcache.UseProgram(CurrentShader->program);
	if (CurrentShader->trilinear_alpha != -1)
		glUniform1f(CurrentShader->trilinear_alpha, ShaderUniforms.trilinear_alpha);
	if (palette)
	{
		if (gp->tcw.PixelFmt == PixelPal4)
			ShaderUniforms.palette_index = gp->tcw.PalSelect << 4;
		else
			ShaderUniforms.palette_index = (gp->tcw.PalSelect >> 4) << 8;
		glUniform1i(CurrentShader->palette_index, ShaderUniforms.palette_index);
	}

	if (clipmode == TileClipping::Inside)
		glUniform4f(CurrentShader->pp_ClipTest, clip_rect[0], clip_rect[1], clip_rect[0] + clip_rect[2], clip_rect[1] + clip_rect[3]);
	if (clipmode == TileClipping::Outside)
	{
		glcache.Enable(GL_SCISSOR_TEST);
		glcache.Scissor(clip_rect[0], clip_rect[1], clip_rect[2], clip_rect[3]);
	}
	else
		SetBaseClipping();

	//This bit control which pixels are affected
	//by modvols
	const u32 stencil=(gp->pcw.Shadow!=0)?0x80:0x0;

	glcache.StencilFunc(GL_ALWAYS,stencil,stencil);

	glcache.BindTexture(GL_TEXTURE_2D, gp->texid == (u64)-1 ? 0 : (GLuint)gp->texid);

	SetTextureRepeatMode(GL_TEXTURE_WRAP_S, gp->tsp.ClampU, gp->tsp.FlipU);
	SetTextureRepeatMode(GL_TEXTURE_WRAP_T, gp->tsp.ClampV, gp->tsp.FlipV);

	//set texture filter mode
	if (gp->tsp.FilterMode == 0 || palette)
	{
		//disable filtering, mipmaps
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
	{
		//bilinear filtering
		//PowerVR supports also trilinear via two passes, but we ignore that for now
		bool mipmapped = gp->tcw.MipMapped != 0 && gp->tcw.ScanOrder == 0 && settings.rend.UseMipmaps;
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmapped ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_TEXTURE_LOD_BIAS
		if (!gl.is_gles && gl.gl_major >= 3 && mipmapped)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, D_Adjust_LoD_Bias[gp->tsp.MipMapD]);
#endif
		if (gl.max_anisotropy > 1.f)
		{
			if (settings.rend.AnisotropicFiltering > 1)
			{
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT,
						std::min((f32)settings.rend.AnisotropicFiltering, gl.max_anisotropy));
				// Set the recommended minification filter for best results
				if (mipmapped)
					glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}
			else
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.f);
		}
	}

	// Apparently punch-through polys support blending, or at least some combinations
	if (Type == ListType_Translucent || Type == ListType_Punch_Through)
	{
		glcache.Enable(GL_BLEND);
		glcache.BlendFunc(SrcBlendGL[gp->tsp.SrcInstr], DstBlendGL[gp->tsp.DstInstr]);
	}
	else
		glcache.Disable(GL_BLEND);

	//set cull mode !
	//cflip is required when exploding triangles for triangle sorting
	//gcflip is global clip flip, needed for when rendering to texture due to mirrored Y direction
	SetCull(gp->isp.CullMode ^ cflip ^ gcflip);

	//set Z mode, only if required
	if (Type == ListType_Punch_Through || (Type == ListType_Translucent && SortingEnabled))
	{
		glcache.DepthFunc(GL_GEQUAL);
	}
	else
	{
		glcache.DepthFunc(Zfunction[gp->isp.DepthMode]);
	}

	if (SortingEnabled && settings.pvr.Emulation.AlphaSortMode == 0)
		glcache.DepthMask(GL_FALSE);
	else
	{
		// Z Write Disable seems to be ignored for punch-through.
		// Fixes Worms World Party, Bust-a-Move 4 and Re-Volt
		if (Type == ListType_Punch_Through)
			glcache.DepthMask(GL_TRUE);
		else
			glcache.DepthMask(!gp->isp.ZWriteDis);
	}
}

#ifndef GL_PRIMITIVE_RESTART_FIXED_INDEX
// Not declared by the GLES2 headers this fork builds against (HAVE_OPENGLES2),
// but the value is a fixed part of the GLES3/GL4 core spec and works fine on
// any GLES3+ context reached at runtime (glEnable/glDisable need no extra
// entry point) -- see docs/tech_debits.md item 4.2.
#define GL_PRIMITIVE_RESTART_FIXED_INDEX 0x8D69
#endif

// Same GPU-relevant state comparison as PP_EQ() in sorter.cpp (duplicated
// locally on purpose, to keep this batching change self-contained/easy to
// revert). Covers every PolyParam field SetGPState() reads; texid isn't
// compared separately because it's derived purely from tsp+tcw.
static inline bool PP_SameGPUState(const PolyParam* pp0, const PolyParam* pp1)
{
	return (pp0->pcw.full & PCW_DRAW_MASK) == (pp1->pcw.full & PCW_DRAW_MASK)
			&& pp0->isp.full == pp1->isp.full
			&& pp0->tcw.full == pp1->tcw.full
			&& pp0->tsp.full == pp1->tsp.full
			&& pp0->tileclip == pp1->tileclip;
}

template <u32 Type, bool SortingEnabled>
static void DrawList(const List<PolyParam>& gply, int first, int count)
{
	PolyParam* params= &gply.head()[first];

	/* We want at least 1 PParam */
	if (count==0)
		return;

	/* set some 'global' modes for all primitives */
	glcache.Enable(GL_STENCIL_TEST);
	glcache.StencilFunc(GL_ALWAYS,0,0);
	glcache.StencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);

	// Batch runs of consecutive strips that share identical GPU state into a
	// single glDrawElements call via GLES3 fixed-index primitive restart,
	// instead of one draw call per strip -- draw call submission overhead on
	// the Mali driver dominates render time in heavy scenes (tech_debits.md
	// item 4.2, ~15ms/25ms measured on Shenmue's snow cutscene). Only taken on
	// confirmed GLES3+ contexts; every other config keeps the original
	// one-draw-per-strip path untouched.
	const bool canBatch = gl.is_gles && gl.gl_major >= 3;
	if (canBatch)
		// Parenthesized to dodge glsm's glEnable(T)->rglEnable(S##T) shadow-state
		// macro (glsm.h): GL_PRIMITIVE_RESTART_FIXED_INDEX has no SGL_* entry in
		// glsm's fixed capability set, so route straight to the real driver call.
		(glEnable)(GL_PRIMITIVE_RESTART_FIXED_INDEX);

	static std::vector<u32> batchIdx;
	const u32 *idx_base = pvrrc.idx.head();

	// Um unico upload de indices por lista (docs/tech_debits.md 4.35). Antes,
	// cada draw em lote subia seu proprio index buffer (glBindBuffer +
	// glBufferData + rebind): no KOF Evolution na chuva, 154 subidas por frame
	// = 1,93ms so disso. Agora uma primeira passada monta os indices de TODOS
	// os draws da lista (simples e em lote, mesma segmentacao por
	// PP_SameGPUState) num array so, sobe uma vez, e a segunda passada so aplica
	// estado e desenha por offset, sem trocar de buffer. FC_NO_ONE_UPLOAD=1
	// volta ao caminho anterior.
	static int oneUpload = -1;
	if (oneUpload == -1)
		oneUpload = getenv("FC_NO_ONE_UPLOAD") != nullptr ? 0 : 1;
	if (canBatch && oneUpload && gl.index_type == GL_UNSIGNED_INT)
	{
		struct Run { PolyParam* pp; u32 offset, count, lo, hi; };
		static std::vector<Run> runs;
		static std::vector<u32> allIdx;
		runs.clear();
		allIdx.clear();
		PolyParam* end = params + count;
		PolyParam* p0 = params;
		while (p0 < end)
		{
			if (p0->count <= 2)
			{
				p0++;
				continue;
			}
			PolyParam* runEnd = p0 + 1;
			while (runEnd < end && runEnd->count > 2 && PP_SameGPUState(runEnd, p0))
				runEnd++;
			if (g_rendSplitEnabled)
			{
				g_opaqueRuns++;
				if (runEnd < end && runEnd->count > 2)
				{
					g_batchBreakTotal++;
					if ((p0->pcw.full & PCW_DRAW_MASK) != (runEnd->pcw.full & PCW_DRAW_MASK)) g_batchBreakPcw++;
					if (p0->isp.full != runEnd->isp.full) g_batchBreakIsp++;
					if (p0->tcw.full != runEnd->tcw.full) g_batchBreakTcw++;
					if (p0->tsp.full != runEnd->tsp.full) g_batchBreakTsp++;
					if (p0->tileclip != runEnd->tileclip) g_batchBreakTileclip++;
				}
				if (runEnd - p0 == 1) g_rsSingleDraws++; else g_rsBatchDraws++;
				if (runEnd < end && runEnd->count > 2 && p0->tcw.full != runEnd->tcw.full)
				{
					const bool restSame = (p0->pcw.full & PCW_DRAW_MASK) == (runEnd->pcw.full & PCW_DRAW_MASK)
							&& p0->isp.full == runEnd->isp.full && p0->tsp.full == runEnd->tsp.full
							&& p0->tileclip == runEnd->tileclip;
					if (restSame)
					{
						g_rsBreakTcwOnly++;
						if (p0->texid == runEnd->texid)
							g_rsBreakSameTex++;
					}
				}
			}
			Run r = { p0, (u32)allIdx.size(), 0, 0xFFFFFFFF, 0 };
			for (PolyParam* p = p0; p < runEnd; p++)
			{
				const u32 *src = idx_base + p->first;
				for (u32 i = 0; i < p->count; i++)
				{
					const u32 v = src[i];
					r.lo = std::min(r.lo, v);
					r.hi = std::max(r.hi, v);
					allIdx.push_back(v);
				}
				if (p + 1 < runEnd)
					allIdx.push_back(0xFFFFFFFF);
			}
			r.count = (u32)allIdx.size() - r.offset;
			runs.push_back(r);
			p0 = runEnd;
		}
		if (!runs.empty())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs2);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, allIdx.size() * sizeof(u32), allIdx.data(), GL_STREAM_DRAW);
			for (const Run& r : runs)
			{
				u64 k0 = g_rendSplitEnabled ? rs_ticks() : 0;
				SetGPState<Type,SortingEnabled>(r.pp);
				u64 k1 = g_rendSplitEnabled ? rs_ticks() : 0;
				const GLvoid *offs = (const GLvoid *)(uintptr_t)(r.offset * sizeof(u32));
				if (glDrawRangeElements_ != nullptr)
					FC_COUNT_DRAW glDrawRangeElements_(GL_TRIANGLE_STRIP, r.lo, r.hi, (GLsizei)r.count, GL_UNSIGNED_INT, offs);
				else
					FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)r.count, GL_UNSIGNED_INT, offs);
				if (g_rendSplitEnabled)
				{
					u64 k2 = rs_ticks();
					g_rsStateTicks += k1 - k0;
					g_rsDrawTicks += k2 - k1;
				}
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs);
		}
		(glDisable)(GL_PRIMITIVE_RESTART_FIXED_INDEX);
		return;
	}

	PolyParam* end = params + count;
	while (params < end)
	{
		if (params->count <= 2) /* this actually happens for some games. No idea why .. */
		{
			params++;
			continue;
		}

		PolyParam* runEnd = params + 1;
		if (canBatch)
		{
			while (runEnd < end && runEnd->count > 2 && PP_SameGPUState(runEnd, params))
				runEnd++;
			if (g_rendSplitEnabled)
			{
				g_opaqueRuns++;
				u64 key = (u64)(params->tcw.full) | ((u64)params->tsp.full << 24)
						| ((u64)(params->isp.full & 0x1F) << 56)
						| ((u64)(params->pcw.full & PCW_DRAW_MASK) << 61)
						| ((u64)params->tileclip << 32);
				if (s_opaqueKeyFrameValid && s_opaqueKeyFrame != FrameCount)
				{
					g_opaqueDistinctSum += (u32)s_opaqueKeys.size();
					g_opaqueFramesCounted++;
					s_opaqueKeys.clear();
				}
				s_opaqueKeyFrame = FrameCount;
				s_opaqueKeyFrameValid = true;
				s_opaqueKeys.insert(key);
			}
			if (runEnd < end && runEnd->count > 2)
			{
				const PolyParam* a = params;
				const PolyParam* b = runEnd;
				g_batchBreakTotal++;
				bool pcwDiff = (a->pcw.full & PCW_DRAW_MASK) != (b->pcw.full & PCW_DRAW_MASK);
				bool ispDiff = a->isp.full != b->isp.full;
				bool tcwDiff = a->tcw.full != b->tcw.full;
				bool tspDiff = a->tsp.full != b->tsp.full;
				bool tileDiff = a->tileclip != b->tileclip;
				if (pcwDiff) g_batchBreakPcw++;
				if (ispDiff) g_batchBreakIsp++;
				if (tcwDiff) g_batchBreakTcw++;
				if (tspDiff) g_batchBreakTsp++;
				if (tileDiff) g_batchBreakTileclip++;
			}
		}

		SetGPState<Type,SortingEnabled>(params);

		if (runEnd - params == 1)
		{
			if (g_rendSplitEnabled) g_rsSingleDraws++;
			if (glDrawRangeElements_ != nullptr)
			{
				const u32 *pi = idx_base + params->first;
				u32 lo = pi[0], hi = pi[0];
				for (u32 i = 1; i < params->count; i++)
				{
					lo = std::min(lo, pi[i]);
					hi = std::max(hi, pi[i]);
				}
				FC_COUNT_DRAW glDrawRangeElements_(GL_TRIANGLE_STRIP, lo, hi, params->count, gl.index_type,
						(GLvoid*)(gl.get_index_size() * params->first));
			}
			else
				FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, params->count, gl.index_type,
						(GLvoid*)(gl.get_index_size() * params->first));
		}
		else
		{
			// Merge this run's strips into one draw call, separating them with
			// the fixed restart index so each strip keeps its own topology
			// (GL starts a fresh strip right after a restart index, exactly
			// like a new, independent glDrawElements call would).
			if (g_rendSplitEnabled) g_rsBatchDraws++;
			batchIdx.clear();
			u32 lo = 0xFFFFFFFF, hi = 0;
			for (PolyParam* p = params; p < runEnd; p++)
			{
				for (u32 i = 0; i < p->count; i++)
				{
					u32 v = idx_base[p->first + i];
					lo = std::min(lo, v);
					hi = std::max(hi, v);
					batchIdx.push_back(v);
				}
				if (p + 1 < runEnd)
					batchIdx.push_back(0xFFFFFFFF);
			}
			u64 rsB = g_rendSplitEnabled ? rs_now_us() : 0;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs2);
			if (gl.index_type == GL_UNSIGNED_SHORT)
			{
				static std::vector<u16> batchIdx16;
				batchIdx16.resize(batchIdx.size());
				for (size_t i = 0; i < batchIdx.size(); i++)
					batchIdx16[i] = (u16)batchIdx[i];
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, batchIdx16.size() * sizeof(u16), batchIdx16.data(), GL_STREAM_DRAW);
			}
			else
			{
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, batchIdx.size() * sizeof(u32), batchIdx.data(), GL_STREAM_DRAW);
			}
			u64 rsD = g_rendSplitEnabled ? rs_now_us() : 0;
			if (g_rendSplitEnabled) g_rsUs[10] += rsD - rsB;
			if (glDrawRangeElements_ != nullptr)
				FC_COUNT_DRAW glDrawRangeElements_(GL_TRIANGLE_STRIP, lo, hi, (GLsizei)batchIdx.size(), gl.index_type, (GLvoid*)0);
			else
				FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)batchIdx.size(), gl.index_type, (GLvoid*)0);
			if (g_rendSplitEnabled) g_rsUs[11] += rs_now_us() - rsD;
			// Re-bind the main index buffer so unrelated draws (and the next
			// DrawList call) keep using the un-batched geometry as before.
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs);
		}

		params = runEnd;
	}

	if (canBatch)
		(glDisable)(GL_PRIMITIVE_RESTART_FIXED_INDEX); // see the glEnable comment above
}

static std::vector<SortTrigDrawParam> pidx_sort;

static void SortTriangles(int first, int count)
{
	std::vector<u32> vidx_sort;
	GenSorted(first, count, pidx_sort, vidx_sort);

	//Upload to GPU if needed
	if (!pidx_sort.empty())
	{
		//Bind and upload sorted index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs2); glCheck();
		if (gl.index_type == GL_UNSIGNED_SHORT)
		{
			static bool overrun;
			static List<u16> short_vidx;
			if (short_vidx.daty != NULL)
				short_vidx.Free();
			short_vidx.Init(vidx_sort.size(), &overrun, NULL);
			for (u32 i = 0; i < vidx_sort.size(); i++)
				*(short_vidx.Append()) = vidx_sort[i];
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, short_vidx.bytes(), short_vidx.head(), GL_STREAM_DRAW);
		}
		else
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, vidx_sort.size() * sizeof(u32), &vidx_sort[0], GL_STREAM_DRAW);
	}
}

void DrawSorted(bool multipass)
{
	//if any drawing commands, draw them
	if (!pidx_sort.empty())
	{
		u32 count=pidx_sort.size();

		{
			//set some 'global' modes for all primitives

			glcache.Enable(GL_STENCIL_TEST);
			glcache.StencilFunc(GL_ALWAYS, 0, 0);
			glcache.StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

			for (u32 p=0; p<count; p++)
			{
				const PolyParam* params = pidx_sort[p].ppid;
				if (pidx_sort[p].count>2) //this actually happens for some games. No idea why ..
				{
					SetGPState<ListType_Translucent, true>(params);
					FC_COUNT_DRAW glDrawElements(GL_TRIANGLES, pidx_sort[p].count, gl.index_type,
						(GLvoid*)(gl.get_index_size() * pidx_sort[p].first));
				}
				params++;
			}

			if (multipass && settings.rend.TranslucentPolygonDepthMask)
			{
				// Write to the depth buffer now. The next render pass might need it. (Cosmic Smash)
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glcache.Disable(GL_BLEND);

				glcache.StencilMask(0);

				// We use the modifier volumes shader because it's fast. We don't need textures, etc.
				glcache.UseProgram(gl.modvol_shader.program);
				glUniform1f(gl.modvol_shader.sp_ShaderColor, 1.f);

				glcache.DepthFunc(GL_GEQUAL);
				glcache.DepthMask(GL_TRUE);

				for (u32 p = 0; p < count; p++)
				{
					const PolyParam* params = pidx_sort[p].ppid;
					if (pidx_sort[p].count > 2 && !params->isp.ZWriteDis) {
						// FIXME no clipping in modvol shader
						//SetTileClip(gp->tileclip,true);

						SetCull(params->isp.CullMode ^ gcflip);

						FC_COUNT_DRAW glDrawElements(GL_TRIANGLES, pidx_sort[p].count, gl.index_type,
							(GLvoid*)(gl.get_index_size() * pidx_sort[p].first));
					}
				}
				glcache.StencilMask(0xFF);
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			}

		}
		// Re-bind the previous index buffer for subsequent render passes
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs);
	}
}

//All pixels are in area 0 by default.
//If inside an 'in' volume, they are in area 1
//if inside an 'out' volume, they are in area 0
/*
	Stencil bits:
		bit 7: mv affected (must be preserved)
		bit 1: current volume state
		but 0: summary result (starts off as 0)

	Lower 2 bits:

	IN volume (logical OR):
	00 -> 00
	01 -> 01
	10 -> 01
	11 -> 01

	Out volume (logical AND):
	00 -> 00
	01 -> 00
	10 -> 00
	11 -> 01
*/
void SetMVS_Mode(ModifierVolumeMode mv_mode, ISP_Modvol ispc)
{
	if (mv_mode == Xor)
	{
		// set states
		glcache.Enable(GL_DEPTH_TEST);
		// write only bit 1
		glcache.StencilMask(2);
		// no stencil testing
		glcache.StencilFunc(GL_ALWAYS, 0, 2);
		// count the number of pixels in front of the Z buffer (xor zpass)
		glcache.StencilOp(GL_KEEP, GL_KEEP, GL_INVERT);

		//Cull mode needs to be set
		SetCull(ispc.CullMode);
	}
	else if (mv_mode == Or)
	{
		// set states
		glcache.Enable(GL_DEPTH_TEST);
		// write only bit 1
		glcache.StencilMask(2);
		// no stencil testing
		glcache.StencilFunc(GL_ALWAYS, 2, 2);
		// Or'ing of all triangles
		glcache.StencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

		// Cull mode needs to be set
		SetCull(ispc.CullMode);
	}
	else
	{
		// Inclusion or Exclusion volume

		// no depth test
		glcache.Disable(GL_DEPTH_TEST);
		// write bits 1:0
		glcache.StencilMask(3);

		if (mv_mode == Inclusion)
		{
			// Inclusion volume
			//res : old : final 
			//0   : 0      : 00
			//0   : 1      : 01
			//1   : 0      : 01
			//1   : 1      : 01
			
			// if (1<=st) st=1; else st=0;
			glcache.StencilFunc(GL_LEQUAL, 1, 3);
			glcache.StencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
		}
		else
		{
			// Exclusion volume
			/*
				I've only seen a single game use it, so i guess it doesn't matter ? (Zombie revenge)
				(actually, i think there was also another, racing game)
			*/

			// The initial value for exclusion volumes is 1 so we need to invert the result before and'ing.
			//res : old : final 
			//0   : 0   : 00
			//0   : 1   : 01
			//1   : 0   : 00
			//1   : 1   : 00

			// if (1 == st) st = 1; else st = 0;
			glcache.StencilFunc(GL_EQUAL, 1, 3);
			glcache.StencilOp(GL_ZERO, GL_ZERO, GL_KEEP);
		}
	}
}

static void SetupMainVBO(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo.geometry);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl.vbo.idxs);

	//setup vertex buffers attrib pointers
	glEnableVertexAttribArray(VERTEX_POS_ARRAY);
	glVertexAttribPointer(VERTEX_POS_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,x));

	glEnableVertexAttribArray(VERTEX_COL_BASE_ARRAY);
	glVertexAttribPointer(VERTEX_COL_BASE_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex,col));

	glEnableVertexAttribArray(VERTEX_COL_OFFS_ARRAY);
	glVertexAttribPointer(VERTEX_COL_OFFS_ARRAY, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex,vtx_spc));

	glEnableVertexAttribArray(VERTEX_UV_ARRAY);
	glVertexAttribPointer(VERTEX_UV_ARRAY, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,u));
}

static void SetupModvolVBO(void)
{
	glBindBuffer(GL_ARRAY_BUFFER, gl.vbo.modvols);

	//setup vertex buffers attrib pointers
	glEnableVertexAttribArray(VERTEX_POS_ARRAY);
	glVertexAttribPointer(VERTEX_POS_ARRAY, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);

	glDisableVertexAttribArray(VERTEX_UV_ARRAY);
	glDisableVertexAttribArray(VERTEX_COL_OFFS_ARRAY);
	glDisableVertexAttribArray(VERTEX_COL_BASE_ARRAY);
}

static void DrawModVols(int first, int count)
{
	/* A bit of explanation:
	 * In theory it works like this: generate a 1-bit stencil for each polygon
	 * volume, and then AND or OR it against the overall 1-bit tile stencil at 
	 * the end of the volume. */

	if (count == 0)
		return;

	SetupModvolVBO();

	glcache.Disable(GL_BLEND);
	SetBaseClipping();

	glcache.UseProgram(gl.modvol_shader.program);
	glUniform1f(gl.modvol_shader.sp_ShaderColor, 1 - FPU_SHAD_SCALE.scale_factor / 256.f);

	glcache.Enable(GL_DEPTH_TEST);
	glcache.DepthMask(GL_FALSE);
	glcache.DepthFunc(GL_GREATER);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	ModifierVolumeParam* params = &pvrrc.global_param_mvo.head()[first];

	int mod_base = -1;

	for (int cmv = 0; cmv < count; cmv++)
	{
		ModifierVolumeParam& param = params[cmv];

		if (param.count == 0)
			continue;

		u32 mv_mode = param.isp.DepthMode;

		if (mod_base == -1)
			mod_base = param.first;

		if (!param.isp.VolumeLast && mv_mode > 0)
			SetMVS_Mode(Or, param.isp);		// OR'ing (open volume or quad)
		else
			SetMVS_Mode(Xor, param.isp);	// XOR'ing (closed volume)
		FC_COUNT_DRAW glDrawArrays(GL_TRIANGLES, param.first * 3, param.count * 3);

		if (mv_mode == 1 || mv_mode == 2)
		{
			// Sum the area
			SetMVS_Mode(mv_mode == 1 ? Inclusion : Exclusion, param.isp);
			FC_COUNT_DRAW glDrawArrays(GL_TRIANGLES, mod_base * 3, (param.first + param.count - mod_base) * 3);
			mod_base = -1;
		}
	}
	//disable culling
	SetCull(0);
	//enable color writes
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

	//black out any stencil with '1'
	glcache.Enable(GL_BLEND);
	glcache.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glcache.Enable(GL_STENCIL_TEST);
	glcache.StencilFunc(GL_EQUAL, 0x81, 0x81); //only pixels that are Modvol enabled, and in area 1

	//clear the stencil result bit
	glcache.StencilMask(0x3);    //write to lsb
	glcache.StencilOp(GL_ZERO, GL_ZERO, GL_ZERO);

	//don't do depth testing
	glcache.Disable(GL_DEPTH_TEST);

	SetupMainVBO();
	FC_COUNT_DRAW glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	//restore states
	glcache.Enable(GL_DEPTH_TEST);
}

void DrawStrips()
{
	SetupMainVBO();
	//Draw the strips !

	//We use sampler 0
	glActiveTexture(GL_TEXTURE0);

	RenderPass previous_pass = {};
	for (int render_pass = 0; render_pass < pvrrc.render_passes.used(); render_pass++)
	{
		const RenderPass& current_pass = pvrrc.render_passes.head()[render_pass];

		DEBUG_LOG(RENDERER, "Render pass %d OP %d PT %d TR %d MV %d", render_pass + 1,
			current_pass.op_count - previous_pass.op_count,
			current_pass.pt_count - previous_pass.pt_count,
			current_pass.tr_count - previous_pass.tr_count,
			current_pass.mvo_count - previous_pass.mvo_count);

		//initial state
		glcache.Enable(GL_DEPTH_TEST);
		glcache.DepthMask(GL_TRUE);

		u64 rs0 = g_rendSplitEnabled ? rs_now_us() : 0;
		//Opaque
		DrawList<ListType_Opaque, false>(pvrrc.global_param_op, 
			previous_pass.op_count, current_pass.op_count - previous_pass.op_count);
		u64 rs1 = g_rendSplitEnabled ? rs_now_us() : 0;
		if (g_rendSplitEnabled) g_rsUs[3] += rs1 - rs0;

		//Alpha tested
		DrawList<ListType_Punch_Through, false>(pvrrc.global_param_pt,
			previous_pass.pt_count, current_pass.pt_count - previous_pass.pt_count);
		u64 rs2 = g_rendSplitEnabled ? rs_now_us() : 0;
		if (g_rendSplitEnabled) g_rsUs[4] += rs2 - rs1;

		// Modifier volumes
		if (gl.stencil_present && settings.rend.ModifierVolumes)
			DrawModVols(previous_pass.mvo_count, current_pass.mvo_count - previous_pass.mvo_count);

		u64 rs3 = g_rendSplitEnabled ? rs_now_us() : 0;
		if (g_rendSplitEnabled) g_rsUs[5] += rs3 - rs2;
		//Alpha blended
		// Frame-budget speedhack v2 (settings.rend.FrameBudgetVblankMultiplier):
		// when active, draw only a FRACTION of this pass's Translucent
		// strips -- sorting (SortPParams) still runs on the FULL range
		// either way, so depth ordering of whatever we do draw stays
		// correct, and DrawList()'s own per-strip state handling is
		// untouched (we're not skipping a block, just iterating less of an
		// already-per-strip loop). See render_reduce_translucent_this_frame
		// in gles.h and docs/tech_debits.md item 5.2 for why v1 (skipping
		// the whole block, including SortTriangles/DrawSorted's per-triangle
		// path) caused real visual corruption instead of just a smaller
		// effect. The per-triangle path (AlphaSortMode==0) is left
		// completely untouched here -- confirmed inactive in this project's
		// config (tech_debits.md item 4.1), so there's no value in adding
		// risk to a dead path.
		{
			int trCount = current_pass.tr_count - previous_pass.tr_count;
			if (current_pass.autosort)
			{
				if (settings.pvr.Emulation.AlphaSortMode == 0)
				{
					SortTriangles(previous_pass.tr_count, trCount);
					DrawSorted(render_pass < pvrrc.render_passes.used() - 1);
				}
				else
				{
					u64 rsS = g_rendSplitEnabled ? rs_now_us() : 0;
					SortPParams(previous_pass.tr_count, trCount);
					if (g_rendSplitEnabled) { g_rsUs[8] += rs_now_us() - rsS; g_rsUs[9] += trCount; }
					int drawCount = render_reduce_translucent_this_frame
						? (int)(trCount * render_translucent_draw_fraction) : trCount;
					DrawList<ListType_Translucent, true>(pvrrc.global_param_tr, previous_pass.tr_count, drawCount);
				}
			}
			else
			{
				int drawCount = render_reduce_translucent_this_frame
					? (int)(trCount * render_translucent_draw_fraction) : trCount;
				DrawList<ListType_Translucent, false>(pvrrc.global_param_tr, previous_pass.tr_count, drawCount);
			}
		}
		if (g_rendSplitEnabled) g_rsUs[6] += rs_now_us() - rs3;

		previous_pass = current_pass;
		}

		vertex_buffer_unmap();
}

static void DrawQuad(GLuint texId, float x, float y, float w, float h, float u0, float v0, float u1, float v1)
{
	struct Vertex vertices[] = {
		{ x,     y + h, 0.1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, u0, v1 },
		{ x,     y,     0.1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, u0, v0 },
		{ x + w, y + h, 0.1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, u1, v1 },
		{ x + w, y,     0.1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, u1, v0 },
	};
	GLushort indices[] = { 0, 1, 2, 1, 3 };

	glcache.Disable(GL_SCISSOR_TEST);
	glcache.Disable(GL_DEPTH_TEST);
	glcache.Disable(GL_STENCIL_TEST);
	glcache.Disable(GL_CULL_FACE);
	glcache.Disable(GL_BLEND);

	ShaderUniforms.trilinear_alpha = 1.0;

	PipelineShader *shader = GetProgram(false, false, true, false, true, 0, false, 2, false, false, false, false, false);
	glcache.UseProgram(shader->program);

	glActiveTexture(GL_TEXTURE0);
	glcache.BindTexture(GL_TEXTURE_2D, texId);

	SetupMainVBO();
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);

	FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, (void *)0);
}

void DrawFramebuffer()
{
	DrawQuad(fbTextureId, 0, 0, 640.f, 480.f, 0, 0, 1, 1);
	glcache.DeleteTextures(1, &fbTextureId);
	fbTextureId = 0;
}

void UpdateVmuTexture(int vmu_screen_number)
{
	s32 x,y ;
	u8 temp_tex_buffer[VMU_SCREEN_HEIGHT*VMU_SCREEN_WIDTH*4];
	u8 *dst = temp_tex_buffer;
	u8 *src = NULL ;
	u8 *origsrc = NULL ;
	u8 vmu_pixel_on_R = vmu_screen_params[vmu_screen_number].vmu_pixel_on_R ;
	u8 vmu_pixel_on_G = vmu_screen_params[vmu_screen_number].vmu_pixel_on_G ;
	u8 vmu_pixel_on_B = vmu_screen_params[vmu_screen_number].vmu_pixel_on_B ;
	u8 vmu_pixel_off_R = vmu_screen_params[vmu_screen_number].vmu_pixel_off_R ;
	u8 vmu_pixel_off_G = vmu_screen_params[vmu_screen_number].vmu_pixel_off_G ;
	u8 vmu_pixel_off_B = vmu_screen_params[vmu_screen_number].vmu_pixel_off_B ;
	u8 vmu_screen_opacity = vmu_screen_params[vmu_screen_number].vmu_screen_opacity ;

	if (vmuTextureId[vmu_screen_number] == 0)
	{
		vmuTextureId[vmu_screen_number] = glcache.GenTexture();
		glcache.BindTexture(GL_TEXTURE_2D, vmuTextureId[vmu_screen_number]);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
		glcache.BindTexture(GL_TEXTURE_2D, vmuTextureId[vmu_screen_number]);


	origsrc = vmu_screen_params[vmu_screen_number].vmu_lcd_screen ;

	if ( origsrc == NULL )
		return ;


	for ( y = VMU_SCREEN_HEIGHT-1 ; y >= 0 ; y--)
	{
		src = origsrc + (y*VMU_SCREEN_WIDTH) ;

		for ( x = 0 ; x < VMU_SCREEN_WIDTH ; x++)
		{
			if ( *src++ > 0 )
			{
				*dst++ = vmu_pixel_on_R ;
				*dst++ = vmu_pixel_on_G ;
				*dst++ = vmu_pixel_on_B ;
				*dst++ = vmu_screen_opacity ;
			}
			else
			{
				*dst++ = vmu_pixel_off_R ;
				*dst++ = vmu_pixel_off_G ;
				*dst++ = vmu_pixel_off_B ;
				*dst++ = vmu_screen_opacity ;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, VMU_SCREEN_WIDTH, VMU_SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_tex_buffer);

	vmu_screen_params[vmu_screen_number].vmu_screen_needs_update = false ;

}

void DrawVmuTexture(u8 vmu_screen_number)
{
	glActiveTexture(GL_TEXTURE0);

	float x=0 ;
	float y=0 ;
	float w=VMU_SCREEN_WIDTH*vmu_screen_params[vmu_screen_number].vmu_screen_size_mult ;
	float h=VMU_SCREEN_HEIGHT*vmu_screen_params[vmu_screen_number].vmu_screen_size_mult ;

	if (vmu_screen_params[vmu_screen_number].vmu_screen_needs_update || vmuTextureId[vmu_screen_number] == 0)
		UpdateVmuTexture(vmu_screen_number) ;

	switch ( vmu_screen_params[vmu_screen_number].vmu_screen_position )
	{
		case UPPER_LEFT :
		{
			x = 0 ;
			y = 0 ;
			break ;
		}
		case UPPER_RIGHT :
		{
			x = 640-w ;
			y = 0 ;
			break ;
		}
		case LOWER_LEFT :
		{
			x = 0 ;
			y = 480-h ;
			break ;
		}
		case LOWER_RIGHT :
		{
			x = 640-w ;
			y = 480-h ;
			break ;
		}
	}

	glcache.BindTexture(GL_TEXTURE_2D, vmuTextureId[vmu_screen_number]);

	glcache.Disable(GL_SCISSOR_TEST);
	glcache.Disable(GL_DEPTH_TEST);
	glcache.Disable(GL_STENCIL_TEST);
	glcache.Disable(GL_CULL_FACE);
	glcache.Enable(GL_BLEND);
	glcache.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SetupMainVBO();
	PipelineShader *shader = GetProgram(0, false, 1, 1, 0, 0, 0, 2, false, false, false, false, false);
	glcache.UseProgram(shader->program);

	{
		struct Vertex vertices[] = {
				{ x,   y+h, 1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 0, 1 },
				{ x,   y,   1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 0, 0 },
				{ x+w, y+h, 1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 1, 1 },
				{ x+w, y,   1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 1, 0 },
		};
		GLushort indices[] = { 0, 1, 2, 1, 3 };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
	}

	FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, (void *)0);
}

void UpdateLightGunTexture(int port)
{
	s32 x,y ;
	u8 temp_tex_buffer[LIGHTGUN_CROSSHAIR_SIZE*LIGHTGUN_CROSSHAIR_SIZE*4];
	u8 *dst = temp_tex_buffer;
	u8 *src = NULL ;

	if (lightgunTextureId[port] == 0)
	{
		lightgunTextureId[port] = glcache.GenTexture();
		glcache.BindTexture(GL_TEXTURE_2D, lightgunTextureId[port]);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else
		glcache.BindTexture(GL_TEXTURE_2D, lightgunTextureId[port]);

	u8* colour = &( lightgun_palette[ lightgun_params[port].colour * 3 ] );

	for ( y = LIGHTGUN_CROSSHAIR_SIZE-1 ; y >= 0 ; y--)
	{
		src = lightgun_img_crosshair + (y*LIGHTGUN_CROSSHAIR_SIZE) ;

		for ( x = 0 ; x < LIGHTGUN_CROSSHAIR_SIZE ; x++)
		{
			if ( src[x] )
			{
				*dst++ = colour[0] ;
				*dst++ = colour[1] ;
				*dst++ = colour[2] ;
				*dst++ = 0xFF ;
			}
			else
			{
				*dst++ = 0 ;
				*dst++ = 0 ;
				*dst++ = 0 ;
				*dst++ = 0 ;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, LIGHTGUN_CROSSHAIR_SIZE, LIGHTGUN_CROSSHAIR_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_tex_buffer);

	lightgun_params[port].dirty = false;
}

void DrawGunCrosshair(u8 port)
{
	if ( lightgun_params[port].offscreen || (lightgun_params[port].colour==0) )
		return;

	glActiveTexture(GL_TEXTURE0);

	float x=0;
	float y=0;
	float w=LIGHTGUN_CROSSHAIR_SIZE;
	float h=LIGHTGUN_CROSSHAIR_SIZE;

	x = lightgun_params[port].x - ( LIGHTGUN_CROSSHAIR_SIZE / 2 );
	y = lightgun_params[port].y - ( LIGHTGUN_CROSSHAIR_SIZE / 2 );

	if ( lightgun_params[port].dirty || lightgunTextureId[port] == 0)
		UpdateLightGunTexture(port);

	glcache.BindTexture(GL_TEXTURE_2D, lightgunTextureId[port]);

	glcache.Disable(GL_SCISSOR_TEST);
	glcache.Disable(GL_DEPTH_TEST);
	glcache.Disable(GL_STENCIL_TEST);
	glcache.Disable(GL_CULL_FACE);
	glcache.Enable(GL_BLEND);
	glcache.BlendFunc(GL_SRC_ALPHA, GL_ONE);

	SetupMainVBO();
	PipelineShader *shader = GetProgram(0, false, 1, 1, 0, 0, 0, 2, false, false, false, false, false);
	glcache.UseProgram(shader->program);

	{
		struct Vertex vertices[] = {
				{ x,   y+h, 1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 0, 1 },
				{ x,   y,   1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 0, 0 },
				{ x+w, y+h, 1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 1, 1 },
				{ x+w, y,   1, { 255, 255, 255, 255 }, { 0, 0, 0, 0 }, 1, 0 },
		};
		GLushort indices[] = { 0, 1, 2, 1, 3 };

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STREAM_DRAW);
	}

	FC_COUNT_DRAW glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, (void *)0);

	glcache.BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
