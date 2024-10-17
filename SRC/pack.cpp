//NGTPACK
#include "obfuscate.h"
#include "pack.h"
using namespace std;
static bool read_bytes(void* data, size_t sz, FILE* fh) {
	return fread(data, sizeof(uint8_t), sz, fh) == (sz * sizeof(uint8_t));
}
static bool file_reader(cmp_ctx_t* ctx, void* data, size_t limit) {
	return read_bytes(data, limit, (FILE*)ctx->buf);
}
static bool file_skipper(cmp_ctx_t* ctx, size_t count) {
	return fseek((FILE*)ctx->buf, count, SEEK_CUR);
}
static size_t file_writer(cmp_ctx_t* ctx, const void* data, size_t count) {
	return fwrite(data, sizeof(uint8_t), count, (FILE*)ctx->buf);
}
const char* pack_header = "NGTPack";
bool  pack::open(const string& filename, const string& open_mode) {
	if (open_mode == "w")mode = "w+b";
	else if (open_mode == "r")mode = "r+b";
	else if (open_mode == "a")mode = "a+b";
	else return false;
	file = fopen(filename.c_str(), mode.c_str());
	if (file == nullptr)
		return false;

	cmp_init(&pctx, file, file_reader, file_skipper, file_writer);
	if (mode == "w+b") {
		cmp_write_str(&pctx, pack_header, strlen(pack_header));
		cmp_write_u64(&pctx, number_of_files);
	}
	else if (mode == "r+b" or mode == "a+b") {
		int len = strlen(pack_header);
		char* header = new char[len];
		uint32_t length = len + 1;
		cmp_read_str(&pctx, header, &length);

		if (strcmp(header, pack_header) != 0)
		{
			delete[] header;
			return false;
		}
		delete[] header;
		cmp_read_u64(&pctx, &number_of_files);
		if (number_of_files > 0)
		{
			for (auto i = 0; i < number_of_files; i++) {
				pkfile f;
				uint32_t file_name_length;
				cmp_read_u32(&pctx, &file_name_length);
				uint64_t file_size;
				cmp_read_u64(&pctx, &file_size);

				char* name = new char[file_name_length + 1];
				uint32_t length = file_name_length + 1;
				cmp_read_str(&pctx, name, &length);

				string file_name = string(name);
				delete[] name;
				Uint64 position = ftell(this->file);
				f.name = file_name;
				f.size = file_size;
				f.position = position;
				files.push_back(f);
				fseek(this->file, file_size, SEEK_CUR);
			}
		}
	}
	return file != nullptr;
}
CScriptArray* pack::list_files() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	if (!active())return array;
	for (auto i = 0; i < files.size(); i++) {
		array->InsertLast(&files[i].name);
	}
	return array;
}
string pack::get_file(const string& internal_name) {
	if (!active())return "";
	if (mode != "r+b") return "";
	string file_name;
	uint64_t it;
	bool found = false;
	for (it = 0; it < files.size(); it++) {
		if (internal_name == files[it].name)
		{
			found = true;
			break;
		}
	}
	if (!found)return "";
	if (files[it].data != "")return files[it].data;
	fseek(file, files[it].position, 0);

	uint64_t size = files[it].size;
	char* file_data = new char[size + 1];
	uint32_t length = size + 1;
	//cmp_read_bin(&pctx, file_data, &length);
	fread(file_data, 1, size, this->file);
	string result = string(file_data, size);
	delete[] file_data;
	files[it].data = result;
	return result;
}
size_t pack::get_file_size(const string& internal_name) {
	if (!active())return 0;
	if (mode != "r+b") return 0;
	string file_name;
	uint64_t it;
	bool found = false;
	for (it = 0; it < files.size(); it++) {
		if (internal_name == files[it].name)
		{
			found = true;
			break;
		}
	}
	if (!found)return 0;
	fseek(file, files[it].position, 0);

	return files[it].size;
}
bool pack::add_file(const string& file, const string& name) {
	if (!active())return false;
	if (mode != "w+b" and mode != "a+b") return false;
	if (file_exists(name)) return false;
	pkfile f;
	FILE* temp = nullptr;
	temp = fopen(file.c_str(), "rb");
	if (temp == nullptr)return false;
	fseek(temp, 0, SEEK_END);
	uint64_t size = ftell(temp);
	fseek(temp, 0, SEEK_SET);
	char* data = new char[size + 1];
	size_t sz = fread(data, 1, size, temp);
	data[sz] = '\0';
	fclose(temp);
	fseek(this->file, strlen(pack_header) + 1, SEEK_SET);
	number_of_files += 1;
	cmp_write_u64(&pctx, number_of_files);
	fseek(this->file, 0, SEEK_END);
	Uint64 position = ftell(this->file);
	cmp_write_u32(&pctx, name.size());
	cmp_write_u64(&pctx, size);
	cmp_write_str(&pctx, name.c_str(), name.size());
	//cmp_write_bin(&pctx, data, size);
	fwrite(data, sizeof(uint8_t), size, this->file);
	f.name = name;
	f.position = position;
	f.size = size;
	files.push_back(f);
	delete[] data;
	return true;
}
bool pack::file_exists(const string& filename) {
	for (Uint64 i = 0; i < files.size(); i++)
		if (files[i].name == filename) return true;
	return false;
}
bool pack::extract_file(const string& internal_name, const string& file_on_disk) {
	FILE* temp = nullptr;
	temp = fopen(file_on_disk.c_str(), "wb");
	if (file == nullptr)
		return false;

	auto data = get_file(internal_name);
	fwrite(data.c_str(), 1, get_file_size(internal_name), temp);
	fclose(temp);
	return true;
}
bool pack::active() const {
	return file != nullptr;
}
int pack::file_count() const {
	return files.size();
}
void pack::close() {
	if (!this->active())return;
	fclose(file);
	files.clear();
	number_of_files = 0;
}

pack::~pack() {
	close();
}
pack* fpack() { return new pack; }
void register_pack(asIScriptEngine* engine) {
	engine->RegisterObjectType("pack", sizeof(pack), asOBJ_REF);
	engine->RegisterObjectBehaviour("pack", asBEHAVE_FACTORY, "pack@ p()", asFUNCTION(fpack), asCALL_CDECL);
	engine->RegisterObjectBehaviour("pack", asBEHAVE_ADDREF, "void f()", asMETHOD(pack, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("pack", asBEHAVE_RELEASE, "void f()", asMETHOD(pack, release), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "bool open(const string &in filename, const string &in open_mode)const", asMETHOD(pack, open), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "void close()const", asMETHOD(pack, close), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "bool file_exists(const string &in filename)const", asMETHOD(pack, file_exists), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "void extract_file(const string &in internal_name, const string &in file_on_disk)const", asMETHOD(pack, extract_file), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "array<string>@ list_files()const", asMETHOD(pack, list_files), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "void add_file(const string &in file_on_disk, const string &in internal_name)const", asMETHOD(pack, add_file), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "string get_file(const string &in filename)const", asMETHOD(pack, get_file), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("pack"), "size_t get_file_size(const string &in filename)const", asMETHOD(pack, get_file_size), asCALL_THISCALL);

	engine->RegisterObjectMethod(_O("pack"), "int get_file_count()const property", asMETHOD(pack, file_count), asCALL_THISCALL);

}
