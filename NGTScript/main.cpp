#include <Windows.h>
#include "ngtreg.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()
#include "angelscript.h"
#include "ngt.h"
#include "scriptarray/scriptarray.h"
#include "scriptdictionary/scriptdictionary.h"
#include "scriptfile/scriptfile.h"
#include "scriptfile/scriptfilesystem.h"
#include "scripthelper/scripthelper.h"
#include "datetime/datetime.h"
#include "scriptmath/scriptmath.h"
#include <thread>

BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
class CBytecodeStream : public asIBinaryStream
{
public:
    CBytecodeStream() { f = 0; }
    ~CBytecodeStream() { if (f) fclose(f); }

    int Open(const char* filename, const char* mode)
    {
        if (f) return -1;
#if _MSC_VER >= 1500
        fopen_s(&f, filename, mode);
#else
        f = fopen(filename, mode);
#endif
        if (f == 0) return -1;
        return 0;
    }

    int Write(const void* ptr, asUINT size)
    {
        if (size == 0) return 0;
        fwrite(ptr, size, 1, f);
    }
    int Read(void* ptr, asUINT size)
    {
        if (size == 0) return 0;
        fread(ptr, size, 1, f);
    }
protected:
    FILE* f;
};
int Compile(asIScriptEngine* engine, const char* outputFile)
{
    CBytecodeStream stream;
    int r = stream.Open(outputFile, "wb");
    if (r < 0)
    {
        engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to open output file for writing");

        std::thread t(show_message);
        t.join();
        return -1;
    }

    asIScriptModule* mod = engine->GetModule("ngtgame");
    if (mod == 0)
    {
        engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");

        std::thread t(show_message);
        t.join();

        return -1;
    }

    r = mod->SaveByteCode(&stream);
    if (r < 0)
    {
        engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");

        std::thread t(show_message);
        t.join();

        return -1;
    }
    engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_INFORMATION, "Script was compiled successfully");

    std::thread t(show_message);
    t.join();


    return 0;
}

int Load(asIScriptEngine* engine, const char* inputFile)
{
    CBytecodeStream stream;
    int r = stream.Open(inputFile, "rb");
    if (r < 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to open output file for writing");

        std::thread t(show_message);
        t.join();

        return -1;
    }

    asIScriptModule* mod = engine->GetModule("ngtgame");
    if (mod == 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");

        std::thread t(show_message);
        t.join();

        return -1;
    }

    r = mod->LoadByteCode(&stream);
    if (r < 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");

        std::thread t(show_message);
        t.join();

        return -1;
    }


    return 0;
}


std::string filename;
std::string flag;

int main(int argc, char* argv[]) {
    if (FileExists(L"game_object.ngtb")) {
        filename = "game_object.ngtb";
        flag = "-b";
//        alert("DebugInfo", filename);
    }
    else {
    filename = argv[1];
    flag = argv[2];
    }
        if (flag == "-c") {
        asIScriptEngine* engine = asCreateScriptEngine();

        // Register any necessary functions and types
        // ...
        RegisterStdString(engine);
        RegisterFunctions(engine);
        RegisterScriptArray(engine, true);
        RegisterStdStringUtils(engine);
        RegisterScriptDictionary(engine);
        RegisterScriptDateTime(engine);
        RegisterScriptFile(engine);
        RegisterScriptFileSystem(engine);
        RegisterExceptionRoutines(engine);
        RegisterScriptMath(engine);
        // Compile the script
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
        int result = builder.StartNewModule(engine, "ngtgame");
        result = builder.AddSectionFromFile(argv[1]);

        result = builder.BuildModule();

        if (result < 0) {
            std::thread t(show_message);
            t.join();

            return 1;
        }
        module = engine->GetModule("ngtgame");
        if (module)
        {
            Compile(engine, "game_object.ngtb");
        }


        // Call compiler to create executable file
        CreateDirectory(L"Release", 0);
        CopyFile(L"NGTScript.exe", L"Release/run.exe", false);
        CopyFile(L"angelscript64.dll", L"Release/angelscript64.dll", false);
        CopyFile(L"bass.dll", L"Release/bass.dll", false);
        CopyFile(L"phonon.dll", L"Release/phonon.dll", false);
        CopyFile(L"tolk.dll", L"Release/tolk.dll", false);
        CopyFile(L"SAAPI64.dll", L"Release/SAAPI64.dll", false);

        CopyFile(L"nvdaControllerClient64.dll", L"Release/nvdaControllerClient64.dll", false);
        CopyFile(L"SDL2.dll", L"Release/SDL2.dll", false);
        CopyFile(L"SDL2_net.dll", L"Release/SDL2_net.dll", false);

        CopyFile(L"game_object.ngtb", L"Release/game_object.ngtb", false);
        DeleteFile(L"game_object.ngtb");
        }
    else if (flag == "-d") {
        asIScriptEngine* engine = asCreateScriptEngine();

        // Register any necessary functions and types
        // ...
        RegisterStdString(engine);
        RegisterFunctions(engine);
        RegisterScriptArray(engine, true);
        RegisterStdStringUtils(engine);
        RegisterScriptDictionary(engine);
        RegisterScriptDateTime(engine);
        RegisterScriptFile(engine);
        RegisterScriptFileSystem(engine);
        RegisterExceptionRoutines(engine);
        RegisterScriptMath(engine);

        // Compile the script
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
        int result = builder.StartNewModule(engine, "ngtgame");
        result = builder.AddSectionFromFile(argv[1]);
        result = builder.BuildModule();

        if (result < 0) {
            std::thread t(show_message);
            t.join();


                        return 1;
            }
        int r;
        // Execute the script
        asIScriptContext* ctx = engine->CreateContext();
        if (ctx == 0)
        {
            alert("NGTExecutableError", "Failed to create the context.");
            engine->Release();
            return -1;
        }

        // We don't want to allow the script to hang the application, e.g. with an
        // infinite loop, so we'll use the line callback function to set a timeout
        // that will abort the script after a certain time. Before executing the 
        // script the timeOut variable will be set to the time when the script must 
        // stop executing. 
        // Find the func        tion for the function we want to execute.
        asIScriptFunction* func = engine->GetModule("ngtgame")->GetFunctionByDecl("void main()");
        if (func == 0)
        {
            alert("NGTExecutableError", "The function 'void main()' was not found.");
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
            alert("NGTExecutableError", "Failed to prepare the context.");
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Set the timeout before executing the function. Give the function 1 sec
        // to return before we'll abort it.

        // Execute the function
        result = ctx->Execute();
        if (result != asEXECUTION_FINISHED) {
            std::string output = GetExceptionInfo(ctx, true);
            alert("NGTRuntimeError", "Info: " + output);
            return 1;
        }

        // Clean up
        ctx->Release();
        engine->ShutDownAndRelease();


    }
    else if (flag == "-b") {
        asIScriptEngine* engine = asCreateScriptEngine();

        RegisterStdString(engine);
        RegisterFunctions(engine);
        RegisterScriptArray(engine, true);
        RegisterStdStringUtils(engine);
        RegisterScriptDictionary(engine);
        RegisterScriptDateTime(engine);
        RegisterScriptFile(engine);
        RegisterScriptFileSystem(engine);
        RegisterExceptionRoutines(engine);
        RegisterScriptMath(engine);

        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
        int result;
        module = engine->GetModule("ngtgame");
        if (module)
        {
            Load(engine, "game_object.ngtb");
        }
        asIScriptContext* ctx = engine->CreateContext();
        if (ctx == 0)
        {
            alert("NGTExecutableError", "Failed to create the context.");
            engine->Release();
            return -1;
        }

        // We don't want to allow the script to hang the application, e.g. with an
        // infinite loop, so we'll use the line callback function to set a timeout
        // that will abort the script after a certain time. Before executing the 
        // script the timeOut variable will be set to the time when the script must 
        // stop executing. 
        // Find the func        tion for the function we want to execute.
        asIScriptFunction* func = engine->GetModule("ngtgame")->GetFunctionByDecl("void main()");
        if (func == 0)
        {
            alert("NGTExecutableError", "The function 'void main()' was not found.");
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Prepare the script context with the function we wish to execute. Prepare()
        // must be called on the context before each new script function that will be
        // executed. Note, that if you intend to execute the same function several 
        // times, it might be a good idea to store the function returned by 
        // GetFunctionByDecl(), so that this relatively slow call can be skipped.
        int r = ctx->Prepare(func);
        if (r < 0)
        {
            alert("NGTExecutableError", "Failed to prepare the context.");
            ctx->Release();
            engine->Release();
            return -1;
        }

        // Set the timeout before executing the function. Give the function 1 sec
        // to return before we'll abort it.

        // Execute the function
        result = ctx->Execute();
        if (result != asEXECUTION_FINISHED) {
            std::string output = GetExceptionInfo(ctx, true);
            alert("NGTRuntimeError", "Info: "+ output);
            return 1;
        }

        // Clean up
        ctx->Release();
        engine->ShutDownAndRelease();


    }
    return EXIT_SUCCESS;
}
