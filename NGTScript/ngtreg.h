#pragma comment(lib, "angelscript64.lib")
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#include "ngt.h"
CScriptBuilder builder;
using namespace std;
void MessageCallback(const asSMessageInfo* msg, void* param);
void RegisterFunctions(asIScriptEngine* engine);