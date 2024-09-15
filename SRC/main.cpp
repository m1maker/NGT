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
#include "Poco/SHA2Engine.h"
#include "debugger/debugger.h"
#include "aes/aes.hpp"
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
#include "Poco/Exception.h"


int IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* userParam) {
	// 1. Resolve the relative path
	std::string absoluteIncludePath = Poco::Path(from).append("../" + std::string(include)).toString(); // Construct an absolute path
	// 2. Try to find the file directly
	if (builder->AddSectionFromFile(absoluteIncludePath.c_str()) > -1) {
		g_ScriptMessagesError = "";
		return 0;
	}

	// 3. Try the 'include' directory
	std::string includeDirectoryPath = Poco::Path("include", include).toString();
	if (builder->AddSectionFromFile(includeDirectoryPath.c_str()) > -1) {
		g_ScriptMessagesError = "";
		return 0;
	}

	// 4. Check for .as and .ngt extensions in both paths
	std::string currentAsPath = absoluteIncludePath + ".as";
	if (builder->AddSectionFromFile(currentAsPath.c_str()) > -1) {
		g_ScriptMessagesError = "";
		return 0;
	}
	std::string currentNgtPath = absoluteIncludePath + ".ngt";
	if (builder->AddSectionFromFile(currentNgtPath.c_str()) > -1) {
		g_ScriptMessagesError = "";
		return 0;
	}

	std::string includeAsPath = includeDirectoryPath + ".as";
	if (builder->AddSectionFromFile(includeAsPath.c_str()) > -1) {
		g_ScriptMessagesError = "";
		return 0;
	}
	std::string includeNgtPath = includeDirectoryPath + ".ngt";
	if (builder->AddSectionFromFile(includeNgtPath.c_str()) > -1) {
		g_ScriptMessagesError = "";
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
	g_ScriptMessagesError = "";
	return 0;
}
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

std::vector<std::string> defines;

int PragmaCallback(const std::string& pragmaText, CScriptBuilder& builder, void* userParam) {
	const std::string definePrefix = " define ";
	if (pragmaText.compare(0, definePrefix.length(), definePrefix) == 0) {
		std::string word = pragmaText.substr(definePrefix.length());

		word.erase(0, word.find_first_not_of(" \t\r\n"));
		word.erase(word.find_last_not_of(" \t\r\n") + 1);
		defines.push_back(word);
		return 0;
	}

	return -1;
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


void vector_pad(std::vector<unsigned char>& text) {
	int padding_size = 16 - (text.size() % 16);
	if (padding_size == 0) {
		padding_size = 16;
	}
	text.insert(text.end(), padding_size, static_cast<unsigned char>(padding_size));
}

void vector_unpad(std::vector<unsigned char>& text) {
	int padding_size = static_cast<unsigned char>(text.back());
	if (padding_size > 0 && padding_size <= 16) {
		text.resize(text.size() - padding_size);
	}
}

std::vector<unsigned char> vector_encrypt(const std::vector<unsigned char>& str, const std::string& encryption_key) {
	std::vector<unsigned char> the_vector = str;
	Poco::SHA2Engine hash;
	hash.update(encryption_key);
	const unsigned char* key_hash = hash.digest().data();

	unsigned char iv[16];
	for (int i = 0; i < 16; ++i) {
		iv[i] = key_hash[i * 2] ^ (4 * i + 1);
	}

	AES_ctx crypt;
	AES_init_ctx_iv(&crypt, key_hash, iv);
	vector_pad(the_vector);
	AES_CBC_encrypt_buffer(&crypt, the_vector.data(), the_vector.size());

	// Clear sensitive data
	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt), reinterpret_cast<uint8_t*>(&crypt) + sizeof(AES_ctx), 0);

	return the_vector;
}

std::vector<unsigned char> vector_decrypt(const std::vector<unsigned char>& str, const std::string& encryption_key) {
	if (str.size() % 16 != 0) return {};

	std::vector<unsigned char> the_vector = str;
	Poco::SHA2Engine hash;
	hash.update(encryption_key);
	const unsigned char* key_hash = hash.digest().data();

	unsigned char iv[16];
	for (int i = 0; i < 16; ++i) {
		iv[i] = key_hash[i * 2] ^ (4 * i + 1);
	}

	AES_ctx crypt;
	AES_init_ctx_iv(&crypt, key_hash, iv);
	AES_CBC_decrypt_buffer(&crypt, the_vector.data(), the_vector.size());

	// Clear sensitive data
	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt), reinterpret_cast<uint8_t*>(&crypt) + sizeof(AES_ctx), 0);

	vector_unpad(the_vector);
	return the_vector;
}



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


asIScriptModule* Compile(asIScriptEngine* engine, const char* inputFile)
{
	builder.SetPragmaCallback(PragmaCallback, nullptr);
	builder.SetIncludeCallback(IncludeCallback, nullptr);
	asIScriptModule* module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
	int result = builder.StartNewModule(engine, "ngtgame");
	result = builder.AddSectionFromFile(inputFile);
	if (defines.size() > 0) {
		// Now, restart all, because we need to define all words before adding section
		builder.ClearAll();
		module = engine->GetModule("ngtgame", asGM_ALWAYS_CREATE);
		result = builder.StartNewModule(engine, "ngtgame");
		for (uint64_t i = 0; i < defines.size(); ++i) {
			builder.DefineWord(defines[i].c_str());
		}
		result = builder.AddSectionFromFile(inputFile);

	}
	result = builder.BuildModule();

	if (result < 0) {
		std::thread t(show_message);
		t.join();

		return nullptr;
	}
	CBytecodeStream stream;
	if (module == 0)
	{
		engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to retrieve the compiled bytecode");

		std::thread t(show_message);
		t.join();

		return nullptr;
	}

	result = module->SaveByteCode(&stream, true);
	if (result < 0)
	{
		engine->WriteMessage(inputFile, 0, 0, asMSGTYPE_ERROR, "Failed to write the bytecode");

		std::thread t(show_message);
		t.join();

		return nullptr;
	}
	buffer = stream.GetCode();
	buffer_size = stream.GetSize();
	return module;
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



std::string StringToString(void* obj, int /* expandMembers */, CDebugger* /* dbg */)
{
	// We know the received object is a string
	std::string* val = reinterpret_cast<std::string*>(obj);

	// Format the output string
	// TODO: Should convert non-readable characters to escape sequences
	std::stringstream s;
	s << "(len=" << val->length() << ") \"";
	if (val->length() < 20)
		s << *val << "\"";
	else
		s << val->substr(0, 20) << "...";

	return s.str();
}

// This is the to-string callback for the array type
// This is generic and will take care of all template instances based on the array template
std::string ArrayToString(void* obj, int expandMembers, CDebugger* dbg)
{
	CScriptArray* arr = reinterpret_cast<CScriptArray*>(obj);

	std::stringstream s;
	s << "(len=" << arr->GetSize() << ")";

	if (expandMembers > 0)
	{
		s << " [";
		for (asUINT n = 0; n < arr->GetSize(); n++)
		{
			s << dbg->ToString(arr->At(n), arr->GetElementTypeId(), expandMembers - 1, arr->GetArrayObjectType()->GetEngine());
			if (n < arr->GetSize() - 1)
				s << ", ";
		}
		s << "]";
	}

	return s.str();
}

// This is the to-string callback for the dictionary type
std::string DictionaryToString(void* obj, int expandMembers, CDebugger* dbg)
{
	CScriptDictionary* dic = reinterpret_cast<CScriptDictionary*>(obj);

	std::stringstream s;
	s << "(len=" << dic->GetSize() << ")";

	if (expandMembers > 0)
	{
		s << " [";
		asUINT n = 0;
		for (CScriptDictionary::CIterator it = dic->begin(); it != dic->end(); it++, n++)
		{
			s << "[" << it.GetKey() << "] = ";

			// Get the type and address of the value
			const void* val = it.GetAddressOfValue();
			int typeId = it.GetTypeId();

			// Use the engine from the currently active context (if none is active, the debugger
			// will use the engine held inside it by default, but in an environment where there
			// multiple engines this might not be the correct instance).
			asIScriptContext* ctx = asGetActiveContext();

			s << dbg->ToString(const_cast<void*>(val), typeId, expandMembers - 1, ctx ? ctx->GetEngine() : 0);

			if (n < dic->GetSize() - 1)
				s << ", ";
		}
		s << "]";
	}

	return s.str();
}

// This is the to-string callback for the dictionary type
std::string DateTimeToString(void* obj, int expandMembers, CDebugger* dbg)
{
	CDateTime* dt = reinterpret_cast<CDateTime*>(obj);

	std::stringstream s;
	s << "{" << dt->getYear() << "-" << dt->getMonth() << "-" << dt->getDay() << " ";
	s << dt->getHour() << ":" << dt->getMinute() << ":" << dt->getSecond() << "}";

	return s.str();
}



asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* param);
void              ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* param);



class NGTScripting {
public:
	asIScriptEngine* scriptEngine;
	asIScriptContext* scriptContext;
	CContextMgr* m_ctxMgr = 0;
	std::vector<asIScriptContext*> m_ctxPool;
	CDebugger* m_dbg = 0;
	NGTScripting() {
		scriptEngine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if (scriptEngine == nullptr) {
			std::cout << "Failed to create the script engine." << std::endl;
			std::exit(-1);
		}
		scriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	}
	~NGTScripting() {
		if (m_ctxMgr != 0) {
			delete m_ctxMgr;
			m_ctxMgr = nullptr;
		}
		if (m_dbg)
		{
			delete m_dbg;
			m_dbg = 0;
		}

		for (auto ctx : m_ctxPool)
			ctx->Release();
		m_ctxPool.clear();

		scriptEngine->ClearMessageCallback();
		scriptEngine->ShutDownAndRelease();
	}

	void InitializeDebugger()
	{
		// Create the debugger instance and store it so the context callback can attach
		// it to the scripts contexts that will be used to execute the scripts
		m_dbg = new CDebugger();

		// Let the debugger hold an engine pointer that can be used by the callbacks
		m_dbg->SetEngine(scriptEngine);

		// Register the to-string callbacks so the user can see the contents of strings
		m_dbg->RegisterToStringCallback(scriptEngine->GetTypeInfoByName("string"), StringToString);
		m_dbg->RegisterToStringCallback(scriptEngine->GetTypeInfoByName("array"), ArrayToString);
		m_dbg->RegisterToStringCallback(scriptEngine->GetTypeInfoByName("dictionary"), DictionaryToString);
		m_dbg->RegisterToStringCallback(scriptEngine->GetTypeInfoByName("datetime"), DateTimeToString);

		// Allow the user to initialize the debugging before moving on
		cout << "Debugging, waiting for commands. Type 'h' for help." << endl;
		m_dbg->TakeCommands(0);
	}


	void RegisterStd() {
		RegisterStdString(scriptEngine);
		RegisterStdWstring(scriptEngine);
		RegisterScriptArray(scriptEngine, true);
		RegisterStdStringUtils(scriptEngine);
		RegisterScriptDictionary(scriptEngine);
		RegisterScriptDateTime(scriptEngine);
		RegisterScriptFile(scriptEngine);
		RegisterScriptFileSystem(scriptEngine);
		RegisterExceptionRoutines(scriptEngine);
		RegisterScriptMath(scriptEngine);
		RegisterScriptHandle(scriptEngine);
		RegisterScriptAny(scriptEngine);
		RegisterFunctions(scriptEngine);
		scriptEngine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const string&), int), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const string&, string&), int), asCALL_CDECL);
		scriptEngine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED);
		m_ctxMgr = new CContextMgr();
		m_ctxMgr->RegisterCoRoutineSupport(scriptEngine);

		// Tell the engine to use our context pool. This will also 
		// allow us to debug internal script calls made by the engine
		scriptEngine->SetContextCallbacks(RequestContextCallback, ReturnContextCallback, this);

	}
	int Exec(asIScriptFunction* func) {
		if (func == nullptr)return -1;
		int r;
		try {
			scriptContext = m_ctxMgr->AddContext(scriptEngine, func, true);

			// Execute the script until completion
			// The script may create co-routines. These will automatically
			// be managed by the context manager
			while (m_ctxMgr->ExecuteScripts());

			// Check if the main script finished normally
			r = scriptContext->GetState();
			if (r != asEXECUTION_FINISHED)
			{
				if (r == asEXECUTION_EXCEPTION)
				{
					alert("NGTRuntimeError", GetExceptionInfo(scriptContext, true));
					r = -1;
				}
				else if (r == asEXECUTION_ABORTED)
				{
					r = 0;
				}
				else
				{
					r = -1;
				}
			}
			else
			{
				// Get the return value from the script
				if (func->GetReturnTypeId() == asTYPEID_INT32)
				{
					r = *(int*)scriptContext->GetAddressOfReturnValue();
				}
				else
					r = 0;
			}

			// Return the context after retrieving the return value
			m_ctxMgr->DoneWithContext(scriptContext);
			return r;
		}
		catch (const std::exception& e) {
			alert("NGTRuntimeError", e.what());
			return -1;
		}
		catch (const Poco::Exception& e) {
			alert("NGTRuntimeError", e.displayText());
			return -1;
		}
		catch (...) {
			alert("NGTRuntimeError", "Unknown exception.");
			return -1;
		}
		return 0;
	};
};

asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* param)
{
	NGTScripting* instance = (NGTScripting*)param;
	asIScriptContext* ctx = 0;

	// Check if there is a free context available in the pool
	if (instance->m_ctxPool.size())
	{
		ctx = instance->m_ctxPool.back();
		instance->m_ctxPool.pop_back();
	}
	else
	{
		// No free context was available so we'll have to create a new one
		ctx = engine->CreateContext();
	}

	// Attach the debugger if needed
	if (ctx && instance->m_dbg)
	{
		// Set the line callback for the debugging
		ctx->SetLineCallback(asMETHOD(CDebugger, LineCallback), instance->m_dbg, asCALL_THISCALL);
	}

	return ctx;
}

// This function is called by the engine when the context is no longer in use
void ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* param)
{
	NGTScripting* instance = (NGTScripting*)param;
	// We can also check for possible script exceptions here if so desired

	// Unprepare the context to free any objects it may still hold (e.g. return value)
	// This must be done before making the context available for re-use, as the clean
	// up may trigger other script executions, e.g. if a destructor needs to call a function.
	ctx->Unprepare();

	// Place the context into the pool for when it will be needed again
	instance->m_ctxPool.push_back(ctx);
}







std::string filename;
std::string flag;
int scriptArg = 0;
std::string this_exe;
NGTScripting app;
CContextMgr* get_context_manager() {
	return app.m_ctxMgr;
}
auto main(int argc, char* argv[]) -> int {
	asIScriptEngine* engine = app.scriptEngine;
	app.RegisterStd();
	asIScriptModule* module = nullptr;
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
			engine->WriteMessage(get_exe().c_str(), 0, 0, asMSGTYPE_INFORMATION, "Something went wrong when starting the engine.\r\nNothing to debug, nothing to compile.\r\nArguments and flags that can be used:\r\n\"NGTScript.exe <filename> -d\" - Debug a script.\r\n\"NGTScript.exe <filename> -c\" - Compile a script to executable file.\r\n\"NGTScript.exe <output file> -i\" - Write engine config to a file.");
			return -1;
		}

		filename = argv[1];
		flag = argv[2];
	}
	g_argc = argc - (scriptArg + 1);
	g_argv = argv + (scriptArg + 1);

	if (flag == "-c") {
		// Compile the script
		module = Compile(engine, argv[1]);
		if (module == nullptr) {
			std::cout << "Failed to compile script." << std::endl;
			return -1;
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
		buffer = vector_encrypt(buffer, string_base64_encode(NGT_BYTECODE_ENCRYPTION_KEY));
		file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
		buffer_size = buffer.size();
		file.write(reinterpret_cast<char*>(&buffer_size), sizeof(asUINT));
		file.close();

	}


	else if (flag == "-r") {
		// Compile the script
		module = Compile(engine, argv[1]);
		if (module == nullptr) {
			std::cout << "Failed to compile script." << std::endl;
			return -1;
		}
		// Execute the script
		asIScriptFunction* func = module->GetFunctionByName("main");
		if (func == 0) {
			std::cout << "Failed to invoke main." << std::endl;
			return 1;
		}
		init_engine();
		int result = app.Exec(func);
		return result;
	}
	else if (flag == "-d") {
		// Compile the script
		module = Compile(engine, argv[1]);
		if (module == nullptr) {
			std::cout << "Failed to compile script." << std::endl;
			return -1;
		}
		// Execute the script
		asIScriptFunction* func = module->GetFunctionByName("main");
		if (func == 0) {
			std::cout << "Failed to invoke main." << std::endl;
			return 1;
		}
		init_engine();
		app.InitializeDebugger();
		int result = app.Exec(func);
		return result;
	}

	else if (flag == "-b") {
		SCRIPT_COMPILED = true;
		// Execute the script
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
				buffer = vector_decrypt(buffer, string_base64_encode(NGT_BYTECODE_ENCRYPTION_KEY));

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
		asIScriptFunction* func = module->GetFunctionByName("main");
		if (func == 0) {
			std::cout << "Failed to invoke main." << std::endl;
			return 1;
		}
		init_engine();
		result = app.Exec(func);
		return result;
	}
	else if (flag == "-i") {
		WriteConfigToFile(engine, filename.c_str());
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

