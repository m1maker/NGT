#ifndef NGTPACK_H
#define NGTPACK_H
#pragma once
#include "angelscript.h"
#include "as_class.h"
#include "cmp.h"
#include "ngt.h"
#include "scriptarray/scriptarray.h"
#include <filesystem>
#include <stdint.h>
#include <stdio.h>
#include<string>
#include <vector>
using namespace std;
typedef struct {
	string name;
	uint64_t size;
	uint64_t position;
	string data;
}pkfile;
class pack : public as_class {
private:
	FILE* file = nullptr;
	string mode;
	cmp_ctx_t pctx = { 0 };
	std::vector<pkfile> files;
	uint64_t number_of_files = 0;
public:
	bool  open(const string& filename, const string& open_mode);
	CScriptArray* list_files();
	string get_file(const string& internal_name);
	size_t get_file_size(const string& internal_name);
	bool add_file(const string& file, const string& name);
	bool file_exists(const string& filename);
	bool extract_file(const string& internal_name, const string& file_on_disk);
	bool active()const;
	int file_count() const;
	void close();
	~pack();
};
void register_pack(asIScriptEngine* engine);
#endif