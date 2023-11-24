#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#include "ngt.h"
using namespace std;
void RegisterFunctions(asIScriptEngine* engine)
{
    engine->RegisterGlobalFunction("void sound_system_init()", asFUNCTION(sound_system_init), asCALL_CDECL);
    engine->RegisterGlobalFunction("float random(int, int)", asFUNCTION(random), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak(string, bool)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool show_game_window(string)", asFUNCTION(show_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void update_game_window()", asFUNCTION(update_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_pressed(int)", asFUNCTION(key_pressed), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_released(int)", asFUNCTION(key_released), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_down(int)", asFUNCTION(key_down), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool alert(string, string)", asFUNCTION(alert), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTION(set_listener_position), asCALL_CDECL);
    engine->RegisterGlobalFunction("void wait(int)", asFUNCTION(wait), asCALL_CDECL);
    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<sound>());
    engine->RegisterObjectMethod("sound", "bool load(string, bool)", asMETHOD(sound, load), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool load_looped(string, bool)", asMETHOD(sound, load_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play()", asMETHOD(sound, play), asCALL_THISCALL);
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
    engine->RegisterObjectMethod("sound", "double get_pitch_lower_limit() const", asMETHOD(sound, get_pitch_lower_limit), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_active() const", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_playing() const", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_paused() const", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_position() const", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_length() const", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_sample_rate() const", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_channels() const", asMETHOD(sound, get_channels), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_bits() const", asMETHOD(sound, get_bits), asCALL_THISCALL);
    engine->RegisterObjectType("timer", sizeof(timer), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<timer>());
    engine->RegisterObjectMethod("timer", "int elapsed()", asMETHOD(timer, elapsed), asCALL_THISCALL);
//    engine->RegisterObjectMethod("timer", "void elapsed(int)", asMETHOD(timer, elapsed), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void restart()", asMETHOD(timer, restart), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void pause()", asMETHOD(timer, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void resume()", asMETHOD(timer, resume), asCALL_THISCALL);
    engine->RegisterObjectType("key_hold", sizeof(key_hold), asOBJ_VALUE | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<key_hold>());
    engine->RegisterObjectMethod("key_hold", "bool pressing()", asMETHOD(key_hold, pressing), asCALL_THISCALL);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename.as> [-c/-d]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    std::string flag = argv[2];

    if (flag != "-c" && flag != "-d") {
        std::cerr << "Invalid flag: " << flag << std::endl;
        return EXIT_FAILURE;
    }

    if (flag == "-c") {
        // Call compiler to create executable file
    }
    else if (flag == "-d") {
        // Call interpreter to execute .as file
    }

    return EXIT_SUCCESS;
}