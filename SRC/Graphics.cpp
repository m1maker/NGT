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

	engine->RegisterGlobalFunction("int get_num_render_drivers() property", asFUNCTION(SDL_GetNumRenderDrivers), asCALL_CDECL);


}