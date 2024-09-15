#ifndef NGT_REGISTERED_H
#define NGT_REGISTERED_H
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <cstdlib>
#include <fstream>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "ngt.h"
#define AS_BEGIN(x, y) x->SetDefaultNamespace(y);
#define AS_END(x) x->SetDefaultNamespace("")
using namespace std;
void show_message();
extern std::string g_ScriptMessagesInfo;
extern std::string g_ScriptMessagesWarning;
extern std::string g_ScriptMessagesError;



void MessageCallback(const asSMessageInfo* msg, void* param);
void RegisterFunctions(asIScriptEngine* engine);
#endif