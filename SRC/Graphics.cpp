#include "Graphics.h"
#include "ngt.h"
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
	void AddRef()const {
		ref += 1;
	}

	void Release()const {
		if (--ref < 1) {
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
		return t;
	}

private:
	mutable int ref = 0;
};

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

	engine->RegisterGlobalFunction("int get_num_render_drivers() property", asFUNCTION(SDL_GetNumRenderDrivers), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_render_driver(int index)", asFUNCTION(get_render_driver), asCALL_CDECL);


}