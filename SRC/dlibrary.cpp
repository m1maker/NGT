#include "dlibrary.h"
#include"ngt.h"
#include<filesystem>
#include <sdl3/sdl.h>
bool dlibrary::load(const std::string& name) {
	lib = SDL_LoadObject(name.c_str());
	return lib != NULL;
}
void* dlibrary::get(const char* function_name) {
	void* address = (void*)SDL_LoadFunction(lib, function_name);
	return address;
}
bool dlibrary::get_active()const {
	return lib != 0;
}
bool dlibrary::unload() {
	SDL_UnloadObject(lib);
	return lib == NULL;
}