#include "ngtreg.h"
#include "scripthelper/scripthelper.h"
#include "Scripting.h"
#include<string>
#undef GetObject
int RegisterFunction(const std::string& signature, void* function_handle, int callconv) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->RegisterGlobalFunction(signature.c_str(), asFUNCTION(function_handle), callconv);
}

std::string GetMessages(bool include_error = true, bool include_warning = true, bool include_info = false) {
	std::string messages;

	if (include_error) {
		messages += g_ScriptMessagesError; // Assuming g_ScriptMessagesError is defined elsewhere
	}
	if (include_warning) {
		messages += g_ScriptMessagesWarning; // Assuming g_ScriptMessagesWarning is defined elsewhere
	}
	if (include_info) {
		messages += g_ScriptMessagesInfo; // Assuming g_ScriptMessagesInfo is defined elsewhere
	}
	return messages;
}
void ClearMessages() {
	g_ScriptMessagesError = "";
	g_ScriptMessagesWarning = "";
	g_ScriptMessagesInfo = "";
}
void GetArgAddress(asIScriptGeneric* gen) {
	gen->SetReturnQWord((asINT64)gen->GetArgAddress(0));
}

void GetAddressOfArg(asIScriptGeneric* gen) {
	gen->SetReturnQWord((asINT64)gen->GetAddressOfArg(0));
}

void GetArgTypeId(asIScriptGeneric* gen) {
	gen->SetReturnDWord(gen->GetArgTypeId(0));
}


asIScriptContext* CtxFactory() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->RequestContext();
}
asIScriptFunction* FunctionFactory(int id) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->GetFunctionById(id);
}
asIScriptFunction* GetFunctionById(int id) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->GetFunctionById(id);
}
asIScriptFunction* GetFunctionByDecl(const std::string& sig) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->GetGlobalFunctionByDecl(sig.c_str());
}


class ScriptModuleBytecodeStream : public asIBinaryStream {
	std::string data;
	int cursor;
public:
	ScriptModuleBytecodeStream(const std::string& code = "") : data(code), cursor(0) {}
	void set(const std::string& code) {
		data = code;
		cursor = 0;
	}
	std::string get() {
		return data;
	}
	int Write(const void* ptr, asUINT size) {
		data.append((char*)ptr, size);
		return size;
	}
	int Read(void* ptr, asUINT size) {
		if (cursor >= data.size()) return -1;
		memcpy(ptr, &data[cursor], size);
		cursor += size;
		return size;
	}
};

asIScriptModule* ModuleFactory(const std::string& section_name) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asIScriptModule* module = engine->GetModule(section_name.c_str(), asGM_ALWAYS_CREATE);
	return module;
}

void ModuleAddScriptSection(asIScriptGeneric* gen) {
	asIScriptModule* module = (asIScriptModule*)gen->GetObject();
	std::string name = *reinterpret_cast<std::string*>(gen->GetArgAddress(0));
	std::string code = *reinterpret_cast<std::string*>(gen->GetArgAddress(1));
	gen->SetReturnDWord(module->AddScriptSection(name.c_str(), code.c_str()));
}

void ModuleGetByteCode(asIScriptGeneric* gen) {
	asIScriptModule* module = (asIScriptModule*)gen->GetObject();
	bool strip_debug = gen->GetArgDWord(0);
	ScriptModuleBytecodeStream stream;
	module->SaveByteCode(&stream, strip_debug);
	std::string code = stream.get();
	gen->SetReturnObject(&code);
}

void ModuleSetByteCode(asIScriptGeneric* gen) {
	asIScriptModule* module = (asIScriptModule*)gen->GetObject();
	std::string code = *reinterpret_cast<std::string*>(gen->GetArgAddress(0));
	ScriptModuleBytecodeStream stream;
	stream.set(code);
	gen->SetReturnDWord(module->LoadByteCode(&stream));
}

void ModuleGetFunctionByIndex(asIScriptGeneric* gen) {
	asIScriptModule* module = (asIScriptModule*)gen->GetObject();
	int index = gen->GetArgDWord(0);
	asIScriptFunction* function = module->GetFunctionByIndex(index);
	gen->SetReturnObject(function);
}


void ContextGetExceptionInfo(asIScriptGeneric* gen) {
	asIScriptContext* ctx = (asIScriptContext*)gen->GetObject();
	bool call_stac = gen->GetArgDWord(0);
	std::string exception = GetExceptionInfo(ctx, call_stac);
	gen->SetReturnObject(&exception);
}

void RegisterScripting(asIScriptEngine* engine) {
	engine->RegisterEnum("typeid");
	engine->RegisterEnumValue("typeid", "VOID", asTYPEID_VOID);
	engine->RegisterEnumValue("typeid", "BOOL", asTYPEID_BOOL);
	engine->RegisterEnumValue("typeid", "INT8", asTYPEID_INT8);
	engine->RegisterEnumValue("typeid", "INT16", asTYPEID_INT16);
	engine->RegisterEnumValue("typeid", "INT32", asTYPEID_INT32);
	engine->RegisterEnumValue("typeid", "INT64", asTYPEID_INT64);

	engine->RegisterEnumValue("typeid", "UINT8", asTYPEID_UINT8);
	engine->RegisterEnumValue("typeid", "UINT16", asTYPEID_UINT16);
	engine->RegisterEnumValue("typeid", "UINT32", asTYPEID_UINT32);
	engine->RegisterEnumValue("typeid", "UINT64", asTYPEID_UINT64);

	engine->RegisterEnumValue("typeid", "FLOAT", asTYPEID_FLOAT);
	engine->RegisterEnumValue("typeid", "DOUBLE", asTYPEID_DOUBLE);
	engine->RegisterEnumValue("typeid", "OBJHANDLE", asTYPEID_OBJHANDLE);
	engine->RegisterEnumValue("typeid", "HANDLETOCONST", asTYPEID_HANDLETOCONST);
	engine->RegisterEnumValue("typeid", "MASK_OBJECT", asTYPEID_MASK_OBJECT);
	engine->RegisterEnumValue("typeid", "APPOBJECT", asTYPEID_APPOBJECT);
	engine->RegisterEnumValue("typeid", "SCRIPTOBJECT", asTYPEID_SCRIPTOBJECT);
	engine->RegisterEnumValue("typeid", "TEMPLATE", asTYPEID_TEMPLATE);
	engine->RegisterEnumValue("typeid", "MASK_SEQNBR", asTYPEID_MASK_SEQNBR);


	engine->RegisterEnum("callconv");
	engine->RegisterEnumValue("callconv", "CDECL", asCALL_CDECL);
	engine->RegisterEnumValue("callconv", "STDCALL", asCALL_STDCALL);

	engine->RegisterGlobalFunction("int register_function(const string &in, uint64, callconv)", asFUNCTION(RegisterFunction), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_messages(bool=true, bool=true, bool=false)", asFUNCTION(GetMessages), asCALL_CDECL);
	engine->RegisterGlobalFunction("void clear_messages()", asFUNCTION(ClearMessages), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_arg_address(?&in)", asFUNCTION(GetArgAddress), asCALL_GENERIC);
	engine->RegisterGlobalFunction("uint64 get_address_of_arg(?&in)", asFUNCTION(GetAddressOfArg), asCALL_GENERIC);
	engine->RegisterGlobalFunction("int get_arg_type_id(?&in)", asFUNCTION(GetArgTypeId), asCALL_GENERIC);


	engine->RegisterObjectType("function", sizeof(asIScriptFunction), asOBJ_REF);
	engine->RegisterObjectBehaviour("function", asBEHAVE_FACTORY, "function@ f(int=0)", asFUNCTION(FunctionFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("function", asBEHAVE_ADDREF, "void f()", asMETHOD(asIScriptFunction, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("function", asBEHAVE_RELEASE, "void f()", asMETHOD(asIScriptFunction, Release), asCALL_THISCALL);

	engine->RegisterGlobalFunction("function@ get_function_by_id(int)", asFUNCTION(GetFunctionById), asCALL_CDECL);
	engine->RegisterGlobalFunction("function@ get_function_by_decl(const string &in)", asFUNCTION(GetFunctionByDecl), asCALL_CDECL);

	engine->RegisterEnum("ctxstate");
	engine->RegisterEnumValue("ctxstate", "EXECUTION_FINISHED", asEXECUTION_FINISHED);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_SUSPENDED", asEXECUTION_SUSPENDED);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_ABORTED", asEXECUTION_ABORTED);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_EXCEPTION", asEXECUTION_EXCEPTION);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_PREPARED", asEXECUTION_PREPARED);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_UNINITIALIZED", asEXECUTION_UNINITIALIZED);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_ACTIVE", asEXECUTION_ACTIVE);
	engine->RegisterEnumValue("ctxstate", "EXECUTION_ERROR", asEXECUTION_ERROR);


	engine->RegisterObjectType("context", sizeof(asIScriptContext), asOBJ_REF);
	engine->RegisterObjectBehaviour("context", asBEHAVE_FACTORY, "context@ c()", asFUNCTION(CtxFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("context", asBEHAVE_ADDREF, "void f()", asMETHOD(asIScriptContext, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("context", asBEHAVE_RELEASE, "void f()", asMETHOD(asIScriptContext, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int prepare(function@)const", asMETHOD(asIScriptContext, Prepare), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int unprepare()const", asMETHOD(asIScriptContext, Unprepare), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int execute()const", asMETHOD(asIScriptContext, Execute), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int abort()const", asMETHOD(asIScriptContext, Abort), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int suspend()const", asMETHOD(asIScriptContext, Suspend), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "ctxstate get_state()const", asMETHOD(asIScriptContext, GetState), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int push_state()const", asMETHOD(asIScriptContext, PushState), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int pop_state()const", asMETHOD(asIScriptContext, PopState), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_byte(uint, char)const", asMETHOD(asIScriptContext, SetArgByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_word(uint, short)const", asMETHOD(asIScriptContext, SetArgWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_dword(uint, int32)const", asMETHOD(asIScriptContext, SetArgDWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_qword(uint, int64)const", asMETHOD(asIScriptContext, SetArgQWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_float(uint, float)const", asMETHOD(asIScriptContext, SetArgFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_double(uint, double)const", asMETHOD(asIScriptContext, SetArgDouble), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_address(uint, uint64)const", asMETHOD(asIScriptContext, SetArgAddress), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int set_arg_var_type(uint, uint64, typeid)const", asMETHOD(asIScriptContext, SetArgVarType), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "uint64 get_address_of_arg(uint)const", asMETHOD(asIScriptContext, GetAddressOfArg), asCALL_THISCALL);

	engine->RegisterObjectMethod("context", "char get_return_byte()const", asMETHOD(asIScriptContext, GetReturnByte), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "short get_return_word()const", asMETHOD(asIScriptContext, GetReturnWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int32 get_return_dword()const", asMETHOD(asIScriptContext, GetReturnDWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int64 get_return_qword()const", asMETHOD(asIScriptContext, GetReturnQWord), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "float get_return_float()const", asMETHOD(asIScriptContext, GetReturnFloat), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "double get_return_double()const", asMETHOD(asIScriptContext, GetReturnDouble), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "uint64 get_return_address()const", asMETHOD(asIScriptContext, GetReturnAddress), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "uint64 get_address_of_return_value()const", asMETHOD(asIScriptContext, GetAddressOfReturnValue), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "string get_exception_info(bool=true)const", asFUNCTION(ContextGetExceptionInfo), asCALL_GENERIC);
	engine->RegisterGlobalFunction("context@ get_active_context()property", asFUNCTION(asGetActiveContext), asCALL_CDECL);



	engine->RegisterObjectType("module", sizeof(asIScriptModule), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectBehaviour("module", asBEHAVE_FACTORY, "module@ m(const string &in)", asFUNCTION(ModuleFactory), asCALL_CDECL);
	engine->RegisterObjectMethod("module", "int add_script_section(const string &in, const string &in)const", asFUNCTION(ModuleAddScriptSection), asCALL_GENERIC);
	engine->RegisterObjectMethod("module", "int build()const", asMETHOD(asIScriptModule, Build), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "int discard()const", asMETHOD(asIScriptModule, Discard), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "string get_byte_code(bool=true)const", asFUNCTION(ModuleGetByteCode), asCALL_GENERIC);
	engine->RegisterObjectMethod("module", "int set_byte_code(const string &in)const", asFUNCTION(ModuleSetByteCode), asCALL_GENERIC);
	engine->RegisterObjectMethod("module", "function@ get_function_by_index(int)const", asMETHOD(asIScriptModule, GetFunctionByIndex), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "function@ get_function_by_decl(const string &in)const", asMETHOD(asIScriptModule, GetFunctionByDecl), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "function@ get_function_by_name(const string &in)const", asMETHOD(asIScriptModule, GetFunctionByName), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "int bind_all_imported_functions()const", asMETHOD(asIScriptModule, BindAllImportedFunctions), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "int unbind_all_imported_functions()const", asMETHOD(asIScriptModule, UnbindAllImportedFunctions), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "int bind_imported_function(uint, function@)const", asMETHOD(asIScriptModule, BindImportedFunction), asCALL_THISCALL);
	engine->RegisterObjectMethod("module", "int unbind_imported_function(uint)const", asMETHOD(asIScriptModule, UnbindImportedFunction), asCALL_THISCALL);



}
