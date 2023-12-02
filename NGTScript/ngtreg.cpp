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
    const int SDLK_FIRST = 0;
    int temp3 = SDLK_FIRST;
    engine->RegisterGlobalProperty("const int SDLK_FIRST", (void*)&temp3);

    const int SDLK_BACKSPACE = SDLK_BACKSPACE;
    int temp4 = SDLK_BACKSPACE;
    engine->RegisterGlobalProperty("const int SDLK_BACKSPACE", (void*)&temp4);

    const int SDLK_TAB = SDLK_TAB;
    int temp5 = SDLK_TAB;
    engine->RegisterGlobalProperty("const int SDLK_TAB", (void*)&temp5);

    const int SDLK_CLEAR = SDLK_CLEAR;
    int temp6 = SDLK_CLEAR;
    engine->RegisterGlobalProperty("const int SDLK_CLEAR", (void*)&temp6);

    const int SDLK_RETURN = SDLK_RETURN;
    int temp7 = SDLK_RETURN;
    engine->RegisterGlobalProperty("const int SDLK_RETURN", (void*)&temp7);

    const int SDLK_PAUSE = 19;
    int temp8 = SDLK_PAUSE;
    engine->RegisterGlobalProperty("const int SDLK_PAUSE", (void*)&temp8);

    const int SDLK_ESCAPE = 27;
    int temp9 = SDLK_ESCAPE;
    engine->RegisterGlobalProperty("const int SDLK_ESCAPE", (void*)&temp9);

    const int SDLK_SPACE = 32;
    int temp10 = SDLK_SPACE;
    engine->RegisterGlobalProperty("const int SDLK_SPACE", (void*)&temp10);

    const int SDLK_EXCLAIM = 33;
    int temp11 = SDLK_EXCLAIM;
    engine->RegisterGlobalProperty("const int SDLK_EXCLAIM", (void*)&temp11);

    const int SDLK_QUOTEDBL = 34;
    int temp12 = SDLK_QUOTEDBL;
    engine->RegisterGlobalProperty("const int SDLK_QUOTEDBL", (void*)&temp12);

    const int SDLK_HASH = 35;
    int temp13 = SDLK_HASH;
    engine->RegisterGlobalProperty("const int SDLK_HASH", (void*)&temp13);

    const int SDLK_DOLLAR = 36;
    int temp14 = SDLK_DOLLAR;
    engine->RegisterGlobalProperty("const int SDLK_DOLLAR", (void*)&temp14);

    const int SDLK_AMPERSAND = 38;
    int temp15 = SDLK_AMPERSAND;
    engine->RegisterGlobalProperty("const int SDLK_AMPERSAND", (void*)&temp15);

    const int SDLK_QUOTE = 39;
    int temp16 = SDLK_QUOTE;
    engine->RegisterGlobalProperty("const int SDLK_QUOTE", (void*)&temp16);

    const int SDLK_LEFTPAREN = 40;
    int temp17 = SDLK_LEFTPAREN;
    engine->RegisterGlobalProperty("const int SDLK_LEFTPAREN", (void*)&temp17);

    const int SDLK_RIGHTPAREN = 41;
    int temp18 = SDLK_RIGHTPAREN;
    engine->RegisterGlobalProperty("const int SDLK_RIGHTPAREN", (void*)&temp18);

    const int SDLK_ASTERISK = 42;
    int temp19 = SDLK_ASTERISK;
    engine->RegisterGlobalProperty("const int SDLK_ASTERISK", (void*)&temp19);

    const int SDLK_PLUS = 43;
    int temp20 = SDLK_PLUS;
    engine->RegisterGlobalProperty("const int SDLK_PLUS", (void*)&temp20);

    const int SDLK_COMMA = 44;
    int temp21 = SDLK_COMMA;
    engine->RegisterGlobalProperty("const int SDLK_COMMA", (void*)&temp21);

    const int SDLK_MINUS = 45;
    int temp22 = SDLK_MINUS;
    engine->RegisterGlobalProperty("const int SDLK_MINUS", (void*)&temp22);

    const int SDLK_PERIOD = 46;
    int temp23 = SDLK_PERIOD;
    engine->RegisterGlobalProperty("const int SDLK_PERIOD", (void*)&temp23);

    const int SDLK_SLASH = 47;
    int temp24 = SDLK_SLASH;
    engine->RegisterGlobalProperty("const int SDLK_SLASH", (void*)&temp24);

    const int SDLK_0 = 48;
    int temp25 = SDLK_0;
    engine->RegisterGlobalProperty("const int SDLK_0", (void*)&temp25);

    const int SDLK_1 = 49;
    int temp26 = SDLK_1;
    engine->RegisterGlobalProperty("const int SDLK_1", (void*)&temp26);

    const int SDLK_2 = 50;
    int temp27 = SDLK_2;
    engine->RegisterGlobalProperty("const int SDLK_2", (void*)&temp27);

    const int SDLK_3 = 51;
    int temp28 = SDLK_3;
    engine->RegisterGlobalProperty("const int SDLK_3", (void*)&temp28);

    const int SDLK_4 = 52;
    int temp29 = SDLK_4;
    engine->RegisterGlobalProperty("const int SDLK_4", (void*)&temp29);

    const int SDLK_5 = 53;
    int temp30 = SDLK_5;
    engine->RegisterGlobalProperty("const int SDLK_5", (void*)&temp30);

    const int SDLK_6 = 54;
    int temp31 = SDLK_6;
    engine->RegisterGlobalProperty("const int SDLK_6", (void*)&temp31);

    const int SDLK_7 = 55;
    int temp32 = SDLK_7;
    engine->RegisterGlobalProperty("const int SDLK_7", (void*)&temp32);

    const int SDLK_8 = 56;
    int temp33 = SDLK_8;
    engine->RegisterGlobalProperty("const int SDLK_8", (void*)&temp33);

    const int SDLK_9 = 57;
    int temp34 = SDLK_9;
    engine->RegisterGlobalProperty("const int SDLK_9", (void*)&temp34);

    const int SDLK_COLON = 58;
    int temp35 = SDLK_COLON;
    engine->RegisterGlobalProperty("const int SDLK_COLON", (void*)&temp35);
    const int temp36 = SDLK_SEMICOLON;
    engine->RegisterGlobalProperty("const int SDLK_SEMICOLON", (void*)&temp36);

    const int SDLK_LESS = 60;
    int temp37 = SDLK_LESS;
    engine->RegisterGlobalProperty("const int SDLK_LESS", (void*)&temp37);

    const int SDLK_EQUALS = 61;
    int temp38 = SDLK_EQUALS;
    engine->RegisterGlobalProperty("const int SDLK_EQUALS", (void*)&temp38);

    const int SDLK_GREATER = 62;
    int temp39 = SDLK_GREATER;
    engine->RegisterGlobalProperty("const int SDLK_GREATER", (void*)&temp39);

    const int SDLK_QUESTION = 63;
    int temp40 = SDLK_QUESTION;
    engine->RegisterGlobalProperty("const int SDLK_QUESTION", (void*)&temp40);

    const int SDLK_AT = 64;
    int temp41 = SDLK_AT;
    engine->RegisterGlobalProperty("const int SDLK_AT", (void*)&temp41);

    const int SDLK_LEFTBRACKET = 91;
    int temp42 = SDLK_LEFTBRACKET;
    engine->RegisterGlobalProperty("const int SDLK_LEFTBRACKET", (void*)&temp42);

    const int SDLK_BACKSLASH = 92;
    int temp43 = SDLK_BACKSLASH;
    engine->RegisterGlobalProperty("const int SDLK_BACKSLASH", (void*)&temp43);

    const int SDLK_RIGHTBRACKET = 93;
    int temp44 = SDLK_RIGHTBRACKET;
    engine->RegisterGlobalProperty("const int SDLK_RIGHTBRACKET", (void*)&temp44);

    const int SDLK_CARET = 94;
    int temp45 = SDLK_CARET;
    engine->RegisterGlobalProperty("const int SDLK_CARET", (void*)&temp45);

    const int SDLK_UNDERSCORE = 95;
    int temp46 = SDLK_UNDERSCORE;
    engine->RegisterGlobalProperty("const int SDLK_UNDERSCORE", (void*)&temp46);

    const int SDLK_BACKQUOTE = 96;
    int temp47 = SDLK_BACKQUOTE;
    engine->RegisterGlobalProperty("const int SDLK_BACKQUOTE", (void*)&temp47);

    const int SDLK_a = 97;
    int temp48 = SDLK_a;
    engine->RegisterGlobalProperty("const int SDLK_a", (void*)&temp48);

    const int SDLK_b = 98;
    int temp49 = SDLK_b;
    engine->RegisterGlobalProperty("const int SDLK_b", (void*)&temp49);

    const int SDLK_c = 99;
    int temp50 = SDLK_c;
    engine->RegisterGlobalProperty("const int SDLK_c", (void*)&temp50);

    const int SDLK_d = 100;
    int temp51 = SDLK_d;
    engine->RegisterGlobalProperty("const int SDLK_d", (void*)&temp51);

    const int SDLK_e = 101;
    int temp52 = SDLK_e;
    engine->RegisterGlobalProperty("const int SDLK_e", (void*)&temp52);

    const int SDLK_f = 102;
    int temp53 = SDLK_f;
    engine->RegisterGlobalProperty("const int SDLK_f", (void*)&temp53);

    const int SDLK_g = 103;
    int temp54 = SDLK_g;
    engine->RegisterGlobalProperty("const int SDLK_g", (void*)&temp54);

    const int SDLK_h = 104;
    int temp55 = SDLK_h;
    engine->RegisterGlobalProperty("const int SDLK_h", (void*)&temp55);

    const int SDLK_i = 105;
    int temp56 = SDLK_i;
    engine->RegisterGlobalProperty("const int SDLK_i", (void*)&temp56);

    const int SDLK_j = 106;
    int temp57 = SDLK_j;
    engine->RegisterGlobalProperty("const int SDLK_j", (void*)&temp57);

    const int SDLK_k = 107;
    int temp58 = SDLK_k;
    engine->RegisterGlobalProperty("const int SDLK_k", (void*)&temp58);

    const int SDLK_l = 108;
    int temp59 = SDLK_l;
    engine->RegisterGlobalProperty("const int SDLK_l", (void*)&temp59);

    const int SDLK_m = 109;
    int temp60 = SDLK_m;
    engine->RegisterGlobalProperty("const int SDLK_m", (void*)&temp60);

    const int SDLK_n = 110;
    int temp61 = SDLK_n;
    engine->RegisterGlobalProperty("const int SDLK_n", (void*)&temp61);

    const int SDLK_o = 111;
    int temp62 = SDLK_o;
    engine->RegisterGlobalProperty("const int SDLK_o", (void*)&temp62);

    const int SDLK_p = 112;
    int temp63 = SDLK_p;
    engine->RegisterGlobalProperty("const int SDLK_p", (void*)&temp63);

    const int SDLK_q = 113;
    int temp64 = SDLK_q;
    engine->RegisterGlobalProperty("const int SDLK_q", (void*)&temp64);

    const int SDLK_r = 114;
    int temp65 = SDLK_r;
    engine->RegisterGlobalProperty("const int SDLK_r", (void*)&temp65);

    const int SDLK_s = 115;
    int temp66 = SDLK_s;
    engine->RegisterGlobalProperty("const int SDLK_s", (void*)&temp66);

    const int SDLK_t = 116;
    int temp67 = SDLK_t;
    engine->RegisterGlobalProperty("const int SDLK_t", (void*)&temp67);

    const int SDLK_u = 117;
    int temp68 = SDLK_u;
    engine->RegisterGlobalProperty("const int SDLK_u", (void*)&temp68);

    const int SDLK_v = 118;
    int temp69 = SDLK_v;
    engine->RegisterGlobalProperty("const int SDLK_v", (void*)&temp69);

    const int SDLK_w = 119;
    int temp70 = SDLK_w;
    engine->RegisterGlobalProperty("const int SDLK_w", (void*)&temp70);

    const int SDLK_x = 120;
    int temp71 = SDLK_x;
    engine->RegisterGlobalProperty("const int SDLK_x", (void*)&temp71);

    const int SDLK_y = 121;
    int temp72 = SDLK_y;
    engine->RegisterGlobalProperty("const int SDLK_y", (void*)&temp72);

    const int SDLK_z = 122;
    int temp73 = SDLK_z;
    engine->RegisterGlobalProperty("const int SDLK_z", (void*)&temp73);
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

        std::ifstream input("NGTScript.exe", std::ios::binary);
        std::ofstream output("a.exe", std::ios::binary);

        if (input.is_open() && output.is_open()) {
            char header[0x3C];
            input.read(header, 0x3C);
            output.write(header, 0x3C);

            const char* prefix = "AS";
            const char* bytecode = "bytecode_here";
            output.write(prefix, 2);
            output.write(bytecode, 12);

            char buffer[1024];
            while (!input.eof()) {
                input.read(buffer, 1024);
                output.write(buffer, input.gcount());
            }
        }

        input.close();
        output.close();



        std::ifstream input("new.exe", std::ios::binary);

        if (input.is_open()) {
            char header[0x3C];
            input.read(header, 0x3C);

            char prefix[2];
            input.read(prefix, 2);
            if (prefix[0] == 'A' && prefix[1] == 'S') {
                char bytecode[12];
                input.read(bytecode, 12);
            }
        }

        input.close();
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