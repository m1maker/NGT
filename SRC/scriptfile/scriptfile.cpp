#include "scriptfile.h"
#include <assert.h>
#include <new>
#include <stdio.h>
#include <string>
#include <string.h>

#ifdef _WIN32_WCE
#include <windows.h> // For GetModuleFileName
#ifdef GetObject
#undef GetObject
#endif
#endif

using namespace std;

BEGIN_AS_NAMESPACE

CScriptFile* ScriptFile_Factory()
{
	return new CScriptFile();
}

void ScriptFile_Factory_Generic(asIScriptGeneric* gen)
{
	*(CScriptFile**)gen->GetAddressOfReturnLocation() = ScriptFile_Factory();
}

void ScriptFile_AddRef_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	file->AddRef();
}

void ScriptFile_Release_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	file->Release();
}

void ScriptFile_Open_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	std::string* f = (std::string*)gen->GetArgAddress(0);
	std::string* m = (std::string*)gen->GetArgAddress(1);
	int r = file->Open(*f, *m);
	gen->SetReturnDWord(r);
}

void ScriptFile_Close_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	int r = file->Close();
	gen->SetReturnDWord(r);
}

void ScriptFile_GetSize_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	int r = file->GetSize();
	gen->SetReturnDWord(r);
}

void ScriptFile_Read_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	int len = gen->GetArgDWord(0);
	string str = file->Read(len);
	gen->SetReturnObject(&str);
}

void ScriptFile_ReadLine_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	std::string str = file->ReadLine();
	gen->SetReturnObject(&str);
}

void ScriptFile_Write_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	std::string* str = (std::string*)gen->GetArgAddress(0);
	gen->SetReturnDWord(file->Write(*str));
}

void ScriptFile_IsEOF_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	bool r = file->IsEOF();
	gen->SetReturnByte(r);
}

void ScriptFile_GetPos_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	gen->SetReturnDWord(file->GetPos());
}

void ScriptFile_SetPos_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	int pos = (int)gen->GetArgDWord(0);
	gen->SetReturnDWord(file->SetPos(pos));
}

void ScriptFile_MovePos_Generic(asIScriptGeneric* gen)
{
	CScriptFile* file = (CScriptFile*)gen->GetObject();
	int delta = (int)gen->GetArgDWord(0);
	gen->SetReturnDWord(file->MovePos(delta));
}

void RegisterScriptFile_Native(asIScriptEngine* engine)
{
	int r;

	r = engine->RegisterObjectType("file", 0, asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_FACTORY, "file @f()", asFUNCTION(ScriptFile_Factory), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptFile, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptFile, Release), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectMethod("file", "int open(const string &in, const string &in)", asMETHOD(CScriptFile, Open), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int close()", asMETHOD(CScriptFile, Close), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int get_size() const property", asMETHOD(CScriptFile, GetSize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "bool is_end_of_file() const", asMETHOD(CScriptFile, IsEOF), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string read(uint=0)", asMETHOD(CScriptFile, Read), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string read_line()", asMETHOD(CScriptFile, ReadLine), asCALL_THISCALL); assert(r >= 0);
#if AS_WRITE_OPS == 1
	r = engine->RegisterObjectMethod("file", "int write(const string &in)", asMETHOD(CScriptFile, Write), asCALL_THISCALL); assert(r >= 0);
#endif
	r = engine->RegisterObjectMethod("file", "int get_pos() const property", asMETHOD(CScriptFile, GetPos), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int set_pos(int)", asMETHOD(CScriptFile, SetPos), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int move_pos(int)", asMETHOD(CScriptFile, MovePos), asCALL_THISCALL); assert(r >= 0);

	r = engine->RegisterObjectProperty("file", "bool most_significant_byte_first", asOFFSET(CScriptFile, mostSignificantByteFirst)); assert(r >= 0);
}

void RegisterScriptFile_Generic(asIScriptEngine* engine)
{
	int r;

	r = engine->RegisterObjectType("file", 0, asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_FACTORY, "file @f()", asFUNCTION(ScriptFile_Factory_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_ADDREF, "void f()", asFUNCTION(ScriptFile_AddRef_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("file", asBEHAVE_RELEASE, "void f()", asFUNCTION(ScriptFile_Release_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectMethod("file", "int open(const string &in, const string &in)", asFUNCTION(ScriptFile_Open_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int close()", asFUNCTION(ScriptFile_Close_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int get_size() const property", asFUNCTION(ScriptFile_GetSize_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "bool is_end_of_file() const", asFUNCTION(ScriptFile_IsEOF_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string read(uint=0)", asFUNCTION(ScriptFile_Read_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "string read_line()", asFUNCTION(ScriptFile_ReadLine_Generic), asCALL_GENERIC); assert(r >= 0);
#if AS_WRITE_OPS == 1
	r = engine->RegisterObjectMethod("file", "int write(const string &in)", asFUNCTION(ScriptFile_Write_Generic), asCALL_GENERIC); assert(r >= 0);
#endif
	r = engine->RegisterObjectMethod("file", "int get_pos() const property", asFUNCTION(ScriptFile_GetPos_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int set_pos(int)", asFUNCTION(ScriptFile_SetPos_Generic), asCALL_GENERIC); assert(r >= 0);
	r = engine->RegisterObjectMethod("file", "int move_pos(int)", asFUNCTION(ScriptFile_MovePos_Generic), asCALL_GENERIC); assert(r >= 0);

	r = engine->RegisterObjectProperty("file", "bool most_significant_byte_first", asOFFSET(CScriptFile, mostSignificantByteFirst)); assert(r >= 0);
}

void RegisterScriptFile(asIScriptEngine* engine)
{
	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
		RegisterScriptFile_Generic(engine);
	else
		RegisterScriptFile_Native(engine);
}

CScriptFile::CScriptFile()
{
	refCount = 1;
	file = 0;
	mostSignificantByteFirst = false;
}

CScriptFile::~CScriptFile()
{
	Close();
}

void CScriptFile::AddRef() const
{
	asAtomicInc(refCount);
}

void CScriptFile::Release() const
{
	if (asAtomicDec(refCount) == 0)
		delete this;
}

int CScriptFile::Open(const std::string& filename, const std::string& mode)
{
	// Close the previously opened file handle
	if (file)
		Close();

	std::string myFilename = filename;

	// Validate the mode
	string m;
#if AS_WRITE_OPS == 1
	if (mode != "r" && mode != "w" && mode != "a")
#else
	if (mode != "r")
#endif
		return -1;
	else
		m = mode;

#ifdef _WIN32_WCE
	// no relative pathing on CE
	char buf[MAX_PATH];
	static TCHAR apppath[MAX_PATH] = TEXT("");
	if (!apppath[0])
	{
		GetModuleFileName(NULL, apppath, MAX_PATH);

		int appLen = _tcslen(apppath);
		while (appLen > 1)
		{
			if (apppath[appLen - 1] == TEXT('\\'))
				break;
			appLen--;
		}

		// Terminate the string after the trailing backslash
		apppath[appLen] = TEXT('\0');
	}
#ifdef _UNICODE
	wcstombs(buf, apppath, wcslen(apppath) + 1);
#else
	memcpy(buf, apppath, strlen(apppath));
#endif
	myFilename = buf + myFilename;
#endif


	// By default windows translates "\r\n" to "\n", but we want to read the file as-is.
	m += "b";

	// Open the file
#if _MSC_VER >= 1400 && !defined(__S3E__) 
	// MSVC 8.0 / 2005 introduced new functions 
	// Marmalade doesn't use these, even though it uses the MSVC compiler
	fopen_s(&file, myFilename.c_str(), m.c_str());
#else
	file = fopen(myFilename.c_str(), m.c_str());
#endif
	if (file == 0)
		return -1;

	return 0;
}

int CScriptFile::Close()
{
	if (file == 0)
		return -1;

	fclose(file);
	file = 0;

	return 0;
}

int CScriptFile::GetSize() const
{
	if (file == 0)
		return -1;

	int pos = ftell(file);
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fseek(file, pos, SEEK_SET);

	return size;
}

int CScriptFile::GetPos() const
{
	if (file == 0)
		return -1;

	return ftell(file);
}

int CScriptFile::SetPos(int pos)
{
	if (file == 0)
		return -1;

	int r = fseek(file, pos, SEEK_SET);

	// Return -1 on error
	return r ? -1 : 0;
}

int CScriptFile::MovePos(int delta)
{
	if (file == 0)
		return -1;

	int r = fseek(file, delta, SEEK_CUR);

	// Return -1 on error
	return r ? -1 : 0;
}

string CScriptFile::Read(unsigned int length)
{
	if (file == 0)
		return "";
	if (length == 0)length = this->GetSize();
	// Read the string
	string str;
	str.resize(length);
	int size = (int)fread(&str[0], 1, length, file);
	str.resize(size);

	return str;
}

string CScriptFile::ReadLine()
{
	if (file == 0)
		return "";

	// Read until the first new-line character
	string str;
	char buf[256];

	do
	{
		// Get the current position so we can determine how many characters were read
		int start = ftell(file);

		// Set the last byte to something different that 0, so that we can check if the buffer was filled up
		buf[255] = 1;

		// Read the line (or first 255 characters, which ever comes first)
		char* r = fgets(buf, 256, file);
		if (r == 0) break;

		// Get the position after the read
		int end = ftell(file);

		// Add the read characters to the output buffer
		str.append(buf, end - start);
	} while (!feof(file) && buf[255] == 0 && buf[254] != '\n');

	return str;
}

bool CScriptFile::IsEOF() const
{
	if (file == 0)
		return true;

	return feof(file) ? true : false;
}

#if AS_WRITE_OPS == 1
int CScriptFile::Write(const std::string& str)
{
	if (file == 0)
		return -1;

	// Write the entire string
	size_t r = fwrite(&str[0], 1, str.length(), file);

	return int(r);
}

#endif


END_AS_NAMESPACE
