#include "ngtreg.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()
#include "ByteCode.h"
#include "angelscript.h"
#include "ngt.h"
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
        return -1;
    }

    asIScriptModule* mod = engine->GetModule("ngtgame");
    if (mod == 0)
    {
        engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");
        return -1;
    }

    r = mod->SaveByteCode(&stream);
    if (r < 0)
    {
        engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");
        return -1;
    }

    engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_INFORMATION, "Script was compiled successfully");

    return 0;
}

int Load(asIScriptEngine* engine, const char* inputFile)
{
    CBytecodeStream stream;
    int r = stream.Open(inputFile, "rb");
    if (r < 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to open output file for writing");
        return -1;
    }

    asIScriptModule* mod = engine->GetModule("ngtgame");
    if (mod == 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");
        return -1;
    }

    r = mod->LoadByteCode(&stream);
    if (r < 0)
    {
        engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");
        return -1;
    }


    return 0;
}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <filename.ngt> [-c/-d]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string filename = argv[1];
    std::string flag = argv[2];

    if (flag == "-c") {
        asIScriptEngine* engine = asCreateScriptEngine();

        // Register any necessary functions and types
        // ...
        RegisterStdString(engine);
        RegisterFunctions(engine);
        // Compile the script
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
        int result = builder.StartNewModule(engine, "ngtgame");
        result = builder.AddSectionFromFile(argv[1]);

        result = builder.BuildModule();

        if (result < 0) {
            alert("NGTBuilderError", "Failed to build script module: ");
            return 1;
        }
        module = engine->GetModule("ngtgame");
        if (module)
        {
            Compile(engine, "game_object.ngtb");
        }


        // Call compiler to create executable file
        system("7za a -t7z a.7z *.exe *.dll *.ngtb");
        system("copy /b exec.bin + a.7z + config.txt a.exe");
    }
    else if (flag == "-d") {
        asIScriptEngine* engine = asCreateScriptEngine();

        // Register any necessary functions and types
        // ...
        RegisterStdString(engine);
        RegisterFunctions(engine);
        // Compile the script
        engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
        int result = builder.StartNewModule(engine, "ngtgame");
        result = builder.AddSectionFromFile(argv[1]);
        result = builder.BuildModule();

        if (result < 0) {

            alert("NGTBuilderError", "Failed to build script module: ");
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
            alert("NGTRuntimeError", "Script execution failed: ");
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
            alert("NGTRuntimeError", "Script execution failed: ");
            return 1;
        }

        // Clean up
        ctx->Release();
        engine->ShutDownAndRelease();


    }
    return EXIT_SUCCESS;
}