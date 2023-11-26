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
/*
    engine->RegisterGlobalProperty("const int SDLK_UNKNOWN", (void*)&SDLK_UNKNOWN);
        engine->RegisterGlobalProperty("const int SDLK_RETURN", (void*)&SDLK_RETURN);
        engine->RegisterGlobalProperty("const int SDLK_ESCAPE", (void*)&SDLK_ESCAPE);
        engine->RegisterGlobalProperty("const int SDLK_BACKSPACE", (void*)&SDLK_BACKSPACE);
        engine->RegisterGlobalProperty("const int SDLK_TAB", (void*)&SDLK_TAB);
        engine->RegisterGlobalProperty("const int SDLK_SPACE", (void*)&SDLK_SPACE);
*/
        engine->RegisterGlobalFunction("void init_engine()", asFUNCTION(init_engine), asCALL_CDECL);
    engine->RegisterGlobalFunction("float random(int, int)", asFUNCTION(random), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak(string &in, bool=true)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool show_game_window(string &in,int=640,int=480)", asFUNCTION(show_game_window), asCALL_CDECL);
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
        int result = builder.StartNewModule(engine, 0);
        result = builder.AddSectionFromFile(argv[1]);

//        result = module->AddScriptSection("NGTGAME", script.c_str());
//        if (result < 0) {
//            std::cerr << "Failed to add script section: " << result << std::endl;
//            return 1;
//        }
//        result = module->Build();
        result = builder.BuildModule();

        if (result < 0) {
            std::cerr << "Failed to build script module: " << result << std::endl;
            return 1;
        }
        int r;
        // Execute the script
        asIScriptContext* ctx = engine->CreateContext();
        if (ctx == 0)
        {
            cout << "Failed to create the context." << endl;
            engine->Release();
            return -1;
        }

        // We don't want to allow the script to hang the application, e.g. with an
        // infinite loop, so we'll use the line callback function to set a timeout
        // that will abort the script after a certain time. Before executing the 
        // script the timeOut variable will be set to the time when the script must 
        // stop executing. 
        // Find the function for the function we want to execute.
        asIScriptFunction* func = engine->GetModule(0)->GetFunctionByDecl("void main()");
        if (func == 0)
        {
            cout << "The function 'void main()' was not found." << endl;
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Prepare the script context with the function we wish to execute. Prepare()
        // must be called on the context before each new script function that will be
        // executed. Note, that if you intend to execute the same function several 
        // times, it might be a good idea to store the function returned by 
        // GetFunctionByDecl(), so that this relatively slow call can be skipped.
        r = ctx->Prepare(func);
        if (r < 0)
        {
            cout << "Failed to prepare the context." << endl;
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Set the timeout before executing the function. Give the function 1 sec
        // to return before we'll abort it.

        // Execute the function
        cout << "Executing the script." << endl;
        cout << "---" << endl;
        result = ctx->Execute();
        if (result != asEXECUTION_FINISHED) {
            std::cerr << "Script execution failed: " << result << std::endl;
            return 1;
        }

        // Clean up
        ctx->Release();
        engine->ShutDownAndRelease();


    }

    return EXIT_SUCCESS;
}