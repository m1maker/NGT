#ifndef NGTSOUND_H
#define NGTSOUND_H
#include "angelscript.h"
void soundsystem_init();
void soundsystem_free();
void register_sound(asIScriptEngine* engine);
#endif