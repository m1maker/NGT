#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <excpt.h>
#include <Windows.h>
// For PE Resource Update
#pragma comment(lib, "Version.lib") // For GetFileVersionInfoSize, GetFileVersionInfo, VerQueryValue
#else
#include <unistd.h>
#include <csignal>
#include <execinfo.h>
#endif

#include <Poco/UnicodeConverter.h>
#include "obfuscate.h"
#include "sound.h"
#include <SRAL.h>
#include <filesystem>
#include <format>
#include <angelscript.h>
#include "contextmgr/contextmgr.h"
#include <Poco/SHA2Engine.h>
#include <Poco/Util/Application.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/AutoPtr.h>
#include <Poco/Path.h>
#include <Poco/Glob.h>
#include <Poco/Exception.h>
#include <Poco/StreamCopier.h>


#include "debugger/debugger.h"
#include "AES/aes.hpp"
#include "datetime/datetime.h"
#include "docgen.h"
#include "ngt.h"
#include "ngtreg.h"
#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scriptbuilder/scriptbuilder.h"
#include "scriptdictionary/scriptdictionary.h"
#include "scriptfile/scriptfile.h"
#include "scripthandle/scripthandle.h"
#include "scripthelper/scripthelper.h"
#include "scriptmath/scriptmath.h"
#include "scriptmath/scriptmathcomplex.h"
#include "scriptstdstring/scriptstdstring.h"
#include "scriptstdstring/scriptstdwstring.h"

#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <thread>
#include <vector>
#include <string>
#include <sstream>
#include <iostream> // For cout/cerr
#include <algorithm> // For std::fill


#define SDL_MAIN_HANDLED

#define NGT_BYTECODE_ENCRYPTION_KEY "0Z1Eif2JShwWsaAfgw1EfiOwudDAnNg6WdsIuwyTgsJAiw(us)wjHdc87&6w()ErreOiduYRREoEiDKSodoWJritjH;kJSjwjifhaASfdfvV"

// Resource defines for Windows
#ifdef _WIN32
#define NGT_BYTECODE_RESOURCE_TYPE_W L"NGT_BYTECODE"
#define NGT_BYTECODE_RESOURCE_ID_W   MAKEINTRESOURCE(1)
#endif

// Signature for appended bytecode on non-Windows
const char* NGT_BYTECODE_FILE_SIGNATURE = "NGTBC_EOF";
const size_t NGT_BYTECODE_FILE_SIGNATURE_LEN = strlen(NGT_BYTECODE_FILE_SIGNATURE);


#if defined(_MSC_VER)
#include <crtdbg.h>
class MemoryLeakDetector
{
public:
	MemoryLeakDetector()
	{
		_CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
		_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
		//_CrtSetBreakAlloc(124);
	}
} g_leakDetector;
#endif


#ifdef _WIN32
#include <dbghelp.h>
LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* exceptionInfo) {
	std::stringstream ss;
	ss << "Caught an access violation (segmentation fault)." << std::endl;
	ULONG_PTR faultingAddress = exceptionInfo->ExceptionRecord->ExceptionInformation[1];
	ss << "Faulting address: 0x" << std::hex << faultingAddress << std::dec << std::endl;

	void* stack[100];
	unsigned short frames;
	SYMBOL_INFO* symbol;
	HANDLE process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);
	frames = CaptureStackBackTrace(0, 100, stack, NULL);
	symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	if (symbol) { // calloc can fail
		symbol->MaxNameLen = 255;
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		for (unsigned short i = 0; i < frames; i++) {
			SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
			ss << i << ": " << symbol->Name << " - 0x" << number_to_hex_string(symbol->Address) << std::endl;
		}
		free(symbol);
	}
	else {
		ss << "Failed to allocate memory for symbol information." << std::endl;
	}
	SymCleanup(process); // Cleanup symbol handler
	alert("NGTRuntimeError", ss.str());
	// TerminateProcess is a forceful way to exit. Consider if a cleaner shutdown is possible.
	// For a critical fault like this, TerminateProcess might be appropriate to prevent further corruption.
	TerminateProcess(GetCurrentProcess(), 1);
	return EXCEPTION_EXECUTE_HANDLER; // Should not be reached if TerminateProcess succeeds
}
#else
void signalHandler(int signal, siginfo_t* info, void* context) {
	std::cerr << "Caught signal " << signal << " (Segmentation fault)." << std::endl;
	if (info) {
		std::cerr << "Faulting address: " << info->si_addr << std::endl;
	}
	void* array[20]; // Increased buffer size
	size_t size;
	size = backtrace(array, 20);
	std::cerr << "Stack trace:" << std::endl;
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
}
#endif

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;
using Poco::AutoPtr;

bool g_shutdown = false; // Consider managing this within application state
int g_retcode = 0;      // Used by script's potential custom exit, review its necessity


static std::string get_exe_path_helper() {
	return Poco::Util::Application::instance().config().getString("application.dir");
}

static std::string get_exe_helper() {
	return Poco::Util::Application::instance().config().getString("application.path");
}


int IncludeCallback(const char* include, const char* from, CScriptBuilder* builder, void* /*userParam*/) {
	std::filesystem::path fromPath(from);
	std::filesystem::path currentDir = fromPath.parent_path();
	std::filesystem::path includePath(include);

	std::vector<std::filesystem::path> pathsToTry;

	// 1. Relative to current script file
	pathsToTry.push_back(currentDir / includePath);

	// 2. Relative to 'include' directory alongside executable
	std::filesystem::path exeDir(get_exe_path_helper());
	pathsToTry.push_back(exeDir / "include" / includePath);

	// Extensions to try
	const char* extensions[] = { "", ".as", ".ngt" };

	for (const auto& basePath : pathsToTry) {
		for (const char* ext : extensions) {
			std::filesystem::path testPath = basePath;
			if (strlen(ext) > 0 && basePath.extension() != ext) { // Avoid double extension if already present
				testPath.replace_extension(ext);
			}
			// If no extension or trying "" extension, use original path
			if (strlen(ext) == 0) testPath = basePath;


			if (std::filesystem::exists(testPath) && std::filesystem::is_regular_file(testPath)) {
				if (builder->AddSectionFromFile(testPath.string().c_str()) >= 0) {
					g_ScriptMessagesError = "";
					return 0;
				}
			}
		}

		// Handle wildcards (basic support, Poco::Glob is more powerful)
		std::set<std::string> matches;
		// Try globbing on the basePath as a pattern (if 'include' contained wildcards)
		try {
			Poco::Glob::glob(basePath.string(), matches);
			bool added_any = false;
			for (const auto& match : matches) {
				if (std::filesystem::exists(match) && std::filesystem::is_regular_file(match)) {
					if (builder->AddSectionFromFile(match.c_str()) >= 0) {
						added_any = true;
					}
				}
			}
			if (added_any) {
				g_ScriptMessagesError = "";
				return 0;
			}
		}
		catch (const Poco::Exception&) {
		}

		return -1; // File not found
	}
	return -1;
}

static inline void TrimWhitespace(std::string& str) {
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
}

int PragmaCallback(const std::string& pragmaText, CScriptBuilder& builder, void* /*userParam*/) {
	const std::string definePrefix = "define ";
	std::string text = pragmaText;
	TrimWhitespace(text);

	if (text.rfind(definePrefix, 0) == 0) {
		std::string word = text.substr(definePrefix.length());
		TrimWhitespace(word);
		if (!word.empty()) {
			builder.DefineWord(word.c_str());
			return 0;
		}
	}
	return -1;
}

// Bytecode manipulation: XOR "obfuscation"
static inline void apply_simple_obfuscation(std::vector<asBYTE>& bytes) {
	if (bytes.empty()) return; // Avoid issues with bytes.size() being 0 for XOR key
	asBYTE key = static_cast<asBYTE>(bytes.size() & 0xFF); // Use (size % 256) as simple key
	if (key == 0) key = 1; // Ensure key is not zero if size is a multiple of 256
	for (size_t i = 0; i < bytes.size(); ++i) {
		bytes[i] ^= key;
	}
}

// AES Padding
void vector_pad_aes(std::vector<unsigned char>& text) {
	size_t padding_size = AES_BLOCKLEN - (text.size() % AES_BLOCKLEN);
	if (padding_size == 0) {
		padding_size = AES_BLOCKLEN;
	}
	text.insert(text.end(), padding_size, static_cast<unsigned char>(padding_size));
}

void vector_unpad_aes(std::vector<unsigned char>& text) {
	if (text.empty()) return;
	size_t padding_size = static_cast<unsigned char>(text.back());
	if (padding_size > 0 && padding_size <= AES_BLOCKLEN && padding_size <= text.size()) {
		text.resize(text.size() - padding_size);
	}
	else {
		std::cerr << "Warning: Invalid AES unpadding size detected: " << padding_size << std::endl;
	}
}

// AES Encryption/Decryption
static std::vector<unsigned char> aes_encrypt_vector(const std::vector<unsigned char>& data, const std::string& encryption_key_material) {
	std::vector<unsigned char> result_data = data;
	Poco::SHA2Engine hash(Poco::SHA2Engine::SHA_256);
	hash.update(encryption_key_material);
	const Poco::DigestEngine::Digest& key_digest_vec = hash.digest();
	const unsigned char* aes_key = key_digest_vec.data();

	unsigned char iv[AES_BLOCKLEN];
	for (int i = 0; i < AES_BLOCKLEN; ++i) {
		// Simple IV derivation from hash, ensure it's different from key parts if possible
		iv[i] = key_digest_vec[(i * 2) % key_digest_vec.size()] ^ static_cast<unsigned char>(4 * i + 1);
	}

	AES_ctx crypt_ctx;
	AES_init_ctx_iv(&crypt_ctx, aes_key, iv);

	vector_pad_aes(result_data);
	AES_CBC_encrypt_buffer(&crypt_ctx, result_data.data(), result_data.size());

	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt_ctx), reinterpret_cast<uint8_t*>(&crypt_ctx) + sizeof(AES_ctx), 0);
	return result_data;
}

static std::vector<unsigned char> aes_decrypt_vector(const std::vector<unsigned char>& encrypted_data, const std::string& encryption_key_material) {
	if (encrypted_data.empty() || encrypted_data.size() % AES_BLOCKLEN != 0) {
		std::cerr << "Error: Encrypted data size is invalid for AES decryption." << std::endl;
		return {};
	}

	std::vector<unsigned char> result_data = encrypted_data;
	Poco::SHA2Engine hash(Poco::SHA2Engine::SHA_256);
	hash.update(encryption_key_material);
	const Poco::DigestEngine::Digest& key_digest_vec = hash.digest();
	const unsigned char* aes_key = key_digest_vec.data();

	unsigned char iv[AES_BLOCKLEN];
	for (int i = 0; i < AES_BLOCKLEN; ++i) {
		iv[i] = key_digest_vec[(i * 2) % key_digest_vec.size()] ^ static_cast<unsigned char>(4 * i + 1);
	}

	AES_ctx crypt_ctx;
	AES_init_ctx_iv(&crypt_ctx, aes_key, iv);
	AES_CBC_decrypt_buffer(&crypt_ctx, result_data.data(), result_data.size());

	std::fill(std::begin(iv), std::end(iv), 0);
	std::fill(reinterpret_cast<uint8_t*>(&crypt_ctx), reinterpret_cast<uint8_t*>(&crypt_ctx) + sizeof(AES_ctx), 0);

	vector_unpad_aes(result_data);
	return result_data;
}


void ScriptAssert(bool expr, const std::string& fail_text = "") {
	if (!expr) throw Poco::AssertionViolationException(fail_text);
}

class CBytecodeStream : public asIBinaryStream
{
public:
	std::vector<asBYTE> Code;
	int ReadPos, WritePos;

	CBytecodeStream() : ReadPos(0), WritePos(0) {}
	CBytecodeStream(const std::vector<asBYTE>& Data) : Code(Data), ReadPos(0), WritePos(0) {}

	int Read(void* Ptr, asUINT Size) override {
		if (Ptr == nullptr || Size == 0) return -1;
		if (static_cast<asUINT>(ReadPos) + Size > Code.size()) return -1;
		std::memcpy(Ptr, &Code[ReadPos], Size);
		ReadPos += Size;
		return Size; // AngelScript examples return bytes read or 0 for success, docs say "Return the number of bytes read, or a negative value on error."
	}

	int Write(const void* Ptr, asUINT Size) override {
		if (Ptr == nullptr || Size == 0) return -1;
		Code.insert(Code.end(), static_cast<const asBYTE*>(Ptr), static_cast<const asBYTE*>(Ptr) + Size);
		WritePos += Size; // WritePos is not strictly necessary for vector.insert but kept for consistency
		return Size; // AngelScript examples return bytes written or 0 for success, docs say "Return the number of bytes written, or a negative value on error."
	}
};


// Debugger string conversion callbacks
std::string StringToString(void* obj, int /* expandMembers */, CDebugger* /* dbg */) {
	std::string* val = reinterpret_cast<std::string*>(obj);
	std::stringstream s;
	s << "(len=" << val->length() << ") \"";
	if (val->length() < 20) s << *val;
	else s << val->substr(0, 17) << "...";
	s << "\"";
	return s.str();
}

std::string ArrayToString(void* obj, int expandMembers, CDebugger* dbg) {
	CScriptArray* arr = reinterpret_cast<CScriptArray*>(obj);
	std::stringstream s;
	s << "(len=" << arr->GetSize() << ")";
	if (expandMembers > 0 && arr->GetSize() > 0) {
		s << " [";
		for (asUINT n = 0; n < arr->GetSize(); n++) {
			s << dbg->ToString(arr->At(n), arr->GetElementTypeId(), expandMembers - 1, arr->GetArrayObjectType()->GetEngine());
			if (n < arr->GetSize() - 1) s << ", ";
			if (n >= 4 && arr->GetSize() > 5) { // Limit displayed elements for large arrays
				s << ", ...";
				break;
			}
		}
		s << "]";
	}
	return s.str();
}

std::string DictionaryToString(void* obj, int expandMembers, CDebugger* dbg) {
	CScriptDictionary* dic = reinterpret_cast<CScriptDictionary*>(obj);
	std::stringstream s;
	s << "(len=" << dic->GetSize() << ")";
	if (expandMembers > 0 && dic->GetSize() > 0) {
		s << " {";
		asUINT n = 0;
		for (CScriptDictionary::CIterator it = dic->begin(); it != dic->end(); it++, n++) {
			s << "\"" << it.GetKey() << "\": ";
			const void* val = it.GetAddressOfValue();
			int typeId = it.GetTypeId();
			asIScriptContext* ctx = asGetActiveContext();
			s << dbg->ToString(const_cast<void*>(val), typeId, expandMembers - 1, ctx->GetEngine());
			if (n < dic->GetSize() - 1) s << ", ";
			if (n >= 2 && dic->GetSize() > 3) { // Limit displayed elements
				s << ", ...";
				break;
			}
		}
		s << "}";
	}
	return s.str();
}

std::string DateTimeToString(void* obj, int /*expandMembers*/, CDebugger* /*dbg*/) {
	CDateTime* dt = reinterpret_cast<CDateTime*>(obj);
	std::stringstream s;
	char buffer[30];
	snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
		dt->getYear(), dt->getMonth(), dt->getDay(),
		dt->getHour(), dt->getMinute(), dt->getSecond());
	s << buffer;
	return s.str();
}


// Script execution command line arguments
CScriptArray* g_commandLineArgs = nullptr;
int           g_argc_script = 0;
char** g_argv_script = nullptr;

CScriptArray* GetCommandLineArgs() {
	if (g_commandLineArgs) {
		g_commandLineArgs->AddRef();
		return g_commandLineArgs;
	}
	asIScriptContext* ctx = asGetActiveContext();
	if (!ctx) return nullptr; // Should not happen if called from script
	asIScriptEngine* engine = ctx->GetEngine();

	asITypeInfo* arrayType = engine->GetTypeInfoByDecl("array<string>");
	if (!arrayType) return nullptr;

	g_commandLineArgs = CScriptArray::Create(arrayType, (asUINT)0);
	for (int n = 0; n < g_argc_script; ++n) {
		g_commandLineArgs->Resize(g_commandLineArgs->GetSize() + 1);
		((std::string*)g_commandLineArgs->At(n))->assign(g_argv_script[n]);
	}
	g_commandLineArgs->AddRef();
	return g_commandLineArgs;
}


// System command execution
int ExecSystemCmd(const std::string& cmd);
int ExecSystemCmd(const std::string& str, std::string& out);


void TranslateExceptionCallback(asIScriptContext* ctx, void* /*userParam*/) {
	try {
		throw; // Re-throw the C++ exception
	}
	catch (const Poco::Exception& e) {
		ctx->SetException(e.displayText().c_str());
	}
	catch (const std::exception& e) {
		ctx->SetException(e.what());
	}
	catch (...) {
		ctx->SetException("Unknown C++ exception");
	}
}


class NGTScripting;
NGTScripting* g_app_scripting_instance = nullptr; // For access from global callbacks like RequestContextCallback

asIScriptContext* RequestContextCallback(asIScriptEngine* engine, void* param);
void ReturnContextCallback(asIScriptEngine* engine, asIScriptContext* ctx, void* param);


class NGTScripting {
public:
	asIScriptEngine* scriptEngine = nullptr;
	CContextMgr* m_ctxMgr = nullptr;
	std::vector<asIScriptContext*> m_ctxPool;
	CDebugger* m_debugger = nullptr;
	bool SCRIPT_COMPILED_FLAG = false;

	NGTScripting() {
		g_app_scripting_instance = this;
		scriptEngine = asCreateScriptEngine();
		if (!scriptEngine) {
			std::cerr << "Failed to create AngelScript engine." << std::endl;
			throw std::runtime_error("Failed to create AngelScript engine.");
		}
		scriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), nullptr, asCALL_CDECL);
		scriptEngine->SetTranslateAppExceptionCallback(asFUNCTION(TranslateExceptionCallback), nullptr, asCALL_CDECL);
		scriptEngine->SetEngineProperty(asEP_ALLOW_UNSAFE_REFERENCES, true);
		scriptEngine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, false);
		scriptEngine->SetEngineProperty(asEP_ALLOW_IMPLICIT_HANDLE_TYPES, true);

		// Context manager for co-routines. I don't know, but it is crashing the application
		// m_ctxMgr = new CContextMgr();
		// m_ctxMgr->RegisterCoRoutineSupport(scriptEngine);

		scriptEngine->SetContextCallbacks(RequestContextCallback, ReturnContextCallback, this);
	}

	~NGTScripting() {
		if (m_debugger) {
			delete m_debugger;
			m_debugger = nullptr;
		}
		for (auto* ctx : m_ctxPool) {
			if (ctx) ctx->Release();
		}
		m_ctxPool.clear();

		if (m_ctxMgr) {
			delete m_ctxMgr;
			m_ctxMgr = nullptr;
		}

		if (scriptEngine) {
			if (g_commandLineArgs) {
				g_commandLineArgs->Release();
				g_commandLineArgs = nullptr;
			}
			scriptEngine->ShutDownAndRelease();
			scriptEngine = nullptr;
		}
		g_app_scripting_instance = nullptr;
	}

	void RegisterStandardAPI() {
		RegisterStdString(scriptEngine);
		RegisterStdWstring(scriptEngine);
		RegisterScriptArray(scriptEngine, true);
		RegisterStdStringUtils(scriptEngine);
		RegisterScriptDictionary(scriptEngine);
		RegisterScriptDateTime(scriptEngine);
		RegisterScriptFile(scriptEngine);
		RegisterExceptionRoutines(scriptEngine);
		RegisterScriptMath(scriptEngine);
		RegisterScriptMathComplex(scriptEngine);
		RegisterScriptHandle(scriptEngine);
		RegisterScriptAny(scriptEngine);

		RegisterFunctions(scriptEngine);

		scriptEngine->RegisterGlobalFunction("array<string> @get_char_argv()", asFUNCTION(GetCommandLineArgs), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("int exec(const string &in)", asFUNCTIONPR(ExecSystemCmd, (const std::string&), int), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("int exec(const string &in, string &out)", asFUNCTIONPR(ExecSystemCmd, (const std::string&, std::string&), int), asCALL_CDECL);
		scriptEngine->RegisterGlobalProperty("const bool SCRIPT_COMPILED", (void*)&SCRIPT_COMPILED_FLAG);
		scriptEngine->RegisterGlobalFunction("string get_SCRIPT_EXECUTABLE() property", asFUNCTION(get_exe_helper), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("string get_SCRIPT_EXECUTABLE_PATH() property", asFUNCTION(get_exe_path_helper), asCALL_CDECL);
		scriptEngine->RegisterGlobalFunction("void assert(bool expr, const string&in fail_text = \"\")", asFUNCTION(ScriptAssert), asCALL_CDECL);
	}

	// Compiles script from file, returns module and bytecode
	asIScriptModule* CompileScriptToBytecode(const char* scriptFile, std::vector<asBYTE>& outBytecode, const std::string& moduleName) {
		CScriptBuilder builder;
		builder.SetPragmaCallback(PragmaCallback, nullptr); // Pragma callback uses the builder passed to it
		builder.SetIncludeCallback(IncludeCallback, nullptr);

		int r = builder.StartNewModule(scriptEngine, moduleName.c_str());
		if (r < 0) {
			show_message();
			return nullptr;
		}
		r = builder.AddSectionFromFile(scriptFile);
		if (r < 0) {
			show_message();
			return nullptr;
		}
		r = builder.BuildModule();
		if (r < 0) {
			show_message();
			return nullptr;
		}

		asIScriptModule* module = scriptEngine->GetModule(moduleName.c_str());
		CBytecodeStream stream;
		if (module && module->SaveByteCode(&stream, true) >= 0) {
			outBytecode = stream.Code;
			return module;
		}
		scriptEngine->WriteMessage(scriptFile, 0, 0, asMSGTYPE_ERROR, "Failed to save bytecode from compiled module.");
		show_message();
		return nullptr;
	}

	// Loads bytecode into a new module
	asIScriptModule* LoadBytecodeIntoModule(const std::vector<asBYTE>& bytecode, const std::string& moduleName) {
		asIScriptModule* module = scriptEngine->GetModule(moduleName.c_str(), asGM_ALWAYS_CREATE);
		if (!module) {
			scriptEngine->WriteMessage("BytecodeLoad", 0, 0, asMSGTYPE_ERROR, "Failed to create module for bytecode loading.");
			show_message();
			return nullptr;
		}

		CBytecodeStream stream(bytecode);
		if (module->LoadByteCode(&stream) < 0) {
			scriptEngine->WriteMessage("BytecodeLoad", 0, 0, asMSGTYPE_ERROR, "Failed to load bytecode into module.");
			show_message();
			scriptEngine->DiscardModule(moduleName.c_str());
			return nullptr;
		}
		return module;
	}

	int ExecuteMain(asIScriptModule* module) {
		if (!module) return -1;

		asIScriptFunction* func = module->GetFunctionByName("main");
		if (!func) {
			scriptEngine->WriteMessage(module->GetName(), 0, 0, asMSGTYPE_ERROR, "Entry point 'main()' not found.");
			show_message();
			return -1;
		}

		// It's good practice to re-initialize global vars if module might be reused
		if (module->ResetGlobalVars(nullptr) < 0) { // Pass nullptr for default context
			scriptEngine->WriteMessage(module->GetName(), 0, 0, asMSGTYPE_ERROR, "Failed to reset global variables.");
			show_message();
			return -1;
		}

		asIScriptContext* ctx = scriptEngine->RequestContext();
		if (!ctx) {
			scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to request script context.");
			show_message();
			return -1;
		}
		ctx->Prepare(func);

		if (m_debugger) {
			std::cout << "Debugger active. Type 'c' to continue, 'h' for help." << std::endl;
			m_debugger->TakeCommands(ctx);
		}

		int r = ctx->Execute();
		int retVal = 0;

		if (r == asEXECUTION_FINISHED) {
			if (func->GetReturnTypeId() == asTYPEID_VOID) {
				retVal = 0;
			}
			else if (func->GetReturnTypeId() == asTYPEID_INT32) {
				retVal = ctx->GetReturnDWord();
			}
			else {
				retVal = 0;
			}
		}
		else if (r == asEXECUTION_EXCEPTION) {
			scriptEngine->WriteMessage(ctx->GetFunction()->GetModuleName(), 0, 0, asMSGTYPE_ERROR, GetExceptionInfo(ctx, true).c_str());
			show_message();
			retVal = -1;
		}
		else if (r == asEXECUTION_ABORTED) {
			retVal = g_retcode;
		}
		else {
			scriptEngine->WriteMessage(ctx->GetFunction()->GetModuleName(), 0, 0, asMSGTYPE_ERROR, std::string("Script execution failed with code: " + std::to_string(r)).c_str());
			show_message();
			retVal = -1; // Other error
		}

		scriptEngine->ReturnContext(ctx);
		return retVal;
	}

	void InitializeDebugger() {
		if (!m_debugger) {
			m_debugger = new CDebugger();
			m_debugger->SetEngine(scriptEngine);
			// Register to-string callbacks
			asITypeInfo* stringType = scriptEngine->GetTypeInfoByName("string");
			if (stringType) m_debugger->RegisterToStringCallback(stringType, StringToString);

			asITypeInfo* arrayType = scriptEngine->GetTypeInfoByDecl("array<T>"); // Get template
			if (arrayType) m_debugger->RegisterToStringCallback(arrayType, ArrayToString);

			asITypeInfo* dictType = scriptEngine->GetTypeInfoByName("dictionary");
			if (dictType) m_debugger->RegisterToStringCallback(dictType, DictionaryToString);

			asITypeInfo* dtType = scriptEngine->GetTypeInfoByName("datetime");
			if (dtType) m_debugger->RegisterToStringCallback(dtType, DateTimeToString);
		}
	}

	void DeinitializeDebugger() {
		if (m_debugger) {
			delete m_debugger;
			m_debugger = nullptr;
			// Contexts need their line callback cleared if debugger is removed mid-session
		}
	}

	int WritePredefinedAPIFile(const std::string& outputPath) {
		std::ofstream f(outputPath);
		if (!f.is_open()) {
			std::cerr << "Failed to open " << outputPath << " for writing." << std::endl;
			return -1;
		}
		f << "// AngelScript Predefined API Dump for NGT\n\n";
		f << "// Enums\n";
		for (asUINT i = 0; i < scriptEngine->GetEnumCount(); ++i) {
			asITypeInfo* enumType = scriptEngine->GetEnumByIndex(i);
			if (enumType) f << "enum " << enumType->GetName() << " { /* ... values ... */ };\n";
		}
		f << "\n// Object Types (Classes, Interfaces)\n";
		for (asUINT i = 0; i < scriptEngine->GetObjectTypeCount(); ++i) {
			asITypeInfo* objType = scriptEngine->GetObjectTypeByIndex(i);
			if (objType) f << "class " << objType->GetName() << " { /* ... members ... */ };\n";
		}
		f << "\n// Global Functions\n";
		for (asUINT i = 0; i < scriptEngine->GetGlobalFunctionCount(); ++i) {
			asIScriptFunction* func = scriptEngine->GetGlobalFunctionByIndex(i);
			if (func) f << func->GetDeclaration(true, true, true) << ";\n";
		}
		f << "\n// Global Properties\n";
		for (asUINT i = 0; i < scriptEngine->GetGlobalPropertyCount(); ++i) {
			const char* name, * nameSpace;
			int typeId;
			bool isConst;
			scriptEngine->GetGlobalPropertyByIndex(i, &name, &nameSpace, &typeId, &isConst);
			f << (isConst ? "const " : "") << scriptEngine->GetTypeDeclaration(typeId, true) << " " << name << ";\n";
		}
		f.close();
		scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_INFORMATION, std::string("API definition written to " + outputPath).c_str());
		show_message(true);
		return 0;
	}
};

// Context Callbacks
asIScriptContext* RequestContextCallback(asIScriptEngine* /*engine*/, void* param) {
	NGTScripting* scripting = static_cast<NGTScripting*>(param);
	asIScriptContext* ctx = nullptr;
	if (!scripting->m_ctxPool.empty()) {
		ctx = scripting->m_ctxPool.back();
		scripting->m_ctxPool.pop_back();
	}
	else {
		ctx = scripting->scriptEngine->CreateContext();
		if (!ctx) {
			scripting->scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to create new script context in callback.");
			show_message();
			return nullptr;
		}
	}
	if (scripting->m_debugger && ctx) {
		ctx->SetLineCallback(asMETHOD(CDebugger, LineCallback), scripting->m_debugger, asCALL_THISCALL);
	}
	return ctx;
}

void ReturnContextCallback(asIScriptEngine* /*engine*/, asIScriptContext* ctx, void* param) {
	NGTScripting* scripting = static_cast<NGTScripting*>(param);
	ctx->Unprepare(); // Important!
	if (scripting->m_debugger) {
		ctx->ClearLineCallback();
	}
	scripting->m_ctxPool.push_back(ctx);
}


// Main Application Class
class NGTEntry : public Application {
private:
	NGTScripting m_scripting;
	std::string m_scriptFileToProcess;
	std::string m_outputFile; // For compilation target

	enum class OperationMode {
		NONE, HELP, DEBUG_SCRIPT, RUN_SCRIPT,
		COMPILE_SCRIPT, DUMP_PREDEFINED, EXECUTE_EMBEDDED
	};
	OperationMode m_opMode = OperationMode::NONE;
	bool m_helpRequested = false;


public:
	int m_lastReturnCode = 0;

	NGTEntry() {
		setUnixOptions(true);
	}

protected:
	void initialize(Application& self) override {
#ifdef _WIN32
		timeBeginPeriod(1);
#endif
		Application::initialize(self);

		m_scripting.RegisterStandardAPI();


		// Check if this executable has embedded bytecode
		std::vector<asBYTE> bytecode;
		if (loadBytecodeFromExecutableInternal(bytecode) && !bytecode.empty()) {
			m_opMode = OperationMode::EXECUTE_EMBEDDED;
			m_scripting.SCRIPT_COMPILED_FLAG = true;
			stopOptionsProcessing(); // Don't process command line options if bytecode is embedded
		}
	}

	void uninitialize() override {
#ifdef _WIN32
		timeEndPeriod(1);
#endif
		soundsystem_free();
		Application::uninitialize();
	}

	void defineOptions(OptionSet& options) override {
		Application::defineOptions(options);
		options.addOption(
			Option("help", "h", "Display help information")
			.required(false).repeatable(false).callback(OptionCallback<NGTEntry>(this, &NGTEntry::handleHelp)));
		options.addOption(
			Option("debug", "d", "Debug a script file.")
			.required(false).argument("script.as").repeatable(false).callback(OptionCallback<NGTEntry>(this, &NGTEntry::handleDebug)));
		options.addOption(
			Option("run", "r", "Run a script file.")
			.required(false).argument("script.as").repeatable(false).callback(OptionCallback<NGTEntry>(this, &NGTEntry::handleRun)));
		options.addOption(
			Option("compile", "c", "Compile a script to an executable.")
			.required(false).argument("script.as", true).repeatable(false).callback(OptionCallback<NGTEntry>(this, &NGTEntry::handleCompile)));
		options.addOption(
			Option("dumpapi", "p", "Generate as.predefined API file for IDEs.")
			.required(false).repeatable(false).callback(OptionCallback<NGTEntry>(this, &NGTEntry::handleDumpApi)));
	}

	// Option Handlers
	void handleHelp(const std::string& name, const std::string& value) {
		m_helpRequested = true;
		m_opMode = OperationMode::HELP;
		stopOptionsProcessing();
	}
	void handleDebug(const std::string& name, const std::string& value) {
		m_opMode = OperationMode::DEBUG_SCRIPT;
		m_scriptFileToProcess = value;
		stopOptionsProcessing();
	}
	void handleRun(const std::string& name, const std::string& value) {
		m_opMode = OperationMode::RUN_SCRIPT;
		m_scriptFileToProcess = value;
		stopOptionsProcessing();
	}
	void handleCompile(const std::string& name, const std::string& value) {
		m_opMode = OperationMode::COMPILE_SCRIPT;
		m_scriptFileToProcess = value;
		m_outputFile = std::filesystem::absolute(value).filename().string();
		size_t pos = m_outputFile.find(".");
		if (pos != std::string::npos) {
			m_outputFile.erase(pos);
		}
#ifdef _WIN32
		m_outputFile.append(".exe");
#endif
		stopOptionsProcessing();
	}
	void handleDumpApi(const std::string& name, const std::string& value) {
		m_opMode = OperationMode::DUMP_PREDEFINED;
		stopOptionsProcessing();
	}


	int main(const ArgVec& args) override {
		g_argc_script = args.size() - 1;
		std::vector<char*> c_style_args;
		for (size_t i = 1; i < argv().size(); ++i) { // Skip argv()[0]
			c_style_args.push_back(const_cast<char*>(argv()[i].c_str()));
		}
		g_argv_script = c_style_args.data(); // Pointer to data in vector, ensure vector outlives usage

		switch (m_opMode) {
		case OperationMode::HELP:
			displayHelp();
			m_lastReturnCode = Application::EXIT_OK;
			break;
		case OperationMode::EXECUTE_EMBEDDED:
			m_lastReturnCode = doExecuteEmbeddedBytecode();
			break;
		case OperationMode::RUN_SCRIPT:
			m_lastReturnCode = doRunScript();
			break;
		case OperationMode::DEBUG_SCRIPT:
			m_lastReturnCode = doDebugScript();
			break;
		case OperationMode::COMPILE_SCRIPT:
			m_lastReturnCode = doCompileScript();
			break;
		case OperationMode::DUMP_PREDEFINED:
			m_lastReturnCode = doDumpApi();
			break;
		case OperationMode::NONE:
			if (!m_helpRequested) {
				displayHelp();
				m_lastReturnCode = Application::EXIT_USAGE;
			}
			else {
				m_lastReturnCode = Application::EXIT_OK;
			}
			break;
		}
		return m_lastReturnCode;
	}

	void displayHelp() {
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("NGT (New Game Toolkit) - AngelScript Runtime and Compiler");
		std::stringstream ss;
		helpFormatter.format(ss);
		m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_INFORMATION, ss.str().c_str());
		show_message(true);
	}

	// Action methods
	int doExecuteEmbeddedBytecode() {
		std::vector<asBYTE> bytecode;
		if (!loadBytecodeFromExecutableInternal(bytecode) || bytecode.empty()) {
			m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to load embedded bytecode or bytecode is empty.");
			show_message();
			return -1;
		}

		// Decrypt and de-obfuscate
		std::string key_material = string_base64_encode(NGT_BYTECODE_ENCRYPTION_KEY);
		std::vector<asBYTE> decrypted_bytecode = aes_decrypt_vector(bytecode, key_material);
		if (decrypted_bytecode.empty() && !bytecode.empty()) { // Decryption failed if result is empty but input was not
			m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "AES decryption of embedded bytecode failed.");
			show_message();
			return -1;
		}
		apply_simple_obfuscation(decrypted_bytecode); // Reverse simple obfuscation

		std::string module_name = Poco::Path(get_exe_helper()).getBaseName();
		asIScriptModule* module = m_scripting.LoadBytecodeIntoModule(decrypted_bytecode, module_name);
		if (!module) return -1;

		int result = m_scripting.ExecuteMain(module);
		m_scripting.scriptEngine->DiscardModule(module_name.c_str());
		return result;
	}

	int doRunOrDebugScript(bool debug) {
		if (debug) {
#ifdef _WIN32
			if (GetConsoleWindow() == nullptr) { // Check if running in a console
				alert("NGT Error", "Debugger must be run from a command console.");
				return -2;
			}
#endif
			m_scripting.InitializeDebugger();
			std::cout << "NGT Debugger Initialized. Script: " << m_scriptFileToProcess << std::endl;
		}

		std::vector<asBYTE> bytecode;
		std::string module_name = Poco::Path(m_scriptFileToProcess).getBaseName();
		asIScriptModule* module = m_scripting.CompileScriptToBytecode(m_scriptFileToProcess.c_str(), bytecode, module_name);

		if (!module) return -1;

		int result = m_scripting.ExecuteMain(module);

		m_scripting.scriptEngine->DiscardModule(module_name.c_str());
		if (debug) m_scripting.DeinitializeDebugger();
		return result;
	}

	int doRunScript() { return doRunOrDebugScript(false); }
	int doDebugScript() { return doRunOrDebugScript(true); }

	int doCompileScript() {
		std::vector<asBYTE> bytecode_raw;
		std::string module_name = Poco::Path(m_scriptFileToProcess).getBaseName();
		asIScriptModule* module = m_scripting.CompileScriptToBytecode(m_scriptFileToProcess.c_str(), bytecode_raw, module_name);

		if (!module) {
			if (bytecode_raw.empty()) {
				m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "Compilation failed or produced no bytecode.");
				show_message();
				return -1;
			}
		}
		if (bytecode_raw.empty()) {
			m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "Compilation produced no bytecode despite module success.");
			show_message();
			if (module) m_scripting.scriptEngine->DiscardModule(module_name.c_str());
			return -1;
		}


		apply_simple_obfuscation(bytecode_raw);
		std::string key_material = string_base64_encode(NGT_BYTECODE_ENCRYPTION_KEY);
		std::vector<asBYTE> bytecode_processed = aes_encrypt_vector(bytecode_raw, key_material);

		if (bytecode_processed.empty() && !bytecode_raw.empty()) {
			m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, "AES encryption failed.");
			show_message();
			if (module) m_scripting.scriptEngine->DiscardModule(module_name.c_str());
			return -1;
		}


		if (!saveBytecodeToExecutableInternal(m_outputFile, bytecode_processed)) {
			m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_ERROR, std::string("Failed to save bytecode to executable: " + m_outputFile).c_str());
			show_message();
			if (module) m_scripting.scriptEngine->DiscardModule(module_name.c_str());
			return -1;
		}

		m_scripting.scriptEngine->WriteMessage(get_exe_helper().c_str(), 0, 0, asMSGTYPE_INFORMATION, std::string("Script '" + m_scriptFileToProcess + "' compiled to '" + m_outputFile + "' successfully.").c_str());
		show_message(true);

		if (module) m_scripting.scriptEngine->DiscardModule(module_name.c_str()); // Discard module after getting bytecode
		return 0;
	}

	int doDumpApi() {
		return m_scripting.WritePredefinedAPIFile("as.predefined");
	}

private:
	bool loadBytecodeFromExecutableInternal(std::vector<asBYTE>& outBytecode) {
		outBytecode.clear();
		std::string exePath = get_exe_helper();

#ifdef _WIN32
		HMODULE hModule = GetModuleHandle(nullptr);
		if (!hModule) return false;
		HRSRC hRes = FindResource(hModule, NGT_BYTECODE_RESOURCE_ID_W, NGT_BYTECODE_RESOURCE_TYPE_W);
		if (!hRes) return false;
		HGLOBAL hResLoad = LoadResource(hModule, hRes);
		if (!hResLoad) return false;
		LPVOID lpResLock = LockResource(hResLoad);
		if (!lpResLock) {
			FreeResource(hResLoad); return false;
		}
		DWORD dwSize = SizeofResource(hModule, hRes);
		if (dwSize == 0) { UnlockResource(hResLoad); FreeResource(hResLoad); return false; } //Don't call FreeResource on success with LockResource

		outBytecode.assign(static_cast<asBYTE*>(lpResLock), static_cast<asBYTE*>(lpResLock) + dwSize);
		return true;
#else
		std::ifstream file(exePath, std::ios::binary | std::ios::ate);
		if (!file.is_open()) return false;
		std::streamsize fileSize = file.tellg();
		if (fileSize < static_cast<std::streamsize>(NGT_BYTECODE_FILE_SIGNATURE_LEN + sizeof(asUINT))) return false;

		std::vector<char> signature_check(NGT_BYTECODE_FILE_SIGNATURE_LEN);
		file.seekg(fileSize - static_cast<std::streamsize>(NGT_BYTECODE_FILE_SIGNATURE_LEN));
		file.read(signature_check.data(), NGT_BYTECODE_FILE_SIGNATURE_LEN);
		if (std::string(signature_check.data(), NGT_BYTECODE_FILE_SIGNATURE_LEN) != NGT_BYTECODE_FILE_SIGNATURE) {
			return false;
		}

		// Read bytecode size
		asUINT bcSize;
		file.seekg(fileSize - static_cast<std::streamsize>(NGT_BYTECODE_FILE_SIGNATURE_LEN + sizeof(asUINT)));
		file.read(reinterpret_cast<char*>(&bcSize), sizeof(asUINT));
		if (bcSize == 0 || file.gcount() != sizeof(asUINT)) return false;

		std::streamsize expectedMinSize = static_cast<std::streamsize>(NGT_BYTECODE_FILE_SIGNATURE_LEN + sizeof(asUINT) + bcSize);
		if (fileSize < expectedMinSize) return false;

		outBytecode.resize(bcSize);
		file.seekg(fileSize - static_cast<std::streamsize>(NGT_BYTECODE_FILE_SIGNATURE_LEN + sizeof(asUINT) + bcSize));
		file.read(reinterpret_cast<char*>(outBytecode.data()), bcSize);
		if (file.gcount() != static_cast<std::streamsize>(bcSize)) {
			outBytecode.clear();
			return false;
		}
		return true;
#endif
		return true;
	}

	bool saveBytecodeToExecutableInternal(const std::string& targetPath, const std::vector<asBYTE>& bytecode) {
		std::string hostExePath = get_exe_helper();
		try {
			std::filesystem::copy_file(hostExePath, targetPath, std::filesystem::copy_options::overwrite_existing);
		}
		catch (const std::filesystem::filesystem_error& e) {
			m_scripting.scriptEngine->WriteMessage(hostExePath.c_str(), 0, 0, asMSGTYPE_ERROR, std::string("Error copying host executable: " + std::string(e.what())).c_str());
			show_message();
			return false;
		}

#ifdef _WIN32
		std::wstring targetPathW;
		Poco::UnicodeConverter::convert(targetPath, targetPathW);
		HANDLE hUpdate = BeginUpdateResourceW(targetPathW.c_str(), FALSE);
		if (hUpdate == nullptr) {
			m_scripting.scriptEngine->WriteMessage(hostExePath.c_str(), 0, 0, asMSGTYPE_ERROR, std::string("BeginUpdateResource failed: " + std::to_string(GetLastError())).c_str());
			show_message();
			return false;
		}
		BOOL success = UpdateResourceW(hUpdate, NGT_BYTECODE_RESOURCE_TYPE_W, NGT_BYTECODE_RESOURCE_ID_W,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
			(LPVOID)bytecode.data(), (DWORD)bytecode.size());
		if (!success) {
			m_scripting.scriptEngine->WriteMessage(hostExePath.c_str(), 0, 0, asMSGTYPE_ERROR, std::string("UpdateResource failed: " + std::to_string(GetLastError())).c_str());
			show_message();
			EndUpdateResource(hUpdate, TRUE);
			return false;
		}
		if (!EndUpdateResource(hUpdate, FALSE)) {
			m_scripting.scriptEngine->WriteMessage(hostExePath.c_str(), 0, 0, asMSGTYPE_ERROR, std::string("EndUpdateResource failed: " + std::to_string(GetLastError())).c_str());
			show_message();
			return false;
		}
		return true;
#else
		std::ofstream file(targetPath, std::ios::binary | std::ios::app | std::ios::ate);
		if (!file.is_open()) {
			m_scripting.scriptEngine->WriteMessage(targetPath.c_str(), 0, 0, asMSGTYPE_ERROR, "Failed to open target executable for appending");
			show_message();
			return false;
		}
		file.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
		asUINT bcSize = static_cast<asUINT>(bytecode.size());
		file.write(reinterpret_cast<const char*>(&bcSize), sizeof(asUINT));
		file.write(NGT_BYTECODE_FILE_SIGNATURE, NGT_BYTECODE_FILE_SIGNATURE_LEN);
		file.close();
		return !file.fail();
#endif
	}

};

#undef SDL_MAIN_HANDLED
#undef SDL_main_h_
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[]) {
#ifdef _WIN32
	SetUnhandledExceptionFilter(ExceptionHandler);
#else
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = signalHandler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, nullptr);
	sigaction(SIGFPE, &sa, nullptr);
	sigaction(SIGILL, &sa, nullptr);
#endif
	AutoPtr<NGTEntry> pApp = new NGTEntry();
	try {
		pApp->init(argc, argv);
	}
	catch (Poco::Exception& exc) {
		pApp->logger().log(exc);
		return Application::EXIT_CONFIG;
	}
	return pApp->run();
}


int ExecSystemCmd(const std::string& cmd) {
#ifdef _WIN32
	// Convert the command to UTF16 to properly handle unicode path names
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, cmd.c_str(), -1, bufUTF16, 10000);
	return _wsystem(bufUTF16);
#else
	return system(cmd.c_str());
#endif
}

int ExecSystemCmd(const std::string& cmd, std::string& out) {
	out = "";
#ifdef _WIN32
	wchar_t bufUTF16[10000]; // Potential buffer overflow if cmd is too long
	MultiByteToWideChar(CP_UTF8, 0, cmd.c_str(), -1, bufUTF16, 10000);

	HANDLE pipeRead, pipeWrite;
	SECURITY_ATTRIBUTES secAttr = { 0 };
	secAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	secAttr.bInheritHandle = TRUE;
	secAttr.lpSecurityDescriptor = NULL;
	if (!CreatePipe(&pipeRead, &pipeWrite, &secAttr, 0)) return -1;

	STARTUPINFOW si = { 0 };
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.hStdOutput = pipeWrite;
	si.hStdError = pipeWrite; // Capture stderr too
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi = { 0 };
	BOOL success = CreateProcessW(NULL, bufUTF16, NULL, NULL, TRUE, 0 /*CREATE_NEW_CONSOLE removed, can cause issues*/, NULL, NULL, &si, &pi);
	if (!success) {
		CloseHandle(pipeWrite);
		CloseHandle(pipeRead);
		return -1;
	}
	CloseHandle(pipeWrite); // Close our copy of write end so ReadFile can detect EOF

	char buffer[4096];
	DWORD bytesRead;
	while (ReadFile(pipeRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
		buffer[bytesRead] = '\0';
		out += buffer;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD status = 0;
	GetExitCodeProcess(pi.hProcess, &status);

	CloseHandle(pipeRead);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return status;
#else
	// Linux/macOS variant using popen
	FILE* pipe = popen((cmd + " 2>&1").c_str(), "r"); // "2>&1" to redirect stderr to stdout
	if (!pipe) return -1;
	char buffer[128];
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		out += buffer;
	}
	return pclose(pipe); // Returns termination status of command
#endif
}

