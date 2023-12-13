#pragma comment(lib, "angelscript64.lib")
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#include "ngt.h"
using namespace std;
void MessageCallback(const asSMessageInfo* msg, void* param)
{
    const char* type = "ERR ";
    if (msg->type == asMSGTYPE_WARNING)
        type = "WARN";
    else if (msg->type == asMSGTYPE_INFORMATION)
        type = "INFO";
    char rowStr[10], colStr[10];
    _itoa_s(msg->row, rowStr, 10);
    _itoa_s(msg->col, colStr, 10);
    std::string messageStr(msg->message);
    std::string output = std::string(msg->section) + " (" + rowStr + ", " + colStr + ") : " + type + " : " + messageStr;

    alert("NGTInfo", output);
}
void RegisterFunctions(asIScriptEngine* engine)
{
    engine->RegisterGlobalFunction("void init_engine()", asFUNCTION(init_engine), asCALL_CDECL);
    engine->RegisterGlobalFunction("int random(int, int)", asFUNCTION(random), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak(string &in, bool=true)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool show_game_window(string &in,int=640,int=480)", asFUNCTION(show_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void hide_game_window()", asFUNCTION(hide_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_game_window_title(string &in)", asFUNCTION(set_game_window_title), asCALL_CDECL);

    engine->RegisterGlobalFunction("void update_game_window()", asFUNCTION(update_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void quit()",asFUNCTION(quit),asCALL_CDECL);
    engine->RegisterGlobalFunction("bool clipboard_copy_text(string &in)", asFUNCTION(clipboard_copy_text), asCALL_CDECL);
    engine->RegisterGlobalFunction("string clipboard_read_text()", asFUNCTION(clipboard_read_text), asCALL_CDECL);
    engine->RegisterGlobalFunction("string get_input()", asFUNCTION(get_input), asCALL_CDECL);

    engine->RegisterGlobalFunction("bool key_pressed(int)", asFUNCTION(key_pressed), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_released(int)", asFUNCTION(key_released), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_down(int)", asFUNCTION(key_down), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_repeat(int)", asFUNCTION(key_repeat), asCALL_CDECL);

    engine->RegisterGlobalFunction("bool alert(string &in, string &in)", asFUNCTION(alert), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTION(set_listener_position), asCALL_CDECL);
    engine->RegisterGlobalFunction("void wait(int)", asFUNCTION(wait), asCALL_CDECL);
    engine->RegisterGlobalFunction("void delay(int)",asFUNCTION(delay),asCALL_CDECL);
    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<sound>());
    engine->RegisterObjectMethod("sound", "bool load(string &in, bool=false)", asMETHOD(sound, load), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play()", asMETHOD(sound, play), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_looped()", asMETHOD(sound, play_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool pause()", asMETHOD(sound, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_wait()", asMETHOD(sound, play_wait), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stop()", asMETHOD(sound, stop), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool close()", asMETHOD(sound, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_sound_position(float, float, float)", asMETHOD(sound, set_sound_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_pan() const", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(float)", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_volume() const", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(float)", asMETHOD(sound, set_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_pitch() const", asMETHOD(sound, get_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch(double)", asMETHOD(sound, set_pitch), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_pitch_lower_limit() const", asMETHOD(sound, get_pitch_lower_limit), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_active() const", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_playing() const", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_paused() const", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_position() const", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_length() const", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_sample_rate() const", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_channels() const", asMETHOD(sound, get_channels), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_bits() const", asMETHOD(sound, get_bits), asCALL_THISCALL);
    engine->RegisterObjectType("timer", sizeof(timer), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<timer>());
    engine->RegisterObjectMethod("timer", "int elapsed()", asMETHOD(timer, elapsed), asCALL_THISCALL);
//    engine->RegisterObjectMethod("timer", "void elapsed(int)", asMETHOD(timer, elapsed), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void restart()", asMETHOD(timer, restart), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void pause()", asMETHOD(timer, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void resume()", asMETHOD(timer, resume), asCALL_THISCALL);
//    engine->RegisterObjectType("key_hold", sizeof(key_hold), asOBJ_VALUE | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<key_hold>());
//    engine->RegisterObjectMethod("key_hold", "bool pressing()", asMETHOD(key_hold, pressing), asCALL_THISCALL);
}

