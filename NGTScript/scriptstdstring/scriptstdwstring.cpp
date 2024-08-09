#include "ngt.h"
#include "scriptstdwstring.h"
#include <assert.h> // assert()
#include <sstream>  // std::wstringstream
#include <stdio.h>	// snprintf()
#include <stdlib.h> // strtod()
#include <string.h> // strstr()
#ifndef __psp2__
#include <locale.h> // setlocale()
#endif
#include <algorithm>
using namespace std;
// This macro is used to avoid warnings about unused variables.
// Usually where the variables are only used in debug mode.
#define UNUSED_VAR(x) (void)(x)

#ifdef AS_CAN_USE_CPP11
// The wstring factory doesn't need to keep a specific order in the
// cache, so the unordered_map is faster than the ordinary map
#include <unordered_map>  // std::unordered_map
BEGIN_AS_NAMESPACE
typedef unordered_map<wstring, int> map_t;
END_AS_NAMESPACE
#else
#include <map>      // std::map
BEGIN_AS_NAMESPACE
typedef map<wstring, int> map_t;
END_AS_NAMESPACE
#endif

BEGIN_AS_NAMESPACE
static void ConstructWstring(wstring* thisPointer)
{
	new(thisPointer) wstring();
}

static void CopyConstructWstring(const wstring& other, wstring* thisPointer)
{
	new(thisPointer) wstring(other);
}
static void DestructWstring(wstring* thisPointer)
{
	thisPointer->~wstring();
}

static wstring& AddAssignWstringToWstring(const wstring& str, wstring& dest)
{
	// We don't register the method directly because some compilers
	// and standard libraries inline the definition, resulting in the
	// linker being unable to find the declaration.
	// Example: CLang/LLVM with XCode 4.3 on OSX 10.7
	dest += str;
	return dest;
}

void RegisterStdWstring_Native(asIScriptEngine* engine)
{
	int r = 0;
	UNUSED_VAR(r);

	// Register the wstring type
#if AS_CAN_USE_CPP11
	// With C++11 it is possible to use asGetTypeTraits to automatically determine the correct flags to use
	r = engine->RegisterObjectType("wstring", sizeof(wstring), asOBJ_VALUE | asGetTypeTraits<wstring>()); assert(r >= 0);
#else
	r = engine->RegisterObjectType("wstring", sizeof(wstring), asOBJ_VALUE | asOBJ_APP_CLASS_CDAK); assert(r >= 0);
#endif


	// Register the object operator overloads
	r = engine->RegisterObjectBehaviour("wstring", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ConstructWstring), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("wstring", asBEHAVE_CONSTRUCT, "void f(const wstring &in)", asFUNCTION(CopyConstructWstring), asCALL_CDECL_OBJLAST); assert(r >= 0);

	r = engine->RegisterObjectBehaviour("wstring", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructWstring), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectMethod("wstring", "wstring &opAssign(const wstring &in)", asMETHODPR(wstring, operator =, (const wstring&), wstring&), asCALL_THISCALL); assert(r >= 0);
	// Need to use a wrapper on Mac OS X 10.7/XCode 4.3 and CLang/LLVM, otherwise the linker fails
	r = engine->RegisterObjectMethod("wstring", "wstring &opAddAssign(const wstring &in)", asFUNCTION(AddAssignWstringToWstring), asCALL_CDECL_OBJLAST); assert(r >= 0);
	r = engine->RegisterObjectMethod("wstring", "void clear()", asMETHODPR(wstring, clear, (), void), asCALL_THISCALL); assert(r >= 0);

	// TODO: Implement the following
	// findAndReplace - replaces a text found in the string
	// replaceRange - replaces a range of bytes in the string
	// multiply/times/opMul/opMul_r - takes the string and multiplies it n times, e.g. "-".multiply(5) returns "-----"
}

void RegisterStdWstring(asIScriptEngine* engine)
{
	RegisterStdWstring_Native(engine);
}

END_AS_NAMESPACE




