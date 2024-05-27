#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "ngt.h"
using namespace std;
void show_message();

void add_text(std::wstring text);
void MessageCallback(const asSMessageInfo* msg, void* param);
void RegisterFunctions(asIScriptEngine* engine);