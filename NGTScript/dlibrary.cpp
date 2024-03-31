#include "dlibrary.h"
bool dlibrary::load(const wchar_t* name) {
	lib = LoadLibraryW(name);
	return lib == NULL;
}
lfunc dlibrary::get(const char* function_name) {
    FARPROC address = GetProcAddress(lib, function_name);
    lfunc function = reinterpret_cast<lfunc>(address);
    return function;
}
bool dlibrary::unload() {
    FreeLibrary(lib);
    return lib != NULL;
}