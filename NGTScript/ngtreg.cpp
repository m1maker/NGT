#pragma comment(lib, "angelscript64.lib")
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
    printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}
void RegisterFunctions(asIScriptEngine* engine)
{
    engine->RegisterGlobalFunction("void init_engine()", asFUNCTION(init_engine), asCALL_CDECL);
    engine->RegisterGlobalFunction("float random(int, int)", asFUNCTION(random), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak(string &in, bool)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool show_game_window(string &in,int,int)", asFUNCTION(show_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void update_game_window()", asFUNCTION(update_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void quit()",asFUNCTION(quit),asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_pressed(int)", asFUNCTION(key_pressed), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_released(int)", asFUNCTION(key_released), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_down(int)", asFUNCTION(key_down), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool alert(string &in, string &in)", asFUNCTION(alert), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTION(set_listener_position), asCALL_CDECL);
    engine->RegisterGlobalFunction("void wait(int)", asFUNCTION(wait), asCALL_CDECL);
    engine->RegisterGlobalFunction("void delay(int)",asFUNCTION(delay),asCALL_CDECL);
    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<sound>());
    engine->RegisterObjectMethod("sound", "bool load(string &in, bool)", asMETHOD(sound, load), asCALL_THISCALL);
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

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename.ngt> [-c/-d]" << std::endl;
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
        std::ifstream file(argv[1]);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << argv[1] << std::endl;
            return 1;
        }
        std::string script((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        // Create the script engine
        asIScriptEngine* engine = asCreateScriptEngine();

        // Register any necessary functions and types
        // ...
        RegisterStdString(engine);
        RegisterFunctions(engine);
        // Compile the script
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("MyModule", asGM_ALWAYS_CREATE);
        int result = module->AddScriptSection("NGTGAME", script.c_str());
        if (result < 0) {
            std::cerr << "Failed to add script section: " << result << std::endl;
            return 1;
        }
        result = module->Build();

        if (result < 0) {
            std::cerr << "Failed to build script module: " << result << std::endl;
            return 1;
        }

        // Execute the script
        asIScriptContext* context = engine->CreateContext();
        result = context->Prepare(module->GetFunctionByDecl("void main()"));
        if (result < 0) {
            std::cerr << "Failed to prepare script context: " << result << std::endl;
            return 1;
        }
        result = context->Execute();
        if (result != asEXECUTION_FINISHED) {
            std::cerr << "Script execution failed: " << result << std::endl;
            return 1;
        }

        // Clean up
        context->Release();
        engine->ShutDownAndRelease();


    }

    return EXIT_SUCCESS;
}