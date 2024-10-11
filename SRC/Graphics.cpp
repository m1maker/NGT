#include "Graphics.h"
#include <angelscript.h>
#include <SDL3/SDL.h>
#include <string>
using namespace std;

class CScriptTexture {
public:
	SDL_Texture* texture;
};


class CScriptRenderer {
public:
	SDL_Renderer* renderer;
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
		return t;
	}

private:
	mutable int ref = 0;
};

std::string get_render_driver(int index) {
	return std::string(SDL_GetRenderDriver(index));
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

	engine->RegisterGlobalFunction("int get_num_render_drivers() property", asFUNCTION(SDL_GetNumRenderDrivers), asCALL_CDECL);


}