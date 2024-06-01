#include "ngt.h"
#include "pack.h"
#include "pack/physfs.h"
#include "scriptarray/scriptarray.h"
#include<string>
using namespace std;
const int PACK_OPEN_MODE_RIGHT = 1;
const int PACK_OPEN_MODE_READ = 2;
const int PACK_OPEN_MODE_APPEND = 3;
class pack {
private:
	PHYSFS_File* file = nullptr;
	int mode;
public:
	pack() {
		PHYSFS_init("");
	}
	bool  open(const string& filename, int open_mode) {
		PHYSFS_setWriteDir("./");
		if (open_mode == PACK_OPEN_MODE_RIGHT)
			file = PHYSFS_openWrite(filename.c_str());
		else if (open_mode == PACK_OPEN_MODE_READ)
			file = PHYSFS_openRead(filename.c_str());
		else if (open_mode == PACK_OPEN_MODE_APPEND)
			file = PHYSFS_openAppend(filename.c_str());
		else {
			asIScriptContext* ctx = asGetActiveContext();
			ctx->SetException("'PACK_OPEN_MODE_NONE' you chosed. A file will not open, because wrong constant selected.");
			return false;
		}
		PHYSFS_ErrorCode code = PHYSFS_getLastErrorCode();
		std::string result(PHYSFS_getErrorByCode(code));
		alert("W", result);
		mode = open_mode;
		return file != nullptr;
	}
	CScriptArray* list_files() {
		return nullptr;
	}
	string get_file(const string& internal_name) {
		return "";
	}
	void add_file(const string& file, const string& name) {
	}
	bool file_exists(const string& filename) {
		return false;
	}
	bool extract_file(const string& internal_name, const string& file_on_disk) {
		return false;
	}
	bool active() const {
		return false;
	}
	int file_count() const {
		return 0;
	}
	void close() {
	}

	~pack() {
		close();
	}
};
pack* fpack() { return new pack; }
void register_pack(asIScriptEngine* engine) {
	engine->RegisterObjectType("pack", sizeof(pack), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectBehaviour("pack", asBEHAVE_FACTORY, "pack@ p()", asFUNCTION(fpack), asCALL_CDECL);
	engine->RegisterObjectMethod("pack", "bool open(const string &in, int)const", asMETHOD(pack, open), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "void close()const", asMETHOD(pack, close), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "bool file_exists(const string &in)const", asMETHOD(pack, file_exists), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "void extract_file(const string &in, const string &in)const", asMETHOD(pack, extract_file), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "array<string>@ list_files()const", asMETHOD(pack, list_files), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "void add_file(const string &in, const string &in)const", asMETHOD(pack, add_file), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "string get_file(const string &in)const", asMETHOD(pack, get_file), asCALL_THISCALL);
	engine->RegisterObjectMethod("pack", "int get_file_count()const property", asMETHOD(pack, file_count), asCALL_THISCALL);

}
