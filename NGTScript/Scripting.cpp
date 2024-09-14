#include "Scripting.h"
#include<string>
int RegisterFunction(const std::string& signature, void* function_handle, int callconv) {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	return engine->RegisterGlobalFunction(signature.c_str(), asFUNCTION(function_handle), callconv);
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


	engine->RegisterObjectType("function", sizeof(asIScriptFunction), asOBJ_REF);
	engine->RegisterObjectBehaviour("function", asBEHAVE_FACTORY, "function@ f(int)", asFUNCTION(FunctionFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("function", asBEHAVE_ADDREF, "void f()", asMETHOD(asIScriptFunction, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("function", asBEHAVE_RELEASE, "void f()", asMETHOD(asIScriptFunction, Release), asCALL_THISCALL);


	engine->RegisterObjectType("context", sizeof(asIScriptContext), asOBJ_REF);
	engine->RegisterObjectBehaviour("context", asBEHAVE_FACTORY, "context@ c()", asFUNCTION(CtxFactory), asCALL_CDECL);
	engine->RegisterObjectBehaviour("context", asBEHAVE_ADDREF, "void f()", asMETHOD(asIScriptContext, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("context", asBEHAVE_RELEASE, "void f()", asMETHOD(asIScriptContext, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int prepare(function@)const", asMETHOD(asIScriptContext, Prepare), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int unprepare()const", asMETHOD(asIScriptContext, Unprepare), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int execute()const", asMETHOD(asIScriptContext, Execute), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int abort()const", asMETHOD(asIScriptContext, Abort), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int suspend()const", asMETHOD(asIScriptContext, Suspend), asCALL_THISCALL);
	engine->RegisterObjectMethod("context", "int get_state()const", asMETHOD(asIScriptContext, GetState), asCALL_THISCALL);
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



}
