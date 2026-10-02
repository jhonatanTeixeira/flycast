#include <chrono>	// FC_TA_SPLIT timing
#include <math.h>
#include <algorithm>

#include <libretro.h>

#include "gles.h"

// Split do custo de UploadToGPU: o bind (que passa pelo state cache do
// glcache e pelo wrapper glsm do libretro) versus a chamada de transferencia
// em si. ~25us por upload de uma textura de ~134 bytes nao e banda; isto diz
// se o custo esta na chamada de dados ou no overhead de estado em volta.
extern bool g_taSplitEnabled;
u64 g_texBindUs, g_texCallUs;
static inline u64 tex_now_us()
{
	return (u64)std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::steady_clock::now().time_since_epoch()).count();
}

static bool TexSubImageEnabled()
{
	static int enabled = -1;
	if (enabled == -1)
		enabled = getenv("FC_TEX_SUBIMAGE") != nullptr ? 1 : 0;
	return enabled == 1;
}


#ifndef GL_IMPLEMENTATION_COLOR_READ_TYPE
#define GL_IMPLEMENTATION_COLOR_READ_TYPE 0x8B9A
#endif

#ifndef GL_IMPLEMENTATION_COLOR_READ_FORMAT
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT 0x8B9B
#endif

/*
Textures

Textures are converted to native OpenGL textures
The mapping is done with tcw:tsp -> GL texture. That includes stuff like
filtering/ texture repeat

To save space native formats are used for 1555/565/4444 (only bit shuffling is done)
YUV is converted to 8888
PALs are decoded to their unpaletted format (5551/565/4444/8888 depending on palette type)

Compression
	look into it, but afaik PVRC is not realtime doable
*/

#if FEAT_HAS_SOFTREND
	#include <xmmintrin.h>
#endif

extern u32 decoded_colors[3][65536];
GlTextureCache TexCache;

extern "C" struct retro_hw_render_callback hw_render;

void TextureCacheData::UploadToGPU(int width, int height, u8 *temp_tex_buffer, bool mipmapped, bool mipmapsIncluded)
{
	// FC_TEX_GPU_MORTON: sobe os bytes CRUS twiddled (R8UI); o shader faz o
	// untwiddle Morton + paleta por fragmento (fiel ao PowerVR).
	if (gpu_morton && texID != 0)
	{
		u32 nbytes = w * h / (tcw.PixelFmt == PixelPal4 ? 2 : 1);
		u32 rw = w, rh = (nbytes + rw - 1) / rw;
		u32 avail = (size > 0 && (u64)sa + size <= VRAM_SIZE) ? size : nbytes;
		if (avail > rw * rh) avail = rw * rh;
		if (avail == rw * rh)
			// caso comum: os bytes da VRAM ja tem o tamanho exato -- sobe direto,
			// sem copia intermediaria
			gl_UploadRawTexture(texID, rw, rh, &vram[sa]);
		else
		{
			static thread_local std::vector<u8> rawbuf;
			rawbuf.assign((size_t)rw * rh, 0);
			memcpy(rawbuf.data(), &vram[sa], avail);
			gl_UploadRawTexture(texID, rw, rh, rawbuf.data());
		}
		gl_RegisterRawTexture(texID, w, h, tcw.PixelFmt == PixelPal4);
		// A pagina ja foi protegida no Update() ANTES desta leitura (uma vez so;
		// antes era travada duas vezes neste caminho).
		if (getenv("FC_MORTON_LOG"))
			NOTICE_LOG(RENDERER, "MORTON upload: texid=%llu w=%u h=%u pix=%d sa=%u size=%u nbytes=%u rw=%u rh=%u stride=%d StrideSel=%d Mip=%d",
				(unsigned long long)texID, w, h, tcw.PixelFmt, sa, size, nbytes, rw, rh,
				(tcw.StrideSel ? (TEXT_CONTROL & 31) * 32 : (int)w), tcw.StrideSel, tcw.MipMapped);
		{
			const char *d = getenv("FC_MORTON_DUMP");
			static int dumps = 0;
			if (d && dumps < 6)
			{
				dumps++;
				char fn[128];
				snprintf(fn, sizeof(fn), "%s/raw_%llu.bin", d, (unsigned long long)texID);
				FILE *rf = fopen(fn, "wb");
				if (rf) { fwrite(&vram[sa], 1, avail, rf); fclose(rf); }
				snprintf(fn, sizeof(fn), "%s/vram_%llu.bin", d, (unsigned long long)texID);
				rf = fopen(fn, "wb");
				if (rf) { fwrite(&vram[sa], 1, rw * rh, rf); fclose(rf); }
			}
		}
		return;
	}
	if (texID != 0)
	{
		//upload to OpenGL !
		u64 _b0 = g_taSplitEnabled ? tex_now_us() : 0;
		glcache.BindTexture(GL_TEXTURE_2D, (GLuint)texID);
		if (g_taSplitEnabled)
			g_texBindUs += tex_now_us() - _b0;
		GLuint comps = tex_type == TextureType::_8 ? gl.single_channel_format : GL_RGBA;
		GLuint gltype;
		u32 bytes_per_pixel = 2;
		switch (tex_type)
		{
		case TextureType::_5551:
			gltype = GL_UNSIGNED_SHORT_5_5_5_1;
			break;
		case TextureType::_565:
			gltype = GL_UNSIGNED_SHORT_5_6_5;
			comps = GL_RGB;
			break;
		case TextureType::_4444:
			gltype = GL_UNSIGNED_SHORT_4_4_4_4;
			break;
		case TextureType::_8888:
			bytes_per_pixel = 4;
			gltype = GL_UNSIGNED_BYTE;
			break;
		case TextureType::_8:
			bytes_per_pixel = 1;
			gltype = GL_UNSIGNED_BYTE;
			break;
		default:
			die("Unsupported texture type");
			gltype = 0;
			break;
		}
		// internalformat TEM de ser calculado aqui, depois do switch: o case
		// _565 reatribui `comps` para GL_RGB. Calcular antes pegava GL_RGBA e
		// gerava glTexImage2D(internalformat=GL_RGBA, format=GL_RGB,
		// type=565) -- combinacao invalida, GL_INVALID_OPERATION, textura
		// nunca sobe. Como 565 e o formato sem alpha, o sintoma era
		// exatamente os cenarios de fundo dos jogos 2D sumirem enquanto os
		// sprites continuavam certos. Ver git a2350c567 (regressao) e este fix.
		// So o caso de 1 canal precisa de internalformat diferente do format
		// (GLES3 exige GL_R8 dimensionado quando o format e GL_RED).
		GLuint internalComps = tex_type == TextureType::_8 ? gl.single_channel_internal_format : comps;

		if (mipmapsIncluded)
		{
			int mipmapLevels = 0;
			int dim = width;
			while (dim != 0)
			{
				mipmapLevels++;
				dim >>= 1;
			}
#if !defined(HAVE_OPENGLES2) && !defined(__APPLE__)
			// Open GL 4.2 or GLES 3.0 min
			if (gl.gl_major > 4 || (gl.gl_major == 4 && gl.gl_minor >= 2)
					|| (gl.is_gles && gl.gl_major >= 3))
			{
				GLuint internalFormat;
				switch (tex_type)
				{
				case TextureType::_5551:
					internalFormat = GL_RGB5_A1;
					break;
				case TextureType::_565:
					internalFormat = GL_RGB565;
					break;
				case TextureType::_4444:
					internalFormat = GL_RGBA4;
					break;
				case TextureType::_8888:
					internalFormat = GL_RGBA8;
					break;
				case TextureType::_8:
					internalFormat = comps;
					break;
				default:
					die("Unsupported texture format");
					internalFormat = 0;
					break;
				}
				if (Updates == 1)
				{
					glTexStorage2D(GL_TEXTURE_2D, mipmapLevels, internalFormat, width, height);
					glCheck();
				}
				for (int i = 0; i < mipmapLevels; i++)
				{
					glTexSubImage2D(GL_TEXTURE_2D, mipmapLevels - i - 1, 0, 0, 1 << i, 1 << i, comps, gltype, temp_tex_buffer);
					temp_tex_buffer += (1 << (2 * i)) * bytes_per_pixel;
				}
			}
			else
#endif
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevels - 1);
				for (int i = 0; i < mipmapLevels; i++)
				{
					glTexImage2D(GL_TEXTURE_2D, mipmapLevels - i - 1, comps, 1 << i, 1 << i, 0, comps, gltype, temp_tex_buffer);
					temp_tex_buffer += (1 << (2 * i)) * bytes_per_pixel;
				}
			}
		}
		else
		{
			// FC_TEX_SUBIMAGE (opt-in): glTexImage2D REALOCA o armazenamento a
			// cada chamada -- o driver descarta a alocacao antiga e cria outra,
			// podendo sincronizar se a textura ainda estiver em uso por um draw
			// pendente. Medido no Metal Slug 6: 166 mil uploads em 30s custando
			// 15,49s (~93us cada, 35ms por frame) com apenas 21,2MB de dados no
			// total -- ou seja, custo de chamada, nao de largura de banda.
			//
			// Quando formato e dimensoes nao mudaram, glTexSubImage2D atualiza
			// no lugar sem realocar. E exatamente o padrao que o caminho
			// mipmapped logo acima ja usa (glTexStorage2D uma vez +
			// glTexSubImage2D); o caminho nao-mipmapped ficou sem ele.
			u64 _c0 = g_taSplitEnabled ? tex_now_us() : 0;
			if (TexSubImageEnabled()
					&& gl_w == (GLuint)width && gl_h == (GLuint)height
					&& gl_comps == comps && gl_type == gltype)
			{
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, comps, gltype, temp_tex_buffer);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0,internalComps, width, height, 0, comps, gltype, temp_tex_buffer);
				gl_w = width; gl_h = height; gl_comps = comps; gl_type = gltype;
			}
			if (g_taSplitEnabled)
				g_texCallUs += tex_now_us() - _c0;
			if (mipmapped)
				glGenerateMipmap(GL_TEXTURE_2D);
	}
		glCheck();
	}
	else {
		#if FEAT_HAS_SOFTREND
			/*
			if (tex_type == TextureType::_565)
				tex_type = 0;
			else if (tex_type == TextureType::_5551)
				tex_type = 1;
			else if (tex_type == TextureType::_4444)
				tex_type = 2;
			*/
			u16 *tex_data = (u16 *)temp_tex_buffer;
			if (pData) {
				_mm_free(pData);
			}

			pData = (u16*)_mm_malloc(w * h * 16, 16);
			for (int y = 0; y < h; y++) {
				for (int x = 0; x < w; x++) {
					u32* data = (u32*)&pData[(x + y*w) * 8];

					data[0] = decoded_colors[tex_type][tex_data[(x + 1) % w + (y + 1) % h * w]];
					data[1] = decoded_colors[tex_type][tex_data[(x + 0) % w + (y + 1) % h * w]];
					data[2] = decoded_colors[tex_type][tex_data[(x + 1) % w + (y + 0) % h * w]];
					data[3] = decoded_colors[tex_type][tex_data[(x + 0) % w + (y + 0) % h * w]];
				}
			}
		#else
			die("Soft rend disabled, invalid code path");
		#endif
	}
}
	
bool TextureCacheData::Delete()
{
	if (!BaseTextureCacheData::Delete())
		return false;

	if (texID) {
		gl_UnregisterRawTexture(texID);
		glcache.DeleteTextures(1, &texID);
	}
	
	return true;
}

void BindRTT(u32 addy, u32 fbw, u32 fbh, u32 channels, u32 fmt)
{
	if (gl.rtt.fbo)
      glDeleteFramebuffers(1,&gl.rtt.fbo);
	if (gl.rtt.tex)
      glcache.DeleteTextures(1,&gl.rtt.tex);
	if (gl.rtt.depthb)
      glDeleteRenderbuffers(1,&gl.rtt.depthb);

	gl.rtt.TexAddr=addy>>3;

	// Find the smallest power of two texture that fits the viewport
   u32 fbh2 = 2;
   while (fbh2 < fbh)
      fbh2 *= 2;
   u32 fbw2 = 2;
   while (fbw2 < fbw)
      fbw2 *= 2;

   if (settings.rend.RenderToTextureUpscale > 1 && !settings.rend.RenderToTextureBuffer)
	{
		fbw *= settings.rend.RenderToTextureUpscale;
		fbh *= settings.rend.RenderToTextureUpscale;
		fbw2 *= settings.rend.RenderToTextureUpscale;
		fbh2 *= settings.rend.RenderToTextureUpscale;
	}

	/* Get the currently bound frame buffer object. On most platforms this just gives 0. */

	/* Generate and bind a render buffer which will become a depth buffer shared between our two FBOs */
	glGenRenderbuffers(1, &gl.rtt.depthb);
	glBindRenderbuffer(RARCH_GL_RENDERBUFFER, gl.rtt.depthb);

	/*
		Currently it is unknown to GL that we want our new render buffer to be a depth buffer.
		glRenderbufferStorage will fix this and in this case will allocate a depth buffer
		m_i32TexSize by m_i32TexSize.
	*/

	glRenderbufferStorage(RARCH_GL_RENDERBUFFER, RARCH_GL_DEPTH24_STENCIL8, fbw2, fbh2);

	/* Create a texture for rendering to */
	gl.rtt.tex = glcache.GenTexture();
	glcache.BindTexture(GL_TEXTURE_2D, gl.rtt.tex);

	glTexImage2D(GL_TEXTURE_2D, 0, channels, fbw2, fbh2, 0, channels, fmt, 0);

	/* Create the object that will allow us to render to the aforementioned texture */
	glGenFramebuffers(1, &gl.rtt.fbo);
	glBindFramebuffer(RARCH_GL_FRAMEBUFFER, gl.rtt.fbo);

	/* Attach the texture to the FBO */
	glFramebufferTexture2D(RARCH_GL_FRAMEBUFFER, RARCH_GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl.rtt.tex, 0);

	// Attach the depth buffer we created earlier to our FBO.
#if defined(HAVE_OPENGLES2) || defined(HAVE_OPENGLES1) || defined(OSX_PPC)
	glFramebufferRenderbuffer(RARCH_GL_FRAMEBUFFER, RARCH_GL_DEPTH_ATTACHMENT,
         RARCH_GL_RENDERBUFFER, gl.rtt.depthb);
   glFramebufferRenderbuffer(RARCH_GL_FRAMEBUFFER, RARCH_GL_STENCIL_ATTACHMENT,
         RARCH_GL_RENDERBUFFER, gl.rtt.depthb);
#else
   glFramebufferRenderbuffer(RARCH_GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
         RARCH_GL_RENDERBUFFER, gl.rtt.depthb);
#endif

	/* Check that our FBO creation was successful */
	GLuint uStatus = glCheckFramebufferStatus(RARCH_GL_FRAMEBUFFER);

	verify(uStatus == RARCH_GL_FRAMEBUFFER_COMPLETE);

   glViewport(0, 0, fbw, fbh);		// TODO CLIP_X/Y min?
}

void ReadRTTBuffer() {
	u32 w = pvrrc.fb_X_CLIP.max - pvrrc.fb_X_CLIP.min + 1;
	u32 h = pvrrc.fb_Y_CLIP.max - pvrrc.fb_Y_CLIP.min + 1;

	u32 stride = FB_W_LINESTRIDE.stride * 8;
	if (stride == 0)
		stride = w * 2;
	else if (w * 2 > stride) {
    	// Happens for Virtua Tennis
    	w = stride / 2;
    }
	u32 size = w * h * 2;

	const u8 fb_packmode = FB_W_CTRL.fb_packmode;

	if (settings.rend.RenderToTextureBuffer)
	{
		u32 tex_addr = gl.rtt.TexAddr << 3;

		// Remove all vram locks before calling glReadPixels
		// (deadlock on rpi)
		u32 page_tex_addr = tex_addr & PAGE_MASK;
		u32 page_size = size + tex_addr - page_tex_addr;
		page_size = ((page_size - 1) / PAGE_SIZE + 1) * PAGE_SIZE;
		for (u32 page = page_tex_addr; page < page_tex_addr + page_size; page += PAGE_SIZE)
			VramLockedWriteOffset(page);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		u16 *dst = (u16 *)&vram[tex_addr];

		GLint color_fmt, color_type;
		glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &color_fmt);
		glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &color_type);

		if (fb_packmode == 1 && stride == w * 2 && color_fmt == GL_RGB && color_type == GL_UNSIGNED_SHORT_5_6_5)
		{
			// Can be read directly into vram
			glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, dst);
		}
		else
		{
			PixelBuffer<u32> tmp_buf;
			tmp_buf.init(w, h);

			u8 *p = (u8 *)tmp_buf.data();
			glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, p);

			WriteTextureToVRam(w, h, p, dst);
		}
	}
	else
	{
		//memset(&vram[fb_rtt.TexAddr << 3], '\0', size);
	}

    //dumpRtTexture(fb_rtt.TexAddr, w, h);
    
    if (w > 1024 || h > 1024 || settings.rend.RenderToTextureBuffer) {
    	glcache.DeleteTextures(1, &gl.rtt.tex);
    }
    else
    {
    	// TexAddr : gl.rtt.TexAddr, Reserved : 0, StrideSel : 0, ScanOrder : 1
    	TCW tcw = { { gl.rtt.TexAddr, 0, 0, 1 } };
    	switch (fb_packmode) {
    	case 0:
    	case 3:
    		tcw.PixelFmt = Pixel1555;
    		break;
    	case 1:
    		tcw.PixelFmt = Pixel565;
    		break;
    	case 2:
    		tcw.PixelFmt = Pixel4444;
    		break;
    	}
    	TSP tsp = { 0 };
    	for (tsp.TexU = 0; tsp.TexU <= 7 && (8u << tsp.TexU) < w; tsp.TexU++);
    	for (tsp.TexV = 0; tsp.TexV <= 7 && (8u << tsp.TexV) < h; tsp.TexV++);

    	TextureCacheData *texture_data = TexCache.getTextureCacheData(tsp, tcw);
    	if (texture_data->texID != 0)
    		glcache.DeleteTextures(1, &texture_data->texID);
    	else
    		texture_data->Create();
    	texture_data->texID = gl.rtt.tex;
    	texture_data->dirty = 0;
      libCore_vramlock_Lock(texture_data->sa_tex, texture_data->sa + texture_data->size - 1, texture_data);
    }
    gl.rtt.tex = 0;

	if (gl.rtt.fbo) { glDeleteFramebuffers(1,&gl.rtt.fbo); gl.rtt.fbo = 0; }
	if (gl.rtt.depthb) { glDeleteRenderbuffers(1,&gl.rtt.depthb); gl.rtt.depthb = 0; }

   glBindFramebuffer(RARCH_GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
}

static int TexCacheLookups;
static int TexCacheHits;
// FC_TA_SPLIT exposes these: 558 GetTexture calls/frame costing 74.8us each
// (Metal Slug 6) can only be cache misses re-uploading every frame, or hits
// that are pathologically slow. The hit/miss split tells which.
int g_texLookups, g_texHits; u64 g_texUpdateUs;
static float LastTexCacheStats;


u64 gl_GetTexture(TSP tsp, TCW tcw)
{
   TexCacheLookups++;

	//lookup texture
   TextureCacheData* tf = TexCache.getTextureCacheData(tsp, tcw);

   g_texLookups++;
   if (tf->texID == 0)
   {
		tf->Create();
		tf->texID = glcache.GenTexture();
		gl_UnregisterRawTexture(tf->texID);	// nome GL reciclado: nao herdar registro velho
		tf->gl_w = tf->gl_h = tf->gl_comps = tf->gl_type = 0;	// alocacao nova
	}

	//update if needed
	if (tf->NeedsUpdate())
	{
		u64 _tu = (u64)std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now().time_since_epoch()).count();
		tf->Update();
		g_texUpdateUs += (u64)std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now().time_since_epoch()).count() - _tu;
	}
   else
   {
      if (tf->IsCustomTextureAvailable())
      {
      	gl_UnregisterRawTexture(tf->texID);
      	glcache.DeleteTextures(1, &tf->texID);
      	tf->texID = glcache.GenTexture();
      	gl_UnregisterRawTexture(tf->texID);
      	tf->gl_w = tf->gl_h = tf->gl_comps = tf->gl_type = 0;	// alocacao nova
      	tf->CheckCustomTexture();
      }
      TexCacheHits++;
      g_texHits++;
   }

	// Return gl texture
	return tf->texID;
}


// FC_TEX_GPU_MORTON: sobe os bytes CRUS (twiddled) como R8UI. O shader faz o
// untwiddle Morton + paleta por fragmento. rw x rh = layout linear dos bytes.
// FC_TA_SPLIT: custo do upload cru do Morton, separado em bind/parametros e envio
u64 g_rawBindUs, g_rawCallUs;
u32 g_rawUploads, g_rawSubUploads;
// dimensoes da ultima alocacao R8 de cada textura crua (para reaproveitar)
static std::unordered_map<GLuint, u64> g_rawAlloc;
void gl_UploadRawTexture(u64 texID, u32 rw, u32 rh, const u8 *data)
{
	// Unidade 0 explicita e bind PELO glcache: o glcache cacheia por textura
	// ignorando a unidade ativa, entao so e seguro com a unidade 0 ativa -- e
	// assim o cache continua coerente (bind direto o deixava desatualizado).
	u64 t0 = g_taSplitEnabled ? tex_now_us() : 0;
	glActiveTexture(GL_TEXTURE0);
	glcache.BindTexture(GL_TEXTURE_2D, (GLuint)texID);
	const u64 dims = ((u64)rw << 32) | rh;
	auto it = g_rawAlloc.find((GLuint)texID);
	const bool same = it != g_rawAlloc.end() && it->second == dims;
	if (!same)
	{
		// parametros so na alocacao: na Mali, mexer em parametro de textura a
		// cada upload forca revalidacao (o upload cru custava 2,7x o normal)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	u64 t1 = g_taSplitEnabled ? tex_now_us() : 0;
	// rw = largura logica (>= 8, potencia de 2): o alinhamento padrao de 4 serve.
	// GL_ALPHA (0x1906), nao GL_R8: neste driver (Mali r13p0) o formato legado
	// de 1 canal e o caminho rapido -- o GL_R8 custou +56% por chamada no
	// FC_TEX_R8 (rendering_improvement_plan.md) e o upload cru em R8 saia
	// ~160 us por chamada contra ~35 us do caminho normal (GL_ALPHA). O shader
	// le o canal .a. Sempre glTexImage2D (realocar): o glTexSubImage2D nao
	// ajudou neste driver (FC_TEX_SUBIMAGE -4%) e aqui piorou.
	glTexImage2D(GL_TEXTURE_2D, 0, 0x1906, rw, rh, 0, 0x1906, GL_UNSIGNED_BYTE, data);
	if (same)
		g_rawSubUploads++;	// mesma dimensao da alocacao anterior
	g_rawAlloc[(GLuint)texID] = dims;
	g_rawUploads++;
	if (g_taSplitEnabled)
	{
		g_rawBindUs += t1 - t0;
		g_rawCallUs += tex_now_us() - t1;
	}
	glCheck();
}

static std::unordered_map<GLuint, TexRawInfo> g_rawTextures;
void gl_RegisterRawTexture(u64 texID, u32 rw, u32 rh, bool pal4)
{
	g_rawTextures[(GLuint)texID] = { rw, rh, pal4 };
}
const TexRawInfo* gl_GetRawInfo(u64 texID)
{
	auto it = g_rawTextures.find((GLuint)texID);
	return it == g_rawTextures.end() ? nullptr : &it->second;
}
void gl_UnregisterRawTexture(u64 texID)
{
	g_rawAlloc.erase((GLuint)texID);
	g_rawTextures.erase((GLuint)texID);
}

GLuint fbTextureId;

void RenderFramebuffer()
{
	if (FB_R_SIZE.fb_x_size == 0 || FB_R_SIZE.fb_y_size == 0)
		return;

	PixelBuffer<u32> pb;
	int width;
	int height;
	ReadFramebuffer(pb, width, height);
	
	if (fbTextureId == 0)
		fbTextureId = glcache.GenTexture();
	
	glcache.BindTexture(GL_TEXTURE_2D, fbTextureId);
	
	//set texture repeat mode
	glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glcache.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pb.data());
}

#if 0
/* currently not needed, but perhaps for soft rendering */
void render_vmu_screen(u8* screen_data, u32 width, u32 height, u8 vmu_screen_to_display)
{
	u8 *dst = screen_data;
	u8 *src = NULL ;
	u32 line_size = width*4 ;
	u32 start_offset ;
	u32 x,y ;

	src = vmu_screen_params[vmu_screen_to_display].vmu_lcd_screen ;

	if ( src == NULL )
		return ;

	switch ( vmu_screen_params[vmu_screen_to_display].vmu_screen_position )
	{
		case UPPER_LEFT :
		{
			start_offset = 0 ;
			break ;
		}
		case UPPER_RIGHT :
		{
			start_offset = line_size - (VMU_SCREEN_WIDTH*vmu_screen_params[vmu_screen_to_display].vmu_screen_size_mult*4) ;
			break ;
		}
		case LOWER_LEFT :
		{
			start_offset = line_size*(height - VMU_SCREEN_HEIGHT) ;
			break ;
		}
		case LOWER_RIGHT :
		{
			start_offset = line_size*(height - VMU_SCREEN_HEIGHT) + (line_size - (VMU_SCREEN_WIDTH*vmu_screen_params[vmu_screen_to_display].vmu_screen_size_mult*4));
			break ;
		}
	}


	for ( y = 0 ; y < VMU_SCREEN_HEIGHT ; y++)
	{
		dst = screen_data + start_offset + (y*line_size);
		for ( x = 0 ; x < VMU_SCREEN_WIDTH ; x++)
		{
			if ( *src++ > 0 )
			{
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_on_R ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_on_G ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_on_B ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_screen_opacity ;
			}
			else
			{
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_off_R ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_off_G ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_pixel_off_B ;
				*dst++ = vmu_screen_params[vmu_screen_to_display].vmu_screen_opacity ;
			}
		}
	}
}
#endif
