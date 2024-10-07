#include <angelscript.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string>
#undef GetObject

using namespace std;


class CScriptIOStream {
public:
	std::ostream* outputStream;
	std::istream* inputStream;

	CScriptIOStream() : ref(1), outputStream(nullptr), inputStream(nullptr) {}

	void AddRef() {
		ref += 1;
	}

	void Release() {
		if (--ref < 1) {
			delete this;
		}
	}

	void write(const string& data) {
		*outputStream << data;
	}

	string read() {
		string data;
		*inputStream >> data;
		return data;
	}

	string getline() {
		string data;
		std::getline(*inputStream, data);
		return data;
	}

	void clear() {
		inputStream->clear();
		outputStream->clear();
	}

	void setOutputStream(std::ostream* stream) {
		outputStream = stream;
	}

	void setInputStream(std::istream* stream) {
		inputStream = stream;
	}


	void seekg(size_t pos) {
		inputStream->seekg(static_cast<std::streampos>(pos));
	}

	size_t tellg() {
		return static_cast<size_t>(inputStream->tellg());
	}

	void seekp(size_t pos) {
		outputStream->seekp(static_cast<std::streampos>(pos));
	}

	size_t tellp() {
		return static_cast<size_t>(outputStream->tellp());
	}


private:
	mutable int ref;
};

void WriteGeneric(asIScriptGeneric* gen) {
	CScriptIOStream* stream = (CScriptIOStream*)gen->GetObject();
	void* value = gen->GetArgAddress(0);
	int tid = gen->GetArgTypeId(0);
	std::ostream* ostr = stream->outputStream;
	switch (tid) {
	case asTYPEID_BOOL:
	{
		bool val = *reinterpret_cast<const bool*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_FLOAT:
	{
		float val = *reinterpret_cast<const float*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_DOUBLE:
	{
		double val = *reinterpret_cast<const double*> (value);
		*ostr << val;
		break;
	}
	case asTYPEID_INT8:
	{
		int8_t val = *reinterpret_cast<const int8_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_INT16:
	{
		int16_t val = *reinterpret_cast<const int16_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_INT32:
	{
		int32_t val = *reinterpret_cast<const int32_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_INT64:
	{
		int64_t val = *reinterpret_cast<const int64_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_UINT8:
	{
		uint8_t val = *reinterpret_cast<const uint8_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_UINT16:
	{
		uint16_t val = *reinterpret_cast<const uint16_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_UINT32:
	{
		uint32_t val = *reinterpret_cast<const uint32_t*>(value);
		*ostr << val;
		break;
	}
	case asTYPEID_UINT64:
	{
		uint64_t val = *reinterpret_cast<const uint64_t*>(value);
		*ostr << val;
		break;
	}
	case 67108876:
	{
		const string& str = *static_cast<const string*>(value);
		*ostr << str;
		break;
	}

	default:
		*ostr << value;
		break;
	}
	gen->SetReturnObject(gen->GetObject());
}

void ReadGeneric(asIScriptGeneric* gen) {
	CScriptIOStream* stream = (CScriptIOStream*)gen->GetObject();
	void* value = gen->GetArgAddress(0);
	int tid = gen->GetArgTypeId(0);
	std::istream* istr = stream->inputStream;
	switch (tid) {
	case asTYPEID_BOOL: {
		bool val;
		*istr >> val;
		*reinterpret_cast<bool*>(value) = val;
		break;
	}
	case asTYPEID_FLOAT: {
		float val;
		*istr >> val;
		*reinterpret_cast<float*>(value) = val;
		break;
	}
	case asTYPEID_DOUBLE: {
		double val;
		*istr >> val;
		*reinterpret_cast<double*>(value) = val;
		break;
	}
	case asTYPEID_INT8: {
		int8_t val;
		*istr >> val;
		*reinterpret_cast<int8_t*>(value) = val;
		break;
	}
	case asTYPEID_INT16: {
		int16_t val;
		*istr >> val;
		*reinterpret_cast<int16_t*>(value) = val;
		break;
	}
	case asTYPEID_INT32: {
		int32_t val;
		*istr >> val;
		*reinterpret_cast<int32_t*>(value) = val;
		break;
	}
	case asTYPEID_INT64: {
		int64_t val;
		*istr >> val;
		*reinterpret_cast<int64_t*>(value) = val;
		break;
	}
	case asTYPEID_UINT8: {
		uint8_t val;
		*istr >> val;
		*reinterpret_cast<uint8_t*>(value) = val;
		break;
	}
	case asTYPEID_UINT16: {
		uint16_t val;
		*istr >> val;
		*reinterpret_cast<uint16_t*>(value) = val;
		break;
	}
	case asTYPEID_UINT32: {
		uint32_t val;
		*istr >> val;
		*reinterpret_cast<uint32_t*>(value) = val;
		break;
	}
	case asTYPEID_UINT64: {
		uint64_t val;
		*istr >> val;
		*reinterpret_cast<uint64_t*>(value) = val;
		break;
	}
	case 67108876: { // Assuming this is the type ID for std::string
		std::string str;
		*istr >> str; // Read into a temporary string
		*reinterpret_cast<std::string*>(value) = str; // Assign to the provided address
		break;
	}
	default: {
		break;
	}
	}

	gen->SetReturnObject(gen->GetObject());
}



// Factory function to create a new CScriptIOStream for the specified streams
CScriptIOStream* createStream(std::ostream* outStream, std::istream* inStream) {
	CScriptIOStream* strm = new CScriptIOStream();
	strm->setOutputStream(outStream);
	strm->setInputStream(inStream);
	return strm;
}

CScriptIOStream* get_cout() {
	return createStream(&std::cout, nullptr);
}

CScriptIOStream* get_cin() {
	return createStream(nullptr, &std::cin);
}

CScriptIOStream* get_cerr() {
	return createStream(&std::cerr, nullptr);
}

CScriptIOStream* IostreamFactory() {
	return new CScriptIOStream;
}

void RegisterScriptIOStreams(asIScriptEngine* engine) {
	engine->RegisterObjectType("iostream", sizeof(CScriptIOStream), asOBJ_REF);
	engine->RegisterObjectBehaviour("iostream", asBEHAVE_FACTORY, "iostream@ io()", asFUNCTION(IostreamFactory), asCALL_CDECL);

	engine->RegisterObjectBehaviour("iostream", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptIOStream, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("iostream", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptIOStream, Release), asCALL_THISCALL);

	engine->RegisterObjectMethod("iostream", "void write(const string &in)", asMETHOD(CScriptIOStream, write), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "string read()", asMETHOD(CScriptIOStream, read), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "string getline()", asMETHOD(CScriptIOStream, getline), asCALL_THISCALL);

	engine->RegisterObjectMethod("iostream", "void clear()", asMETHOD(CScriptIOStream, clear), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "void seekg(size_t)", asMETHOD(CScriptIOStream, seekg), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "size_t tellg()", asMETHOD(CScriptIOStream, tellg), asCALL_THISCALL);

	engine->RegisterObjectMethod("iostream", "void seekp(size_t)", asMETHOD(CScriptIOStream, seekp), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "size_t tellp()", asMETHOD(CScriptIOStream, tellp), asCALL_THISCALL);
	engine->RegisterObjectMethod("iostream", "iostream& opShl(?&in)", asFUNCTION(WriteGeneric), asCALL_GENERIC);
	engine->RegisterObjectMethod("iostream", "iostream& opShr(?&out)", asFUNCTION(ReadGeneric), asCALL_GENERIC);


	engine->RegisterGlobalFunction("iostream@ get_cout()property", asFUNCTION(get_cout), asCALL_CDECL);
	engine->RegisterGlobalFunction("iostream@ get_cin()property", asFUNCTION(get_cin), asCALL_CDECL);
	engine->RegisterGlobalFunction("iostream@ get_cerr()property", asFUNCTION(get_cerr), asCALL_CDECL);
}
