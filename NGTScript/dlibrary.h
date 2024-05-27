#ifndef DLIB_H
#define DLIB_H
#include<string>
class dlibrary {
public:
	bool load(const std::string &name);
	void* get(const char* function_name);
	bool get_active()const;
	bool unload();
private:
void* lib;
};
#endif