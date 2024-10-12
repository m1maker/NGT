#include "Graphics.h"
#include "ngt.h"
#include <angelscript.h>
#include <SDL3/SDL.h>
#include <string>

using namespace std;





class CScriptRenderer;
class CScriptTexture {
public:
	SDL_Texture* texture;
	void AddRef()const {
		ref += 1;
	}

	void Release()const {
		if (--ref < 1) {
			SDL_DestroyTexture(texture);
			delete this;
		}
	}



	CScriptRenderer* get_renderer();
	bool get_size(float& w, float& h) {
		return SDL_GetTextureSize(texture, &w, &h);
	}
	bool set_color_mode(unsigned char r, unsigned char g, unsigned char b) {
		return SDL_SetTextureColorMod(texture, r, g, b);
	}
	bool set_color_mode(float r, float g, float b) {
		return SDL_SetTextureColorModFloat(texture, r, g, b);
	}
	bool get_color_mode(unsigned char& r, unsigned char& g, unsigned char& b) {
		return SDL_GetTextureColorMod(texture, &r, &g, &b);
	}
	bool get_color_mode(float& r, float& g, float& b) {
		return SDL_GetTextureColorModFloat(texture, &r, &g, &b);
	}
	bool set_alpha_mod(unsigned char alpha) {
		return SDL_SetTextureAlphaMod(texture, alpha);
	}
	bool set_alpha_mod(float alpha) {
		return SDL_SetTextureAlphaModFloat(texture, alpha);
	}
	bool get_alpha_mod(unsigned char& alpha) {
		return SDL_GetTextureAlphaMod(texture, &alpha);
	}
	bool get_alpha_mod(float& alpha) {
		return SDL_GetTextureAlphaModFloat(texture, &alpha);
	}
	bool update(const SDL_Rect* rect, void* pixels, int pitch) {
		return SDL_UpdateTexture(texture, rect, pixels, pitch);
	}

	bool set_blend_mode(SDL_BlendMode mode) {
		return SDL_SetTextureBlendMode(texture, mode);
	}
	SDL_BlendMode get_blend_mode() {
		SDL_BlendMode mode;
		SDL_GetTextureBlendMode(texture, &mode);
		return mode;
	}
	bool set_scale_mode(SDL_ScaleMode mode) {
		return SDL_SetTextureScaleMode(texture, mode);
	}
	SDL_ScaleMode get_scale_mode() {
		SDL_ScaleMode mode;
		SDL_GetTextureScaleMode(texture, &mode);
		return mode;
	}
private:
	mutable int ref = 0;
};

class CScriptSurface {
public:
	SDL_Surface* surface;
	void AddRef()const {
		ref += 1;
	}

	void Release()const {
		if (--ref < 1) {
			SDL_DestroySurface(surface);
			delete this;
		}
	}
	CScriptSurface(int width, int height, SDL_PixelFormat format) :ref(1) {
		surface = SDL_CreateSurface(width, height, format);
	}
	CScriptSurface(int width, int height, SDL_PixelFormat format, void* pixels, int pitch) :ref(1) {
		surface = SDL_CreateSurfaceFrom(width, height, format, pixels, pitch);
	}
	CScriptSurface(const std::string& filename) :ref(1) {
		surface = SDL_LoadBMP(filename.c_str());
	}

	bool set_colorspace(SDL_Colorspace color) {
		return SDL_SetSurfaceColorspace(surface, color);
	}
	SDL_Colorspace get_colorspace() {
		return SDL_GetSurfaceColorspace(surface);
	}
	bool add_alternate_image(CScriptSurface* s) {
		if (s == nullptr)return false;
		return SDL_AddSurfaceAlternateImage(surface, s->surface);
	}
	bool has_alternate_images() {
		return SDL_SurfaceHasAlternateImages(surface);
	}
	void remove_alternate_images() {
		SDL_RemoveSurfaceAlternateImages(surface);
	}
	bool lock() {
		return SDL_LockSurface(surface);
	}
	void unlock() {
		SDL_UnlockSurface(surface);
	}
	bool save_to_file(const std::string& filename) {
		return SDL_SaveBMP(surface, filename.c_str());
	}
	bool set_rle(bool rle) {
		return SDL_SetSurfaceRLE(surface, rle);
	}
	bool get_rle() {
		return SDL_SurfaceHasRLE(surface);
	}
	bool set_color_key(bool enabled, uint32_t key) {
		return SDL_SetSurfaceColorKey(surface, enabled, key);
	}
	bool has_color_key() {
		return SDL_SurfaceHasColorKey(surface);
	}
	bool set_color_mod(unsigned char r, unsigned char g, unsigned char b) {
		return SDL_SetSurfaceColorMod(surface, r, g, b);
	}
	bool get_color_mode(unsigned char& r, unsigned char& g, unsigned char& b) {
		return SDL_GetSurfaceColorMod(surface, &r, &g, &b);
	}
	bool set_alpha_mod(unsigned char alpha) {
		return SDL_SetSurfaceAlphaMod(surface, alpha);
	}
	unsigned char get_alpha_mod() {
		unsigned char mod;
		SDL_GetSurfaceAlphaMod(surface, &mod);
		return mod;
	}
	bool set_blend_mode(SDL_BlendMode mode) {
		return SDL_SetSurfaceBlendMode(surface, mode);
	}
	SDL_BlendMode get_blend_mode() {
		SDL_BlendMode mode;
		SDL_GetSurfaceBlendMode(surface, &mode);
		return mode;
	}
	bool set_clip_rect(const SDL_Rect* rect) {
		return SDL_SetSurfaceClipRect(surface, rect);
	}
	SDL_Rect* get_clip_rect() {
		SDL_Rect* rect = new SDL_Rect;
		SDL_GetSurfaceClipRect(surface, rect);
		return rect;
	}
	bool flip(SDL_FlipMode flip) {
		return SDL_FlipSurface(surface, flip);
	}
	bool clear(float r, float g, float b, float a) {
		return SDL_ClearSurface(surface, r, g, b, a);
	}
	bool fill_rect(const SDL_Rect* rect, uint32_t color) {
		return SDL_FillSurfaceRect(surface, rect, color);
	}
	bool blit(const SDL_Rect* srcrect, CScriptSurface* dst, const SDL_Rect* dstrect) {
		return SDL_BlitSurface(surface, srcrect, dst->surface, dstrect);
	}
	bool blit(const SDL_Rect* srcrect, CScriptSurface* dst, const SDL_Rect* dstrect, SDL_ScaleMode scaleMode) {
		return SDL_BlitSurfaceScaled(surface, srcrect, dst->surface, dstrect, scaleMode);
	}
	bool blit(const SDL_Rect* srcrect, int left_width, int right_width, int top_height, int bottom_height, float scale, SDL_ScaleMode scaleMode, CScriptSurface* dst, const SDL_Rect* dstrect) {
		return SDL_BlitSurface9Grid(surface, srcrect, left_width, right_width, top_height, bottom_height, scale, scaleMode, dst->surface, dstrect);
	}

	bool blit_unchecked(const SDL_Rect* srcrect, CScriptSurface* dst, const SDL_Rect* dstrect) {
		return SDL_BlitSurfaceUnchecked(surface, srcrect, dst->surface, dstrect);
	}
	bool blit_unchecked(const SDL_Rect* srcrect, CScriptSurface* dst, const SDL_Rect* dstrect, SDL_ScaleMode scaleMode) {
		return SDL_BlitSurfaceUncheckedScaled(surface, srcrect, dst->surface, dstrect, scaleMode);
	}
	bool blit_tiled(const SDL_Rect* srcrect, CScriptSurface* dst, const SDL_Rect* dstrect) {
		return SDL_BlitSurfaceTiled(surface, srcrect, dst->surface, dstrect);
	}

	bool blit_tiled(const SDL_Rect* srcrect, float scale, SDL_ScaleMode scaleMode, CScriptSurface* dst, const SDL_Rect* dstrect) {
		return SDL_BlitSurfaceTiledWithScale(surface, srcrect, scale, scaleMode, dst->surface, dstrect);
	}

	uint32_t map(unsigned char r, unsigned char g, unsigned char b) {
		return SDL_MapSurfaceRGB(surface, r, g, b);
	}
	uint32_t map(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		return SDL_MapSurfaceRGBA(surface, r, g, b, a);
	}
	bool read_pixel(int x, int y, unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) {
		return SDL_ReadSurfacePixel(surface, x, y, &r, &g, &b, &a);
	}
	bool read_pixel(int x, int y, float& r, float& g, float& b, float& a) {
		return SDL_ReadSurfacePixelFloat(surface, x, y, &r, &g, &b, &a);
	}
	bool write_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		return SDL_WriteSurfacePixel(surface, x, y, r, g, b, a);
	}
	bool write_pixel(int x, int y, float r, float g, float b, float a) {
		return SDL_WriteSurfacePixelFloat(surface, x, y, r, g, b, a);
	}

private:
	mutable int ref = 0;
};


class CScriptRenderer {
public:
	SDL_Renderer* renderer;
	void AddRef()const {
		ref += 1;
	}

	void Release()const {
		if (--ref < 1) {
			SDL_DestroyRenderer(renderer);
			delete this;
		}
	}


	std::string get_name() {
		return std::string(SDL_GetRendererName(renderer));
	}
	bool get_output_size(int& w, int& h) {
		return SDL_GetRenderOutputSize(renderer, &w, &h);
	}
	bool get_current_output_size(int& w, int& h) {
		return SDL_GetCurrentRenderOutputSize(renderer, &w, &h);
	}
	CScriptTexture* create_texture(SDL_PixelFormat format, SDL_TextureAccess access, int w, int h) {
		CScriptTexture* t = new CScriptTexture;
		t->texture = SDL_CreateTexture(renderer, format, access, w, h);
		t->AddRef();
		return t;
	}
	CScriptTexture* create_texture(CScriptSurface* surface) {
		CScriptTexture* t = new CScriptTexture;
		t->texture = SDL_CreateTextureFromSurface(renderer, surface->surface);
		t->AddRef();
		return t;
	}

	bool set_target(CScriptTexture* texture) {
		if (texture == nullptr || texture->texture == nullptr)return false;
		return SDL_SetRenderTarget(renderer, texture->texture);
	}
	CScriptTexture* get_target() {
		CScriptTexture* t = new CScriptTexture;
		t->texture = SDL_GetRenderTarget(renderer);
		t->AddRef();
		return t;
	}
	bool set_logical_presentation(int w, int h, SDL_RendererLogicalPresentation mode, SDL_ScaleMode scale_mode) {
		return SDL_SetRenderLogicalPresentation(renderer, w, h, mode, scale_mode);
	}
	bool get_logical_presentation(int& w, int& h, SDL_RendererLogicalPresentation& mode, SDL_ScaleMode& scale_mode) {
		return SDL_GetRenderLogicalPresentation(renderer, &w, &h, &mode, &scale_mode);
	}
	bool set_render_scale(float x, float y) {
		return SDL_SetRenderScale(renderer, x, y);
	}
	bool get_render_scale(float& x, float& y) {
		return SDL_GetRenderScale(renderer, &x, &y);
	}
	bool set_draw_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		return SDL_SetRenderDrawColor(renderer, r, g, b, a);
	}
	bool set_draw_color(float r, float g, float b, float a) {
		return SDL_SetRenderDrawColorFloat(renderer, r, g, b, a);
	}
	bool get_draw_color(unsigned char& r, unsigned char& g, unsigned char& b, unsigned char& a) {
		return SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	}
	bool get_draw_color(float& r, float& g, float& b, float& a) {
		return SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);
	}

	bool set_color_scale(float scale) {
		return SDL_SetRenderColorScale(renderer, scale);
	}
	float get_color_scale() {
		float scale;
		SDL_GetRenderColorScale(renderer, &scale);
		return scale;
	}
	bool set_draw_blend_mode(SDL_BlendMode mode) {
		return SDL_SetRenderDrawBlendMode(renderer, mode);
	}
	SDL_BlendMode get_draw_blend_mode() {
		SDL_BlendMode mode;
		SDL_GetRenderDrawBlendMode(renderer, &mode);
		return mode;
	}
	void clear() {
		SDL_RenderClear(renderer);
	}
	bool render_point(float x, float y) {
		return SDL_RenderPoint(renderer, x, y);
	}
	bool render_line(float x1, float y1, float x2, float y2) {
		return SDL_RenderLine(renderer, x1, y1, x2, y2);
	}
	bool render_rect(const SDL_FRect* rect) {
		return SDL_RenderRect(renderer, rect);
	}
	bool render_fill_rect(const SDL_FRect* rect) {
		return SDL_RenderFillRect(renderer, rect);
	}
	bool render_texture(CScriptTexture* texture, const SDL_FRect* srcrect, const SDL_FRect* dstrect) {
		if (texture == nullptr)return false;
		return SDL_RenderTexture(renderer, texture->texture, srcrect, dstrect);
	}
	bool render_texture(CScriptTexture* texture, const SDL_FRect* srcrect, float scale, const SDL_FRect* dstrect) {
		if (texture == nullptr)return false;
		return SDL_RenderTextureTiled(renderer, texture->texture, srcrect, scale, dstrect);
	}
	bool render_texture(CScriptTexture* texture, const SDL_FRect* srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, const SDL_FRect* dstrect) {
		if (texture == nullptr)return false;
		return SDL_RenderTexture9Grid(renderer, texture->texture, srcrect, left_width, right_width, top_height, bottom_height, scale, dstrect);
	}

	bool flush() {
		return SDL_FlushRenderer(renderer);
	}

	bool set_viewport(const SDL_Rect* rect) {
		return SDL_SetRenderViewport(renderer, rect);
	}
	SDL_Rect* get_viewport() {
		SDL_Rect* rect = new SDL_Rect;
		SDL_GetRenderViewport(renderer, rect);
		return rect;
	}
	SDL_Rect* get_render_safe_area() {
		SDL_Rect* rect = new SDL_Rect;
		SDL_GetRenderSafeArea(renderer, rect);
		return rect;
	}
	void set_render_clip_rect(const SDL_Rect* rect) {
		SDL_SetRenderClipRect(renderer, rect);
	}
	SDL_Rect* get_render_clip_rect() {
		SDL_Rect* rect = new SDL_Rect;
		SDL_GetRenderClipRect(renderer, rect);
		return rect;
	}
	bool render_clip_enabled() {
		return SDL_RenderClipEnabled(renderer);
	}

private:
	mutable int ref = 0;
};

CScriptRenderer* CScriptTexture::get_renderer() {
	CScriptRenderer* r = new CScriptRenderer;
	r->renderer = SDL_GetRendererFromTexture(texture);
	r->AddRef();
	return r;
}

std::string get_render_driver(int index) {
	return std::string(SDL_GetRenderDriver(index));
}


CScriptRenderer* request_renderer() {
	SDL_Renderer* rend = get_window_renderer();
	if (rend == nullptr)return nullptr;
	CScriptRenderer* r = new CScriptRenderer;
	r->renderer = rend;
	r->AddRef();
	return r;
}

CScriptRenderer* RendererFactory() {
	if (get_window_renderer() == nullptr)show_window("");
	return request_renderer();
}

CScriptTexture* TextureFactory() {
	return nullptr;
}
SDL_Rect* RectFactory() {
	return new SDL_Rect;
}
SDL_FRect* FRectFactory() {
	return new SDL_FRect;
}
CScriptSurface* SurfaceFactory(int width, int height, SDL_PixelFormat format) {
	return new CScriptSurface(width, height, format);
}
CScriptSurface* SurfaceFactory(int width, int height, SDL_PixelFormat format, void* pixels, int pitch) {
	return new CScriptSurface(width, height, format, pixels, pitch);
}
CScriptSurface* SurfaceFactory(const std::string& filename) {
	return new CScriptSurface(filename);
}

void RegisterScriptGraphics(asIScriptEngine* engine) {
	engine->RegisterEnum("textureaccess");
	engine->RegisterEnumValue("textureaccess", "TEXTUREACCESS_STATIC", SDL_TEXTUREACCESS_STATIC);
	engine->RegisterEnumValue("textureaccess", "TEXTUREACCESS_STREAMING", SDL_TEXTUREACCESS_STREAMING);
	engine->RegisterEnumValue("textureaccess", "TEXTUREACCESS_TARGET", SDL_TEXTUREACCESS_TARGET);

	engine->RegisterEnum("rendererlogicalpresentation");
	engine->RegisterEnumValue("rendererlogicalpresentation", "LOGICAL_PRESENTATION_DISABLED", SDL_LOGICAL_PRESENTATION_DISABLED);
	engine->RegisterEnumValue("rendererlogicalpresentation", "LOGICAL_PRESENTATION_STRETCH", SDL_LOGICAL_PRESENTATION_STRETCH);
	engine->RegisterEnumValue("rendererlogicalpresentation", "LOGICAL_PRESENTATION_LETTERBOX", SDL_LOGICAL_PRESENTATION_LETTERBOX);
	engine->RegisterEnumValue("rendererlogicalpresentation", "LOGICAL_PRESENTATION_OVERSCAN", SDL_LOGICAL_PRESENTATION_OVERSCAN);
	engine->RegisterEnumValue("rendererlogicalpresentation", "LOGICAL_PRESENTATION_INTEGER_SCALE", SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);

	engine->RegisterEnum("scalemode");
	engine->RegisterEnumValue("scalemode", "SCALEMODE_NEAREST", SDL_SCALEMODE_NEAREST);
	engine->RegisterEnumValue("scalemode", "SCALEMODE_LINEAR", SDL_SCALEMODE_LINEAR);

	engine->RegisterEnum("blendmode");
	engine->RegisterEnumValue("blendmode", "BLENDMODE_NONE", SDL_BLENDMODE_NONE);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_BLEND", SDL_BLENDMODE_BLEND);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_BLEND_PREMULTIPLIED", SDL_BLENDMODE_BLEND_PREMULTIPLIED);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_ADD", SDL_BLENDMODE_ADD);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_ADD_PREMULTIPLIED", SDL_BLENDMODE_ADD_PREMULTIPLIED);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_MOD", SDL_BLENDMODE_MOD);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_MUL", SDL_BLENDMODE_MUL);
	engine->RegisterEnumValue("blendmode", "BLENDMODE_INVALID", SDL_BLENDMODE_INVALID);

	engine->RegisterEnum("flipmode");
	engine->RegisterEnumValue("flipmode", "FLIP_NONE", SDL_FLIP_NONE);
	engine->RegisterEnumValue("flipmode", "FLIP_HORIZONTAL", SDL_FLIP_HORIZONTAL);
	engine->RegisterEnumValue("flipmode", "FLIP_VERTICAL", SDL_FLIP_VERTICAL);

	engine->RegisterEnum("pixeltype");
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_UNKNOWN", SDL_PIXELTYPE_UNKNOWN);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_INDEX1", SDL_PIXELTYPE_INDEX1);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_INDEX4", SDL_PIXELTYPE_INDEX4);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_INDEX8", SDL_PIXELTYPE_INDEX8);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_PACKED8", SDL_PIXELTYPE_PACKED8);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_PACKED16", SDL_PIXELTYPE_PACKED16);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_PACKED32", SDL_PIXELTYPE_PACKED32);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_ARRAYU8", SDL_PIXELTYPE_ARRAYU8);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_ARRAYU16", SDL_PIXELTYPE_ARRAYU16);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_ARRAYU32", SDL_PIXELTYPE_ARRAYU32);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_ARRAYF16", SDL_PIXELTYPE_ARRAYF16);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_ARRAYF32", SDL_PIXELTYPE_ARRAYF32);
	engine->RegisterEnumValue("pixeltype", "PIXELTYPE_INDEX2", SDL_PIXELTYPE_INDEX2);

	engine->RegisterEnum("bitmaporder");
	engine->RegisterEnumValue("bitmaporder", "BITMAPORDER_NONE", SDL_BITMAPORDER_NONE);
	engine->RegisterEnumValue("bitmaporder", "BITMAPORDER_4321", SDL_BITMAPORDER_4321);
	engine->RegisterEnumValue("bitmaporder", "BITMAPORDER_1234", SDL_BITMAPORDER_1234);

	engine->RegisterEnum("packedorder");
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_NONE", SDL_PACKEDORDER_NONE);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_XRGB", SDL_PACKEDORDER_XRGB);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_RGBX", SDL_PACKEDORDER_RGBX);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_ARGB", SDL_PACKEDORDER_ARGB);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_RGBA", SDL_PACKEDORDER_RGBA);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_XBGR", SDL_PACKEDORDER_XBGR);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_BGRX", SDL_PACKEDORDER_BGRX);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_ABGR", SDL_PACKEDORDER_ABGR);
	engine->RegisterEnumValue("packedorder", "PACKEDORDER_BGRA", SDL_PACKEDORDER_BGRA);

	engine->RegisterEnum("arrayorder");
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_NONE", SDL_ARRAYORDER_NONE);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_RGB", SDL_ARRAYORDER_RGB);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_RGBA", SDL_ARRAYORDER_RGBA);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_ARGB", SDL_ARRAYORDER_ARGB);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_BGR", SDL_ARRAYORDER_BGR);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_BGRA", SDL_ARRAYORDER_BGRA);
	engine->RegisterEnumValue("arrayorder", "ARRAYORDER_ABGR", SDL_ARRAYORDER_ABGR);

	engine->RegisterEnum("packedlayout");
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_NONE", SDL_PACKEDLAYOUT_NONE);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_332", SDL_PACKEDLAYOUT_332);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_4444", SDL_PACKEDLAYOUT_4444);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_1555", SDL_PACKEDLAYOUT_1555);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_5551", SDL_PACKEDLAYOUT_5551);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_565", SDL_PACKEDLAYOUT_565);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_8888", SDL_PACKEDLAYOUT_8888);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_2101010", SDL_PACKEDLAYOUT_2101010);
	engine->RegisterEnumValue("packedlayout", "PACKEDLAYOUT_1010102", SDL_PACKEDLAYOUT_1010102);

	engine->RegisterEnum("pixelformat");
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_UNKNOWN", SDL_PIXELFORMAT_UNKNOWN);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX1LSB", SDL_PIXELFORMAT_INDEX1LSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX1MSB", SDL_PIXELFORMAT_INDEX1MSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX2LSB", SDL_PIXELFORMAT_INDEX2LSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX2MSB", SDL_PIXELFORMAT_INDEX2MSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX4LSB", SDL_PIXELFORMAT_INDEX4LSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX4MSB", SDL_PIXELFORMAT_INDEX4MSB);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_INDEX8", SDL_PIXELFORMAT_INDEX8);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XRGB4444", SDL_PIXELFORMAT_XRGB4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XBGR4444", SDL_PIXELFORMAT_XBGR4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XRGB1555", SDL_PIXELFORMAT_XRGB1555);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XBGR1555", SDL_PIXELFORMAT_XBGR1555);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB4444", SDL_PIXELFORMAT_ARGB4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA4444", SDL_PIXELFORMAT_RGBA4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR4444", SDL_PIXELFORMAT_ABGR4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA4444", SDL_PIXELFORMAT_BGRA4444);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB1555", SDL_PIXELFORMAT_ARGB1555);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA5551", SDL_PIXELFORMAT_RGBA5551);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR1555", SDL_PIXELFORMAT_ABGR1555);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA5551", SDL_PIXELFORMAT_BGRA5551);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGB565", SDL_PIXELFORMAT_RGB565);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGR565", SDL_PIXELFORMAT_BGR565);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGB24", SDL_PIXELFORMAT_RGB24);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGR24", SDL_PIXELFORMAT_BGR24);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XRGB8888", SDL_PIXELFORMAT_XRGB8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBX8888", SDL_PIXELFORMAT_RGBX8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XBGR8888", SDL_PIXELFORMAT_XBGR8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRX8888", SDL_PIXELFORMAT_BGRX8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB8888", SDL_PIXELFORMAT_ARGB8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA8888", SDL_PIXELFORMAT_RGBA8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR8888", SDL_PIXELFORMAT_ABGR8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA8888", SDL_PIXELFORMAT_BGRA8888);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XRGB2101010", SDL_PIXELFORMAT_XRGB2101010);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_XBGR2101010", SDL_PIXELFORMAT_XBGR2101010);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB2101010", SDL_PIXELFORMAT_ARGB2101010);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR2101010", SDL_PIXELFORMAT_ABGR2101010);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGB48", SDL_PIXELFORMAT_RGB48);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGR48", SDL_PIXELFORMAT_BGR48);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA64", SDL_PIXELFORMAT_RGBA64);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB64", SDL_PIXELFORMAT_ARGB64);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA64", SDL_PIXELFORMAT_BGRA64);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR64", SDL_PIXELFORMAT_ABGR64);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGB48_FLOAT", SDL_PIXELFORMAT_RGB48_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGR48_FLOAT", SDL_PIXELFORMAT_BGR48_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA64_FLOAT", SDL_PIXELFORMAT_RGBA64_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB64_FLOAT", SDL_PIXELFORMAT_ARGB64_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA64_FLOAT", SDL_PIXELFORMAT_BGRA64_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR64_FLOAT", SDL_PIXELFORMAT_ABGR64_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGB96_FLOAT", SDL_PIXELFORMAT_RGB96_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGR96_FLOAT", SDL_PIXELFORMAT_BGR96_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_RGBA128_FLOAT", SDL_PIXELFORMAT_RGBA128_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ARGB128_FLOAT", SDL_PIXELFORMAT_ARGB128_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_BGRA128_FLOAT", SDL_PIXELFORMAT_BGRA128_FLOAT);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_ABGR128_FLOAT", SDL_PIXELFORMAT_ABGR128_FLOAT);

	// Add video texture formats
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_YV12", SDL_PIXELFORMAT_YV12);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_IYUV", SDL_PIXELFORMAT_IYUV);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_YUY2", SDL_PIXELFORMAT_YUY2);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_UYVY", SDL_PIXELFORMAT_UYVY);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_YVYU", SDL_PIXELFORMAT_YVYU);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_NV12", SDL_PIXELFORMAT_NV12);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_NV21", SDL_PIXELFORMAT_NV21);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_P010", SDL_PIXELFORMAT_P010);
	engine->RegisterEnumValue("pixelformat", "PIXELFORMAT_EXTERNAL_OES", SDL_PIXELFORMAT_EXTERNAL_OES);

	engine->RegisterEnum("colortype");
	engine->RegisterEnumValue("colortype", "COLOR_TYPE_UNKNOWN", SDL_COLOR_TYPE_UNKNOWN);
	engine->RegisterEnumValue("colortype", "COLOR_TYPE_RGB", SDL_COLOR_TYPE_RGB);
	engine->RegisterEnumValue("colortype", "COLOR_TYPE_YCBCR", SDL_COLOR_TYPE_YCBCR);

	engine->RegisterEnum("colorrange");
	engine->RegisterEnumValue("colorrange", "COLOR_RANGE_UNKNOWN", SDL_COLOR_RANGE_UNKNOWN);
	engine->RegisterEnumValue("colorrange", "COLOR_RANGE_LIMITED", SDL_COLOR_RANGE_LIMITED);
	engine->RegisterEnumValue("colorrange", "COLOR_RANGE_FULL", SDL_COLOR_RANGE_FULL);

	engine->RegisterEnum("colorprimaries");
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_UNKNOWN", SDL_COLOR_PRIMARIES_UNKNOWN);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_BT709", SDL_COLOR_PRIMARIES_BT709);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_UNSPECIFIED", SDL_COLOR_PRIMARIES_UNSPECIFIED);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_BT470M", SDL_COLOR_PRIMARIES_BT470M);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_BT470BG", SDL_COLOR_PRIMARIES_BT470BG);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_BT601", SDL_COLOR_PRIMARIES_BT601);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_SMPTE240", SDL_COLOR_PRIMARIES_SMPTE240);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_GENERIC_FILM", SDL_COLOR_PRIMARIES_GENERIC_FILM);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_BT2020", SDL_COLOR_PRIMARIES_BT2020);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_XYZ", SDL_COLOR_PRIMARIES_XYZ);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_SMPTE431", SDL_COLOR_PRIMARIES_SMPTE431);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_SMPTE432", SDL_COLOR_PRIMARIES_SMPTE432);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_EBU3213", SDL_COLOR_PRIMARIES_EBU3213);
	engine->RegisterEnumValue("colorprimaries", "PRIMARIES_CUSTOM", SDL_COLOR_PRIMARIES_CUSTOM);

	engine->RegisterEnum("transfercharacteristics");
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_UNKNOWN", SDL_TRANSFER_CHARACTERISTICS_UNKNOWN);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_BT709", SDL_TRANSFER_CHARACTERISTICS_BT709);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_UNSPECIFIED", SDL_TRANSFER_CHARACTERISTICS_UNSPECIFIED);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_GAMMA22", SDL_TRANSFER_CHARACTERISTICS_GAMMA22);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_GAMMA28", SDL_TRANSFER_CHARACTERISTICS_GAMMA28);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_BT601", SDL_TRANSFER_CHARACTERISTICS_BT601);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_SMPTE240", SDL_TRANSFER_CHARACTERISTICS_SMPTE240);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_LINEAR", SDL_TRANSFER_CHARACTERISTICS_LINEAR);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_LOG100", SDL_TRANSFER_CHARACTERISTICS_LOG100);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_LOG100_SQRT10", SDL_TRANSFER_CHARACTERISTICS_LOG100_SQRT10);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_IEC61966", SDL_TRANSFER_CHARACTERISTICS_IEC61966);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_BT1361", SDL_TRANSFER_CHARACTERISTICS_BT1361);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_SRGB", SDL_TRANSFER_CHARACTERISTICS_SRGB);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_BT2020_10BIT", SDL_TRANSFER_CHARACTERISTICS_BT2020_10BIT);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_BT2020_12BIT", SDL_TRANSFER_CHARACTERISTICS_BT2020_12BIT);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_PQ", SDL_TRANSFER_CHARACTERISTICS_PQ);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_SMPTE428", SDL_TRANSFER_CHARACTERISTICS_SMPTE428);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_HLG", SDL_TRANSFER_CHARACTERISTICS_HLG);
	engine->RegisterEnumValue("transfercharacteristics", "TRANSFER_CHARACTERISTICS_CUSTOM", SDL_TRANSFER_CHARACTERISTICS_CUSTOM);

	engine->RegisterEnum("matrixcoefficients");
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_IDENTITY", SDL_MATRIX_COEFFICIENTS_IDENTITY);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_BT709", SDL_MATRIX_COEFFICIENTS_BT709);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_UNSPECIFIED", SDL_MATRIX_COEFFICIENTS_UNSPECIFIED);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_FCC", SDL_MATRIX_COEFFICIENTS_FCC);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_BT470BG", SDL_MATRIX_COEFFICIENTS_BT470BG);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_BT601", SDL_MATRIX_COEFFICIENTS_BT601);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_SMPTE240", SDL_MATRIX_COEFFICIENTS_SMPTE240);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_YCGCO", SDL_MATRIX_COEFFICIENTS_YCGCO);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_BT2020_NCL", SDL_MATRIX_COEFFICIENTS_BT2020_NCL);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_BT2020_CL", SDL_MATRIX_COEFFICIENTS_BT2020_CL);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_SMPTE2085", SDL_MATRIX_COEFFICIENTS_SMPTE2085);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_CHROMA_DERIVED_NCL", SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_NCL);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_CHROMA_DERIVED_CL", SDL_MATRIX_COEFFICIENTS_CHROMA_DERIVED_CL);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_ICTCP", SDL_MATRIX_COEFFICIENTS_ICTCP);
	engine->RegisterEnumValue("matrixcoefficients", "MATRIX_COEFFICIENTS_CUSTOM", SDL_MATRIX_COEFFICIENTS_CUSTOM);

	engine->RegisterEnum("chromalocation");
	engine->RegisterEnumValue("chromalocation", "CHROMA_LOCATION_NONE", SDL_CHROMA_LOCATION_NONE);
	engine->RegisterEnumValue("chromalocation", "CHROMA_LOCATION_LEFT", SDL_CHROMA_LOCATION_LEFT);
	engine->RegisterEnumValue("chromalocation", "CHROMA_LOCATION_CENTER", SDL_CHROMA_LOCATION_CENTER);
	engine->RegisterEnumValue("chromalocation", "CHROMA_LOCATION_TOPLEFT", SDL_CHROMA_LOCATION_TOPLEFT);

	engine->RegisterObjectType("rect", sizeof(SDL_Rect), asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectBehaviour("rect", asBEHAVE_FACTORY, "rect@ g()", asFUNCTION(RectFactory), asCALL_CDECL);

	engine->RegisterObjectProperty("rect", "int x", asOFFSET(SDL_Rect, x));
	engine->RegisterObjectProperty("rect", "int y", asOFFSET(SDL_Rect, y));
	engine->RegisterObjectProperty("rect", "int w", asOFFSET(SDL_Rect, w));
	engine->RegisterObjectProperty("rect", "int h", asOFFSET(SDL_Rect, h));

	engine->RegisterObjectType("frect", sizeof(SDL_FRect), asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectBehaviour("frect", asBEHAVE_FACTORY, "frect@ g()", asFUNCTION(FRectFactory), asCALL_CDECL);
	engine->RegisterObjectProperty("frect", "float x", asOFFSET(SDL_FRect, x));
	engine->RegisterObjectProperty("frect", "float y", asOFFSET(SDL_FRect, y));
	engine->RegisterObjectProperty("frect", "float w", asOFFSET(SDL_FRect, w));
	engine->RegisterObjectProperty("frect", "float h", asOFFSET(SDL_FRect, h));


	engine->RegisterObjectType("texture", sizeof(CScriptTexture), asOBJ_REF);
	engine->RegisterObjectType("renderer", sizeof(CScriptRenderer), asOBJ_REF);

	engine->RegisterObjectBehaviour("texture", asBEHAVE_FACTORY, "texture@ g()", asFUNCTION(TextureFactory), asCALL_CDECL);

	engine->RegisterObjectBehaviour("texture", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptTexture, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("texture", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptTexture, Release), asCALL_THISCALL);

	engine->RegisterObjectMethod("texture", "renderer@ get_renderer() const property", asMETHOD(CScriptTexture, get_renderer), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool get_size(float&out w, float&out h)", asMETHOD(CScriptTexture, get_size), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool set_color_mode(uchar r, uchar g, uchar b)", asMETHODPR(CScriptTexture, set_color_mode, (unsigned char, unsigned char, unsigned char), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool set_color_mode(float r, float g, float b)", asMETHODPR(CScriptTexture, set_color_mode, (float, float, float), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool get_color_mode(uchar&out r, uchar&out g, uchar&out b)", asMETHODPR(CScriptTexture, get_color_mode, (unsigned char&, unsigned char&, unsigned char&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool get_color_mode(float&out r, float&out g, float&out b)", asMETHODPR(CScriptTexture, get_color_mode, (float&, float&, float&), bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("texture", "bool set_alpha_mod(uchar a)", asMETHODPR(CScriptTexture, set_alpha_mod, (unsigned char), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool set_alpha_mod(float a)", asMETHODPR(CScriptTexture, set_alpha_mod, (float), bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("texture", "bool get_alpha_mod(uchar&out a)", asMETHODPR(CScriptTexture, get_alpha_mod, (unsigned char&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool get_alpha_mod(float&out a)", asMETHODPR(CScriptTexture, get_alpha_mod, (float&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "bool update(rect@ r, uint64 pixels, int pitch)", asMETHOD(CScriptTexture, update), asCALL_THISCALL);

	engine->RegisterObjectMethod("texture", "void set_blend_mode(blendmode mode) const property", asMETHOD(CScriptTexture, set_blend_mode), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "blendmode get_blend_mode() const property", asMETHOD(CScriptTexture, get_blend_mode), asCALL_THISCALL);
	engine->RegisterObjectMethod("texture", "void set_scale_mode(scalemode mode) const property", asMETHOD(CScriptTexture, set_scale_mode), asCALL_THISCALL);
	engine->RegisterObjectType("surface", sizeof(CScriptSurface), asOBJ_REF);

	engine->RegisterObjectBehaviour("surface", asBEHAVE_FACTORY, "surface@ g(int width, int height, pixelformat format)", asFUNCTIONPR(SurfaceFactory, (int, int, SDL_PixelFormat), CScriptSurface*), asCALL_CDECL);
	engine->RegisterObjectBehaviour("surface", asBEHAVE_FACTORY, "surface@ g(int width, int height, pixelformat format, uint64 pixels, int pitch)", asFUNCTIONPR(SurfaceFactory, (int, int, SDL_PixelFormat, void*, int), CScriptSurface*), asCALL_CDECL);
	engine->RegisterObjectBehaviour("surface", asBEHAVE_FACTORY, "surface@ g(const string&in filename)", asFUNCTIONPR(SurfaceFactory, (const std::string&), CScriptSurface*), asCALL_CDECL);

	engine->RegisterObjectBehaviour("surface", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptSurface, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("surface", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptSurface, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "void set_colorspace(int colorspace) const property", asMETHOD(CScriptSurface, set_colorspace), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "int get_colorspace() const property", asMETHOD(CScriptSurface, get_colorspace), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool add_alternate_image(surface@ s)", asMETHOD(CScriptSurface, add_alternate_image), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool has_alternate_images()", asMETHOD(CScriptSurface, has_alternate_images), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "void remove_alternate_images()", asMETHOD(CScriptSurface, remove_alternate_images), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool lock()", asMETHOD(CScriptSurface, lock), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "void unlock()", asMETHOD(CScriptSurface, unlock), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool save_to_file(const string&in filename)", asMETHOD(CScriptSurface, save_to_file), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "void set_rle(bool rle) const property", asMETHOD(CScriptSurface, set_rle), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool get_rle() const property", asMETHOD(CScriptSurface, get_rle), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool set_color_key(bool enabled, uint32 key)", asMETHOD(CScriptSurface, set_color_key), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool has_color_key()", asMETHOD(CScriptSurface, has_color_key), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool set_color_mod(uchar r, uchar g, uchar b)", asMETHOD(CScriptSurface, set_color_mod), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "bool get_color_mod(uchar&out r, uchar&out g, uchar&out b)", asMETHOD(CScriptSurface, get_color_mode), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "void set_alpha_mod(uchar alpha) const property", asMETHOD(CScriptSurface, set_alpha_mod), asCALL_THISCALL);
	engine->RegisterObjectMethod("surface", "uchar get_alpha_mod() const property", asMETHOD(CScriptSurface, get_alpha_mod), asCALL_THISCALL);


	engine->RegisterObjectBehaviour("renderer", asBEHAVE_FACTORY, "renderer@ g()", asFUNCTION(RendererFactory), asCALL_CDECL);

	engine->RegisterObjectBehaviour("renderer", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptRenderer, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("renderer", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptRenderer, Release), asCALL_THISCALL);

	engine->RegisterObjectMethod("renderer", "string get_name() const property", asMETHOD(CScriptRenderer, get_name), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_output_size(int&out w, int&out h)", asMETHOD(CScriptRenderer, get_output_size), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_current_output_size(int&out w, int&out h)", asMETHOD(CScriptRenderer, get_current_output_size), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "texture@ create_texture(pixelformat format, textureaccess access, int w, int h)", asMETHODPR(CScriptRenderer, create_texture, (SDL_PixelFormat, SDL_TextureAccess, int, int), CScriptTexture*), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "texture@ create_texture(surface@ s)", asMETHODPR(CScriptRenderer, create_texture, (CScriptSurface*), CScriptTexture*), asCALL_THISCALL);

	engine->RegisterObjectMethod("renderer", "void set_target(texture@ target) const property", asMETHOD(CScriptRenderer, set_target), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "texture@ get_target() const property", asMETHOD(CScriptRenderer, get_target), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool set_logical_presentation(int w, int h, rendererlogicalpresentation mode, scalemode scale_mode)", asMETHOD(CScriptRenderer, set_logical_presentation), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_logical_presentation(int&out w, int&out h, rendererlogicalpresentation&out mode, scalemode&out scale_mode)", asMETHOD(CScriptRenderer, get_logical_presentation), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool set_render_scale(float x, float y)", asMETHOD(CScriptRenderer, set_render_scale), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_render_scale(float&out x, float&out y)", asMETHOD(CScriptRenderer, get_render_scale), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool set_draw_color(uchar r, uchar g, uchar b, uchar a)", asMETHODPR(CScriptRenderer, set_draw_color, (unsigned char, unsigned char, unsigned char, unsigned char), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool set_draw_color(float r, float g, float b, float a)", asMETHODPR(CScriptRenderer, set_draw_color, (float, float, float, float), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_draw_color(uchar&out r, uchar&out g, uchar&out b, uchar&out a)", asMETHODPR(CScriptRenderer, get_draw_color, (unsigned char&, unsigned char&, unsigned char&, unsigned char&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_draw_color(float&out r, float&out g, float&out b, float&out a)", asMETHODPR(CScriptRenderer, get_draw_color, (float&, float&, float&, float&), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "void set_color_scale(float scale) const property", asMETHOD(CScriptRenderer, set_color_scale), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "float get_color_scale() const property", asMETHOD(CScriptRenderer, get_color_scale), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "void set_draw_blend_mode(blendmode mode) const property", asMETHOD(CScriptRenderer, set_draw_blend_mode), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "blendmode get_draw_blend_mode() const property", asMETHOD(CScriptRenderer, get_draw_blend_mode), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "void clear()", asMETHOD(CScriptRenderer, clear), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_point(float x, float y)", asMETHOD(CScriptRenderer, render_point), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_line(float x1, float y1, float x2, float y2)", asMETHOD(CScriptRenderer, render_line), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_rect(frect@ r)", asMETHOD(CScriptRenderer, render_rect), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_fill_rect(frect@ r)", asMETHOD(CScriptRenderer, render_fill_rect), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_texture(texture@ t, frect@ srcrect, frect@ dstrect)", asMETHODPR(CScriptRenderer, render_texture, (CScriptTexture*, const SDL_FRect*, const SDL_FRect*), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_texture(texture@ t, frect@ srcrect, float scale, frect@ dstrect)", asMETHODPR(CScriptRenderer, render_texture, (CScriptTexture*, const SDL_FRect*, float, const SDL_FRect*), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool render_texture(texture@ t, frect@ srcrect, float left_width, float right_width, float top_height, float bottom_height, float scale, frect@ dstrect)", asMETHODPR(CScriptRenderer, render_texture, (CScriptTexture*, const SDL_FRect*, float, float, float, float, float, const SDL_FRect*), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool flush()", asMETHOD(CScriptRenderer, flush), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "void set_viewport(rect@ r) const property", asMETHOD(CScriptRenderer, set_viewport), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "rect@ get_viewport() const property", asMETHOD(CScriptRenderer, get_viewport), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "rect@ get_render_safe_area() const property", asMETHOD(CScriptRenderer, get_render_safe_area), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "void set_render_clip_rect(rect@ r) const property", asMETHOD(CScriptRenderer, set_render_clip_rect), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "rect@ get_render_clip_rect() const property", asMETHOD(CScriptRenderer, get_render_clip_rect), asCALL_THISCALL);
	engine->RegisterObjectMethod("renderer", "bool get_render_clip_enabled() const property", asMETHOD(CScriptRenderer, render_clip_enabled), asCALL_THISCALL);






	engine->RegisterGlobalFunction("renderer@ request_renderer()", asFUNCTION(request_renderer), asCALL_CDECL);

	engine->RegisterGlobalFunction("int get_num_render_drivers() property", asFUNCTION(SDL_GetNumRenderDrivers), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_render_driver(int index)", asFUNCTION(get_render_driver), asCALL_CDECL);


}