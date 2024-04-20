#pragma once
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <Windows.h>
typedef void*(*lfunc)(...);
class dlibrary {
public:
	bool load(const wchar_t* name);
	lfunc get(const char* function_name);
	bool unload();
protected:
	HMODULE lib;
};