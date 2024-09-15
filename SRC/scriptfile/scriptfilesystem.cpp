#include "../autowrapper/aswrappedcall.h"
#include "scriptfilesystem.h"
#include <string.h> // strstr()

#if defined(_WIN32)
#include <direct.h> // _getcwd
#include <Windows.h> // FindFirstFile, GetFileAttributes

#undef DeleteFile
#undef CopyFile

#else
#include <unistd.h> // getcwd
#include <dirent.h> // opendir, readdir, closedir
#include <sys/stat.h> // stat
#endif
#include <assert.h> // assert
#include <filesystem>
#include "Poco/Glob.h"
#include "Poco/Exception.h"
using namespace std;

BEGIN_AS_NAMESPACE

// TODO: The file system should have a way to allow the application to define in
//       which sub directories it is allowed to make changes and/or read

CScriptFileSystem* ScriptFileSystem_Factory()
{
	return new CScriptFileSystem();
}

void RegisterScriptFileSystem(asIScriptEngine* engine)
{
	int r;

	assert(engine->GetTypeInfoByName("string"));
	assert(engine->GetTypeInfoByDecl("array<string>"));
	assert(engine->GetTypeInfoByName("datetime"));

	r = engine->RegisterObjectType("filesystem", 0, asOBJ_REF); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("filesystem", asBEHAVE_FACTORY, "filesystem @f()", asFUNCTION(ScriptFileSystem_Factory), asCALL_CDECL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("filesystem", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptFileSystem, AddRef), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("filesystem", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptFileSystem, Release), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "bool file_exists(const string &in)", asMETHOD(CScriptFileSystem, FileExists), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "bool dir_exists(const string &in)", asMETHOD(CScriptFileSystem, DirExists), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "bool change_current_path(const string &in)", asMETHOD(CScriptFileSystem, ChangeCurrentPath), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "string get_current_path() const property", asMETHOD(CScriptFileSystem, GetCurrentPath), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "array<string> @get_dirs() const property", asMETHOD(CScriptFileSystem, GetDirs), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "array<string> @get_files(string=\"*\") const", asMETHOD(CScriptFileSystem, GetFiles), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "bool is_dir(const string &in) const", asMETHOD(CScriptFileSystem, IsDir), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "bool is_link(const string &in) const", asMETHOD(CScriptFileSystem, IsLink), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int64 get_size(const string &in) const", asMETHOD(CScriptFileSystem, GetSize), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int make_dir(const string &in)", asMETHOD(CScriptFileSystem, MakeDir), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int remove_dir(const string &in)", asMETHOD(CScriptFileSystem, RemoveDir), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int delete_file(const string &in)", asMETHOD(CScriptFileSystem, DeleteFile), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int copy_file(const string &in, const string &in)", asMETHOD(CScriptFileSystem, CopyFile), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "int move(const string &in, const string &in)", asMETHOD(CScriptFileSystem, Move), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "datetime get_create_date_time(const string &in) const", asMETHOD(CScriptFileSystem, GetCreateDateTime), asCALL_THISCALL); assert(r >= 0);
	r = engine->RegisterObjectMethod("filesystem", "datetime get_modify_date_time(const string &in) const", asMETHOD(CScriptFileSystem, GetModifyDateTime), asCALL_THISCALL); assert(r >= 0);
}

CScriptFileSystem::CScriptFileSystem()
{
	refCount = 1;

	// Gets the application's current working directory as the starting point
	// TODO: Replace backslash with slash to keep a unified naming convention
	char buffer[1000];
#if defined(_WIN32)
	currentPath = _getcwd(buffer, 1000);
#else
	currentPath = getcwd(buffer, 1000);
#endif
}

CScriptFileSystem::~CScriptFileSystem()
{
}

void CScriptFileSystem::AddRef() const
{
	asAtomicInc(refCount);
}

void CScriptFileSystem::Release() const
{
	if (asAtomicDec(refCount) == 0)
		delete this;
}
bool CScriptFileSystem::FileExists(const std::string& file) {
	return filesystem::exists(file);
}
bool CScriptFileSystem::DirExists(const std::string& dir) {
	return filesystem::exists(dir);
}
bool FilenameMatch(const std::string& file, const std::string& pattern) {
	try {
		return Poco::Glob(pattern).match(file);
	}
	catch (Poco::Exception& e) { return false; }
}
CScriptArray* CScriptFileSystem::GetFiles(std::string path) const
{
	if (path == "")
		path = currentPath;
	// Obtain a pointer to the engine
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();

	// TODO: This should only be done once
	// TODO: This assumes that CScriptArray was already registered
	asITypeInfo* arrayType = engine->GetTypeInfoByDecl("array<string>");

	// Create the array object
	CScriptArray* array = CScriptArray::Create(arrayType);

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[1024];
	MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, bufUTF16, 1024);

	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(bufUTF16, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		return array;

	do {
		// Skip directories
		if ((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		// Convert the file name back to UTF8
		char bufUTF8[1024];
		WideCharToMultiByte(CP_UTF8, 0, ffd.cFileName, -1, bufUTF8, 1024, 0, 0);

		// Add the file to the array
		array->Resize(array->GetSize() + 1);
		((string*)(array->At(array->GetSize() - 1)))->assign(bufUTF8);
	} while (FindNextFileW(hFind, &ffd) != 0);

	FindClose(hFind);
#else
	int wildcard = path.rfind('/');
	if (wildcard == std::string::npos) wildcard = path.rfind('\\');
	string currentPath = path;
	string Wildcard = "*";
	if (wildcard != std::string::npos) {
		currentPath = path.substr(0, wildcard + 1);
		Wildcard = path.substr(wildcard + 1);
	}
	else {
		currentPath = "./";
		Wildcard = path;
	}
	dirent* ent = 0;
	DIR* dir = opendir(currentPath.c_str());
	if (!dir) return array;
	while ((ent = readdir(dir)) != NULL) {
		const string filename = ent->d_name;

		// Skip . and ..
		if (filename[0] == '.')
			continue;

		// Skip sub directories
		string fullname = currentPath + filename;
		struct stat st;
		if (stat(fullname.c_str(), &st) == -1)
			continue;
		if ((st.st_mode & S_IFDIR) != 0)
			continue;

		// wildcard matching
		if (!FilenameMatch(filename, Wildcard)) continue;

		// Add the file to the array
		array->Resize(array->GetSize() + 1);
		((string*)(array->At(array->GetSize() - 1)))->assign(filename);
	}
	closedir(dir);
#endif

	return array;
}

CScriptArray* CScriptFileSystem::GetDirs() const
{
	// Obtain a pointer to the engine
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();

	// TODO: This should only be done once
	// TODO: This assumes that CScriptArray was already registered
	asITypeInfo* arrayType = engine->GetTypeInfoByDecl("array<string>");

	// Create the array object
	CScriptArray* array = CScriptArray::Create(arrayType);

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	string searchPattern = currentPath + "/*";
	MultiByteToWideChar(CP_UTF8, 0, searchPattern.c_str(), -1, bufUTF16, 10000);

	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(bufUTF16, &ffd);
	if (INVALID_HANDLE_VALUE == hFind)
		return array;

	do
	{
		// Skip files
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;

		// Convert the file name back to UTF8
		char bufUTF8[10000];
		WideCharToMultiByte(CP_UTF8, 0, ffd.cFileName, -1, bufUTF8, 10000, 0, 0);

		if (strcmp(bufUTF8, ".") == 0 || strcmp(bufUTF8, "..") == 0)
			continue;

		// Add the dir to the array
		array->Resize(array->GetSize() + 1);
		((string*)(array->At(array->GetSize() - 1)))->assign(bufUTF8);
	} while (FindNextFileW(hFind, &ffd) != 0);

	FindClose(hFind);
#else
	dirent* ent = 0;
	DIR* dir = opendir(currentPath.c_str());
	while ((ent = readdir(dir)) != NULL)
	{
		const string filename = ent->d_name;

		// Skip . and ..
		if (filename[0] == '.')
			continue;

		// Skip files
		const string fullname = currentPath + "/" + filename;
		struct stat st;
		if (stat(fullname.c_str(), &st) == -1)
			continue;
		if ((st.st_mode & S_IFDIR) == 0)
			continue;

		// Add the dir to the array
		array->Resize(array->GetSize() + 1);
		((string*)(array->At(array->GetSize() - 1)))->assign(filename);
	}
	closedir(dir);
#endif

	return array;
}

// Doesn't change anything if the new path is not valid
bool CScriptFileSystem::ChangeCurrentPath(const string& path)
{
	string newPath;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		newPath = path;
	else
		newPath = currentPath + "/" + path;

	// TODO: Resolve internal /./ and /../
	// TODO: Replace backslash with slash to keep a unified naming convention

	// Remove trailing slashes from the path
	while (newPath.length() && (newPath[newPath.length() - 1] == '/' || newPath[newPath.length() - 1] == '\\'))
		newPath.resize(newPath.length() - 1);

	if (!IsDir(newPath))
		return false;

	currentPath = newPath;
	return true;
}

bool CScriptFileSystem::IsDir(const string& path) const
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Check if the path exists and is a directory
	DWORD attrib = GetFileAttributesW(bufUTF16);
	if (attrib == INVALID_FILE_ATTRIBUTES ||
		!(attrib & FILE_ATTRIBUTE_DIRECTORY))
		return false;
#else
	// Check if the path exists and is a directory
	struct stat st;
	if (stat(search.c_str(), &st) == -1)
		return false;
	if ((st.st_mode & S_IFDIR) == 0)
		return false;
#endif

	return true;
}

bool CScriptFileSystem::IsLink(const string& path) const
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Check if the path exists and is a link
	DWORD attrib = GetFileAttributesW(bufUTF16);
	if (attrib == INVALID_FILE_ATTRIBUTES ||
		!(attrib & FILE_ATTRIBUTE_REPARSE_POINT))
		return false;
#else
	// Check if the path exists and is a link
	struct stat st;
	if (stat(search.c_str(), &st) == -1)
		return false;
	if ((st.st_mode & S_IFLNK) == 0)
		return false;
#endif

	return true;
}

asINT64 CScriptFileSystem::GetSize(const string& path) const
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Get the size of the file
	LARGE_INTEGER largeInt;
	HANDLE file = CreateFileW(bufUTF16, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL success = GetFileSizeEx(file, &largeInt);
	CloseHandle(file);
	if (!success)
		return -1;
	return asINT64(largeInt.QuadPart);
#else
	// Get the size of the file
	struct stat st;
	if (stat(search.c_str(), &st) == -1)
		return -1;
	return asINT64(st.st_size);
#endif
}

// TODO: Should be able to return different codes for
//       - directory exists
//       - path not found
//       - access denied
// TODO: Should be able to define the permissions for the directory
// TODO: Should support recursively creating directories
int CScriptFileSystem::MakeDir(const string& path)
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Create the directory
	BOOL success = CreateDirectoryW(bufUTF16, 0);
	return success ? 0 : -1;
#else
	// Create the directory
	int failure = mkdir(search.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return !failure ? 0 : -1;
#endif
}

// TODO: Should be able to return different codes for
//       - directory doesn't exist
//       - directory is not empty
//       - access denied
// TODO: Should have an option to remove the directory and all content recursively
int CScriptFileSystem::RemoveDir(const string& path)
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Remove the directory
	BOOL success = RemoveDirectoryW(bufUTF16);
	return success ? 0 : -1;
#else
	// Remove the directory
	int failure = rmdir(search.c_str());
	return !failure ? 0 : -1;
#endif
}

int CScriptFileSystem::DeleteFile(const string& path)
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Remove the file
	BOOL success = DeleteFileW(bufUTF16);
	return success ? 0 : -1;
#else
	// Remove the file
	int failure = unlink(search.c_str());
	return !failure ? 0 : -1;
#endif
}

int CScriptFileSystem::CopyFile(const string& source, const string& target)
{
	string search1;
	if (source.find(":") != string::npos || source.find("/") == 0 || source.find("\\") == 0)
		search1 = source;
	else
		search1 = currentPath + "/" + source;

	string search2;
	if (target.find(":") != string::npos || target.find("/") == 0 || target.find("\\") == 0)
		search2 = target;
	else
		search2 = currentPath + "/" + target;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16_1[10000];
	MultiByteToWideChar(CP_UTF8, 0, search1.c_str(), -1, bufUTF16_1, 10000);

	wchar_t bufUTF16_2[10000];
	MultiByteToWideChar(CP_UTF8, 0, search2.c_str(), -1, bufUTF16_2, 10000);

	// Copy the file
	BOOL success = CopyFileW(bufUTF16_1, bufUTF16_2, TRUE);
	return success ? 0 : -1;
#else
	// Copy the file manually as there is no posix function for this
	bool failure = false;
	FILE* src = 0, * tgt = 0;
	src = fopen(search1.c_str(), "r");
	if (src == 0) failure = true;
	if (!failure) tgt = fopen(search2.c_str(), "w");
	if (tgt == 0) failure = true;
	char buf[1024];
	size_t n;
	while (!failure && (n = fread(buf, sizeof(char), sizeof(buf), src)) > 0)
	{
		if (fwrite(buf, sizeof(char), n, tgt) != n)
			failure = true;
	}
	if (src) fclose(src);
	if (tgt) fclose(tgt);
	return !failure ? 0 : -1;
#endif
}

int CScriptFileSystem::Move(const string& source, const string& target)
{
	string search1;
	if (source.find(":") != string::npos || source.find("/") == 0 || source.find("\\") == 0)
		search1 = source;
	else
		search1 = currentPath + "/" + source;

	string search2;
	if (target.find(":") != string::npos || target.find("/") == 0 || target.find("\\") == 0)
		search2 = target;
	else
		search2 = currentPath + "/" + target;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16_1[10000];
	MultiByteToWideChar(CP_UTF8, 0, search1.c_str(), -1, bufUTF16_1, 10000);

	wchar_t bufUTF16_2[10000];
	MultiByteToWideChar(CP_UTF8, 0, search2.c_str(), -1, bufUTF16_2, 10000);

	// Move the file or directory
	BOOL success = MoveFileW(bufUTF16_1, bufUTF16_2);
	return success ? 0 : -1;
#else
	// Move the file or directory
	int failure = rename(search1.c_str(), search2.c_str());
	return !failure ? 0 : -1;
#endif
}

string CScriptFileSystem::GetCurrentPath() const
{
	return currentPath;
}

CDateTime CScriptFileSystem::GetCreateDateTime(const string& path) const
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Get the create date/time of the file
	FILETIME createTm;
	HANDLE file = CreateFileW(bufUTF16, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL success = GetFileTime(file, &createTm, 0, 0);
	CloseHandle(file);
	if (!success)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Failed to get file creation date/time");
		return CDateTime();
	}
	SYSTEMTIME tm;
	FileTimeToSystemTime(&createTm, &tm);
	return CDateTime(tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
#else
	// Get the create date/time of the file
	struct stat st;
	if (stat(search.c_str(), &st) == -1)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Failed to get file creation date/time");
		return CDateTime();
	}
	tm* t = localtime(&st.st_ctime);
	return CDateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#endif
}

CDateTime CScriptFileSystem::GetModifyDateTime(const string& path) const
{
	string search;
	if (path.find(":") != string::npos || path.find("/") == 0 || path.find("\\") == 0)
		search = path;
	else
		search = currentPath + "/" + path;

#if defined(_WIN32)
	// Windows uses UTF16 so it is necessary to convert the string
	wchar_t bufUTF16[10000];
	MultiByteToWideChar(CP_UTF8, 0, search.c_str(), -1, bufUTF16, 10000);

	// Get the last modify date/time of the file
	FILETIME modifyTm;
	HANDLE file = CreateFileW(bufUTF16, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	BOOL success = GetFileTime(file, 0, 0, &modifyTm);
	CloseHandle(file);
	if (!success)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Failed to get file modify date/time");
		return CDateTime();
	}
	SYSTEMTIME tm;
	FileTimeToSystemTime(&modifyTm, &tm);
	return CDateTime(tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
#else
	// Get the last modify date/time of the file
	struct stat st;
	if (stat(search.c_str(), &st) == -1)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if (ctx)
			ctx->SetException("Failed to get file modify date/time");
		return CDateTime();
	}
	tm* t = localtime(&st.st_mtime);
	return CDateTime(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
#endif
}

END_AS_NAMESPACE
