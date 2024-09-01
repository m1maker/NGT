#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <Windows.h>
#pragma section(".NGT")
#elif defined(__APPLE__)
#include <libproc.h>
#else
#include <unistd.h>
#endif
#include <filesystem>
#include "angelscript.h"
#include "contextmgr/contextmgr.h"
#include "datetime/datetime.h"
#include "docgen.h"
#include "ngt.h"
#include "ngtreg.h"
#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptdictionary/scriptdictionary.h"
#include "scriptfile/scriptfile.h"
#include "scriptfile/scriptfilesystem.h"
#include "scripthandle/scripthandle.h"
#include "scripthelper/scripthelper.h"
#include "scriptmath/scriptmath.h"
#include "scriptstdstring/scriptstdstring.h"
#include "scriptstdstring/scriptstdwstring.h"
#include "sdl3/sdl_main.h"
#include <assert.h>  // assert()
#include <cstdlib>
#include <fstream>
#include <thread>
#include "Poco/Glob.h"
#include "Poco/Path.h"

int IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam) {
	// 1. Resolve the relative path
	std::string absoluteIncludePath = Poco::Path(from).append("../" + std::string(include)).toString(); // Construct an absolute path
	// 2. Try to find the file directly
	if (builder->AddSectionFromFile(absoluteIncludePath.c_str()) > -1) {
		return 0;
	}

	// 3. Try the 'include' directory
	std::string includeDirectoryPath = Poco::Path("include", include).toString();
	if (builder->AddSectionFromFile(includeDirectoryPath.c_str()) > -1) {
		return 0;
	}

	// 4. Check for .as and .ngt extensions in both paths
	std::string currentAsPath = absoluteIncludePath + ".as";
	if (builder->AddSectionFromFile(currentAsPath.c_str()) > -1) {
		return 0;
	}
	std::string currentNgtPath = absoluteIncludePath + ".ngt";
	if (builder->AddSectionFromFile(currentNgtPath.c_str()) > -1) {
		return 0;
	}

	std::string includeAsPath = includeDirectoryPath + ".as";
	if (builder->AddSectionFromFile(includeAsPath.c_str()) > -1) {
		return 0;
	}
	std::string includeNgtPath = includeDirectoryPath + ".ngt";
	if (builder->AddSectionFromFile(includeNgtPath.c_str()) > -1) {
		return 0;
	}

	// 5. Handle wildcards
	std::set<std::string> matches;
	Poco::Glob::glob(absoluteIncludePath, matches);
	for (const auto& match : matches) {
		builder->AddSectionFromFile(match.c_str());
	}
	Poco::Glob::glob(includeDirectoryPath, matches);
	for (const auto& match : matches) {
		builder->AddSectionFromFile(match.c_str());
	}

	return 0;
}
CContextMgr context_manager;
CScriptBuilder builder;
static void crypt(std::vector<asBYTE>& bytes) {
	for (size_t i = 0; i < bytes.size(); ++i) {
		bytes[i] ^= bytes.size();
	}
}



static std::string get_exe() {
#ifdef _WIN32
	std::vector<wchar_t> pathBuf;
	DWORD copied = 0;
	do {
		pathBuf.resize(pathBuf.size() + MAX_PATH);
		copied = GetModuleFileNameW(0, &pathBuf.at(0), pathBuf.size());
	} while (copied >= pathBuf.size());
	pathBuf.resize(copied);
#elif defined(__APPLE__)
	std::vector<char> pathBuf;
	pid_t pid = getpid();
	int ret = proc_pidpath(pid, &pathBuf.at(0), pathBuf.size());
	if (ret <= 0) {
		return "";
	}
	pathBuf.resize(ret);
#else
	std::vector<char> pathBuf;
	ssize_t copied = 0;
	do {
		pathBuf.resize(pathBuf.size() + PATH_MAX);
		copied = readlink("/proc/self/exe", &pathBuf.at(0), pathBuf.size());
	} while (copied == static_cast<ssize_t>(pathBuf.size()));
	pathBuf.resize(copied);
	return std::string(pathBuf.begin(), pathBuf.end());
#endif
	return std::string(pathBuf.begin(), pathBuf.end());
}
static std::vector<std::string> string_split(const std::string& delim, const std::string& str)
{
	std::vector<std::string> array;

	if (delim.empty()) {
		array.push_back(str);
		return array;
	}

	size_t pos = 0, prev = 0;

	while ((pos = str.find(delim, prev)) != std::string::npos)
	{
		array.push_back(str.substr(prev, pos - prev));
		prev = pos + delim.length();
	}

	array.push_back(str.substr(prev));

	return array;
}
int PragmaCallback(const std::string& pragmaText, CScriptBuilder& builder, void* userParam) {
	if (pragmaText.empty())return -1;
	std::vector<std::string> split = string_split(" ", pragmaText);
	if (split.size() == 0)return -2;
	if (split[1] == "define") {
		builder.DefineWord(split[2].c_str());
	}
	return 0;
}



std::vector <asBYTE> buffer;
asUINT buffer_size;
bool SCRIPT_COMPILED = false;
int               ExecSystemCmd(const string& cmd);
int               ExecSystemCmd(const string& str, string& out);
CScriptArray* GetCommandLineArgs();
// The command line arguments
CScriptArray* g_commandLineArgs = 0;
int           g_argc = 0;
char** g_argv = 0;


class CBytecodeStream : public asIBinaryStream
{
public:
	std::vector<asBYTE> Code;
	int ReadPos, WritePos;

public:
	CBytecodeStream() : ReadPos(0), WritePos(0)
	{
	}

	CBytecodeStream(const std::vector<asBYTE>& Data) : Code(Data), ReadPos(0), WritePos(0)
	{
	}

	int Read(void* Ptr, asUINT Size) override
	{
		if (Ptr == nullptr || Size == 0)
		{
			return -1; // Error: corrupted read
		}

		if (ReadPos + Size > Code.size())
		{
			return -1; // Error: trying to read past end of stream
		}

		std::memcpy(Ptr, &Code[ReadPos], Size);
		ReadPos += Size;

		return 0;
	}

	int Write(const void* Ptr, asUINT Size) override
	{
		if (Ptr == nullptr || Size == 0)
		{
			return -1; // Error: corrupted write
		}

		Code.insert(Code.end(), static_cast<const asBYTE*>(Ptr), static_cast<const asBYTE*>(Ptr) + Size);
		WritePos += Size;

		return 0;
	}

	std::vector<asBYTE>& GetCode()
	{
		return Code;
	}

	asUINT GetSize() const
	{
		return static_cast<asUINT>(Code.size());
	}
};


static int Compile(asIScriptEngine* engine, const char* outputFile)
{
	int r;
	CBytecodeStream stream;
	asIScriptModule* mod = engine->GetModule("ngtgame");
	if (mod == 0)
	{
		engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");

		std::thread t(show_message);
		t.join();

		return -1;
	}

	r = mod->SaveByteCode(&stream, false);
	if (r < 0)
	{
		engine->WriteMessage(outputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");

		std::thread t(show_message);
		t.join();

		return -1;
	}
	buffer = stream.GetCode();
	buffer_size = stream.GetSize();
	alert("NGT", "Script was compiled successfully");


	return 0;
}

static int Load(asIScriptEngine* engine, std::vector<asBYTE> code)
{
	int r;
	CBytecodeStream stream;
	asIScriptModule* mod = engine->GetModule("ngtgame");
	if (mod == 0)
	{
		engine->WriteMessage("Product.ngt", 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");

		std::thread t(show_message);
		t.join();

		return -1;
	}
	stream.Code = code;
	r = mod->LoadByteCode(&stream);
	if (r < 0)
	{
		engine->WriteMessage("Product.ngt", 0, 0, asMSGTYPE_ERROR, "Failed to read the bytecode");

		std::thread t(show_message);
		t.join();

		return -1;
	}


	return 0;
}


std::string filename;
std::string flag;
int scriptArg = 0;
std::string this_exe;
auto main(int argc, char* argv[]) -> int {
	this_exe = get_exe();
	std::fstream read_file(this_exe.c_str(), std::ios::binary | std::ios::in);
	read_file.seekg(0, std::ios::end);
	long file_size = read_file.tellg();

	read_file.seekg(file_size - sizeof(asUINT));

	read_file.read(reinterpret_cast<char*>(&buffer_size), sizeof(asUINT));
	if (buffer_size != NULL) {
		filename = "";
		flag = "-b";
	}
	else {
		if (argc < 2) {
			asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
			engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
			show_console();
			engine->WriteMessage(get_exe().c_str(), 0, 0, asMSGTYPE_INFORMATION, "Something went wrong when starting the engine.\r\nNothing to debug, nothing to compile.\r\nArguments and flags that can be used:\r\n\"NGTScript.exe <filename> -d\" - Debug a script.\r\n\"NGTScript.exe <filename> -c\" - Compile a script to executable file.\r\n\"NGTScript.exe <output file> -i\" - Write engine config to a file.");
			engine->ShutDownAndRelease();
			ExecSystemCmd("pause");
			hide_console();
			return -1;
		}

		filename = argv[1];
		flag = argv[2];
	}
	g_argc = argc - (scriptArg + 1);
	g_argv = argv + (scriptArg + 1);

	if (flag == "-c") {
		asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		// Register any necessary functions and types
		// ...
		RegisterStdString(engine);
		RegisterStdWstring(engine);
		RegisterScriptArray(engine, true);
		RegisterStdStringUtils(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptDateTime(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);
		RegisterExceptionRoutines(engine);
		RegisterScriptMath(engine);
		RegisterScriptHandle(engine);
		RegisterScriptAny(engine);
		context_manager.RegisterThreadSupport(engine);
		context_manager.RegisterCoRoutineSupport(engine);
		RegisterFunctions(engine);
		engine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const string&), int), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const string&, string&), int), asCALL_CDECL);
		engine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED);
		// Compile the script
		builder.SetPragmaCallback(PragmaCallback, nullptr);
		builder.SetIncludeCallback(IncludeCallback, nullptr);
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
		std::string main_exe = get_exe();
		std::vector<std::string> name_split = string_split(".", filename);
		std::filesystem::copy_file(main_exe.c_str(), name_split[0] + ".exe");
		std::fstream file(name_split[0] + ".exe", std::ios::app | std::ios::binary);
		if (!file.is_open()) {
			engine->WriteMessage(this_exe.c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to open output file for writing");

			std::thread t(show_message);
			t.join();
			return -1;
		}

		file.seekg(0, std::ios::end);
		long file_size = file.tellg();
		file.write("\r\n.rdata", strlen("\r\n.rdata"));
		crypt(buffer);
		file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
		buffer_size = buffer.size();
		file.write(reinterpret_cast<char*>(&buffer_size), sizeof(asUINT));
		file.close();

	}


	else if (flag == "-d") {
		asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		// Register any necessary functions and types
		// ...
		RegisterStdString(engine);
		RegisterStdWstring(engine);
		RegisterScriptArray(engine, true);
		RegisterStdStringUtils(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptDateTime(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);
		RegisterExceptionRoutines(engine);
		RegisterScriptMath(engine);
		RegisterScriptHandle(engine);
		RegisterScriptAny(engine);
		context_manager.RegisterThreadSupport(engine);
		context_manager.RegisterCoRoutineSupport(engine);
		RegisterFunctions(engine);
		engine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const string&), int), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const string&, string&), int), asCALL_CDECL);
		engine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED);


		// Compile the script
		builder.SetPragmaCallback(PragmaCallback, nullptr);
		builder.SetIncludeCallback(IncludeCallback, nullptr);

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
		asIScriptFunction* func = engine->GetModule("ngtgame")->GetFunctionByName("main");
		if (func == 0)
		{
			alert("NGTExecutableError", "No entry point found (either 'int main()' or 'void main()'.)");
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
		init_engine();
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
		SCRIPT_COMPILED = true;
		asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		RegisterStdString(engine);
		RegisterStdWstring(engine);
		RegisterScriptArray(engine, true);
		RegisterStdStringUtils(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptDateTime(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);
		RegisterExceptionRoutines(engine);
		RegisterScriptMath(engine);
		RegisterScriptHandle(engine);
		RegisterScriptAny(engine);
		context_manager.RegisterThreadSupport(engine);
		context_manager.RegisterCoRoutineSupport(engine);
		RegisterFunctions(engine);
		engine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const string&), int), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const string&, string&), int), asCALL_CDECL);
		engine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED);


		asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
		int result;
		module = engine->GetModule("ngtgame");
		if (module)
		{
			std::fstream read_file(this_exe.c_str(), std::ios::binary | std::ios::in);
			if (read_file.is_open()) {
				read_file.seekg(0, std::ios::end);
				long file_size = read_file.tellg();
				read_file.seekg(file_size - sizeof(asUINT));

				read_file.read(reinterpret_cast<char*>(&buffer_size), sizeof(asUINT));

				read_file.seekg(file_size - buffer_size - 4, std::ios::beg);
				buffer.resize(buffer_size);
				read_file.read(reinterpret_cast<char*>(buffer.data()), buffer_size);
				crypt(buffer);

				read_file.close();
			}
			else {
				engine->WriteMessage(this_exe.c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to open output file for reading");

				std::thread t(show_message);
				t.join();
				return -1;
			}


			Load(engine, buffer);
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
		asIScriptFunction* func = engine->GetModule("ngtgame")->GetFunctionByName("main");
		if (func == 0)
		{
			alert("NGTExecutableError", "No entry point found (either 'int main()' or 'void main()'.)");
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
		init_engine();
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
	else if (flag == "-i") {
		asIScriptEngine* engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		RegisterStdString(engine);
		RegisterStdWstring(engine);
		RegisterScriptArray(engine, true);
		RegisterStdStringUtils(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptDateTime(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);
		RegisterExceptionRoutines(engine);
		RegisterScriptMath(engine);
		RegisterScriptHandle(engine);
		RegisterScriptAny(engine);
		context_manager.RegisterThreadSupport(engine);
		context_manager.RegisterCoRoutineSupport(engine);
		RegisterFunctions(engine);
		engine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const string&), int), asCALL_CDECL);
		engine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const string&, string&), int), asCALL_CDECL);
		engine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED);
		ScriptDocumentationOptions sd;
		sd.addTimestamp = true;
		sd.documentationName = "NGT engine documentation";
		sd.htmlSafe = true;
		sd.includeArrayInterface = true;
		sd.includeRefInterface = true;
		sd.includeStringInterface = true;
		sd.includeWeakRefInterface = true;
		sd.projectName = "NGT";
		sd.outputFile = filename.c_str();
		DocumentationGenerator dg(engine, sd);
		dg.Generate();
		engine->ShutDownAndRelease();

	}
	exit_engine();
	return EXIT_SUCCESS;
}
int ExecSystemCmd(const string& str, string& out)
{
#ifdef _WIN32
	// Convert the command to UTF16 to properly handle unicode path names
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, bufUTF16, 10000);

	// Create a pipe to capture the stdout from the system command
	HANDLE pipeRead, pipeWrite;
	SECURITY_ATTRIBUTES secAttr = { 0 };
	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttr.bInheritHandle = TRUE;
	secAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&pipeRead, &pipeWrite, &secAttr, 0))
		return -1;

	// Start the process for the system command, informing the pipe to 
	// capture stdout, and also to skip showing the command window
	STARTUPINFOW si = { 0 };
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = pipeWrite;
	si.hStdError = pipeWrite;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi = { 0 };
	BOOL success = CreateProcessW(NULL, bufUTF16, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
	if (!success)
	{
		CloseHandle(pipeWrite);
		CloseHandle(pipeRead);
		return -1;
	}

	// Run the command until the end, while capturing stdout
	for (;;)
	{
		// Wait for a while to allow the process to work
		DWORD ret = WaitForSingleObject(pi.hProcess, 50);

		// Read from the stdout if there is any data
		for (;;)
		{
			char buf[1024];
			DWORD readCount = 0;
			DWORD availCount = 0;

			if (!::PeekNamedPipe(pipeRead, NULL, 0, NULL, &availCount, NULL))
				break;

			if (availCount == 0)
				break;

			if (!::ReadFile(pipeRead, buf, sizeof(buf) - 1 < availCount ? sizeof(buf) - 1 : availCount, &readCount, NULL) || !readCount)
				break;

			buf[readCount] = 0;
			out += buf;
		}

		// End the loop if the process finished
		if (ret == WAIT_OBJECT_0)
			break;
	}

	// Get the return status from the process
	DWORD status = 0;
	GetExitCodeProcess(pi.hProcess, &status);

	CloseHandle(pipeRead);
	CloseHandle(pipeWrite);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return status;
#else
	// TODO: Implement suppor for ExecSystemCmd(const string &, string&) on non-Windows platforms
	asIScriptContext* ctx = asGetActiveContext();
	if (ctx)
		ctx->SetException("Oops! This is not yet implemented on non-Windows platforms. Sorry!\n");
	return -1;
#endif
}

// This function simply calls the system command and returns the status
// Return of -1 indicates an error. Else the return code is the return status of the executed command
int ExecSystemCmd(const string& str)
{
	// Check if the command line processor is available
	if (system(0) == 0)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Command interpreter not available\n");
		return -1;
	}

#ifdef _WIN32
	// Convert the command to UTF16 to properly handle unicode path names
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, bufUTF16, 10000);
	return _wsystem(bufUTF16);
#else
	return system(str.c_str());
#endif
}

// This function returns the command line arguments that were passed to the script
CScriptArray* GetCommandLineArgs()
{
	if (g_commandLineArgs)
	{
		g_commandLineArgs->AddRef();
		return g_commandLineArgs;
	}

	// Obtain a pointer to the engine
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();

	// Create the array object
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	g_commandLineArgs = CScriptArray::Create(arrayType, (asUINT)0);

	// Find the existence of the delimiter in the input string
	for (int n = 0; n < g_argc; n++)
	{
		// Add the arg to the array
		g_commandLineArgs->Resize(g_commandLineArgs->GetSize() + 1);
		((string*)g_commandLineArgs->At(n))->assign(g_argv[n]);
	}

	// Return the array by handle
	g_commandLineArgs->AddRef();
	return g_commandLineArgs;
}

