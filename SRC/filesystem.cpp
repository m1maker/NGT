#include "datetime/datetime.h"
#include "filesystem.h"
#include "obfuscate.h"
#include "scriptarray/scriptarray.h"
#include <angelscript.h>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <Poco/Glob.h>
#include <SDL3/SDL.h>
#include <string>
#include <vector>

std::mutex dialog_mutex;
std::condition_variable dialog_cv;
bool dialog_done = false;
std::vector<std::string> selected_files;

void file_open_callback(void* userdata, const char* const* filelist, int filter) {
	std::lock_guard<std::mutex> lock(dialog_mutex);
	dialog_done = true;

	if (filelist != nullptr) {
		for (int i = 0; filelist[i] != nullptr; ++i) {
			selected_files.push_back(filelist[i]);
		}
	}
	dialog_cv.notify_one();
}

std::string simple_file_open_dialog(const std::string& filters = "All files:*", const std::string& default_location = "") {
	selected_files.clear();
	dialog_done = false;

	// Parse filters
	SDL_DialogFileFilter filter_array[1]; // Assuming only one filter for simplicity
	filter_array[0].name = filters.c_str();
	filter_array[0].pattern = filters.c_str();

	SDL_ShowOpenFileDialog(file_open_callback, nullptr, SDL_GetKeyboardFocus(), filter_array, 1, default_location.c_str(), false);

	// Wait for the dialog to finish
	while (!dialog_done) {
		SDL_PumpEvents();  // Process pending events
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Return the first selected file or an empty string if none were selected
	return selected_files.empty() ? "" : selected_files.front();
}

void file_save_callback(void* userdata, const char* const* filelist, int filter) {
	std::lock_guard<std::mutex> lock(dialog_mutex);
	dialog_done = true;

	if (filelist != nullptr && filelist[0] != nullptr) {
		selected_files.push_back(filelist[0]);
	}
	dialog_cv.notify_one();
}

std::string simple_file_save_dialog(const std::string& filters, const std::string& default_location) {
	selected_files.clear();
	dialog_done = false;

	// Parse filters
	SDL_DialogFileFilter filter_array[1]; // Assuming only one filter for simplicity
	filter_array[0].name = filters.c_str();
	filter_array[0].pattern = filters.c_str();

	SDL_ShowSaveFileDialog(file_save_callback, nullptr, SDL_GetKeyboardFocus(), filter_array, 1, default_location.c_str());

	// Wait for the dialog to finish
	while (!dialog_done) {
		SDL_PumpEvents();  // Process pending events
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Return the selected file or an empty string if none was selected
	return selected_files.empty() ? "" : selected_files.front();
}

void folder_select_callback(void* userdata, const char* const* filelist, int filter) {
	std::lock_guard<std::mutex> lock(dialog_mutex);
	dialog_done = true;

	if (filelist != nullptr && filelist[0] != nullptr) {
		selected_files.push_back(filelist[0]);
	}
	dialog_cv.notify_one();
}

std::string simple_folder_select_dialog(const std::string& default_location) {
	selected_files.clear();
	dialog_done = false;

	SDL_ShowOpenFolderDialog(folder_select_callback, nullptr, SDL_GetKeyboardFocus(), default_location.c_str(), false);

	// Wait for the dialog to finish
	while (!dialog_done) {
		SDL_PumpEvents();  // Process pending events
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	// Return the selected folder or an empty string if none was selected
	return selected_files.empty() ? "" : selected_files.front();
}


namespace fs = std::filesystem;

static CDateTime convert_time_point_to_CDateTime(const std::chrono::system_clock::time_point& tp) {
	auto time = std::chrono::system_clock::to_time_t(tp);
	struct tm* tm_info = localtime(&time);

	return CDateTime(
		tm_info->tm_year + 1900,
		tm_info->tm_mon + 1,
		tm_info->tm_mday,
		tm_info->tm_hour,
		tm_info->tm_min,
		tm_info->tm_sec
	);
}

static bool FNMatch(const std::string& file, const std::string& pattern) {
	try {
		return Poco::Glob(pattern).match(file);
	}
	catch (...) { return false; }
}

static void list_directory_helper(const std::string& path, std::vector<std::string>& files, std::vector<std::string>& folders, const std::string& file_pattern = "*", const std::string& folder_pattern = "*") {
	if (fs::is_directory(path)) {
		for (const auto& entry : fs::directory_iterator(path)) {
			std::string entryPath = entry.path().string();
			if (entry.is_directory() && FNMatch(entryPath, folder_pattern)) {
				folders.push_back(entryPath);
			}
			else {
				if (FNMatch(entry.path().filename().string(), file_pattern)) {
					files.push_back(entryPath);
				}
			}
		}
	}
}



bool exists(const std::string& path) {
	return fs::exists(path);
}

bool is_directory(const std::string& path) {
	return fs::is_directory(path);
}

std::string get_current_path() {
	return fs::current_path().string();
}

bool set_current_path(const std::string& path) {
	try
	{
		fs::current_path(path);
		return true;
	}
	catch (const fs::filesystem_error& e) {
		return false;
	}
	return false;
}

void list_directory(const std::string& path = get_current_path(), CScriptArray* files = nullptr, CScriptArray* folders = nullptr, const std::string& file_pattern = "*", const std::string& folder_pattern = "*") {
	std::vector<std::string> files_array;
	std::vector<std::string> folders_array;
	list_directory_helper(path, files_array, folders_array, file_pattern, folder_pattern);
	if (files)files->Reserve(files_array.size());
	if (folders)folders->Reserve(folders_array.size());
	for (uint64_t i = 0; files && i < files_array.size(); ++i) {
		files->InsertAt(i, &files_array[i]);
	}
	for (uint64_t i = 0; folders && i < folders_array.size(); ++i) {
		folders->InsertAt(i, &files_array[i]);
	}
}

bool create_directory(const std::string& path) {
	return fs::create_directory(path);
}

bool ScriptRemove(const std::string& path) {
	return fs::remove(path);
}

bool ScriptRename(const std::string& oldPath, const std::string& newPath) {
	try {
		fs::rename(oldPath, newPath);
		return true;
	}
	catch (...) {
		return false;
	}
}

std::string get_file_extension(const std::string& path) {
	return fs::path(path).extension().string();
}

bool copy(const std::string& source, const std::string& destination) {
	try {
		fs::copy(source, destination);
		return true;
	}
	catch (...) {
		return false;
	}
}

bool move(const std::string& source, const std::string& destination) {
	try {
		fs::rename(source, destination); // Move is essentially a rename
		return true;
	}
	catch (...) {
		return false;
	}
}


uint64_t get_file_size(const std::string& path) {
	try {
		return fs::file_size(path);
	}
	catch (const fs::filesystem_error& e) {
		return 0;
	}
	return 0;
}

CDateTime get_last_modified_time(const std::string& path) {
	try {
		auto lastWriteTime = fs::last_write_time(path);
		auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
			lastWriteTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
		);

		return convert_time_point_to_CDateTime(sctp);
	}
	catch (const fs::filesystem_error& e) {
		return CDateTime(); // Return default CDateTime on error
	}
	return CDateTime();
}


void RegisterScriptFileSystem(asIScriptEngine* engine) {
	// First register dialogs
	const char* ns = engine->GetDefaultNamespace();
	engine->SetDefaultNamespace("");
	engine->RegisterGlobalFunction(_O("string open_file_dialog(const string &in filters = \"\", const string&in default_location = \"\")"), asFUNCTION(simple_file_open_dialog), asCALL_CDECL);
	engine->RegisterGlobalFunction(_O("string save_file_dialog(const string &in filters = \"\", const string&in default_location = \"\")"), asFUNCTION(simple_file_save_dialog), asCALL_CDECL);
	engine->RegisterGlobalFunction(_O("string select_folder_dialog(const string&in default_location = \"\")"), asFUNCTION(simple_folder_select_dialog), asCALL_CDECL);
	engine->SetDefaultNamespace(ns);

	engine->RegisterGlobalFunction("bool exists(const string &in path)", asFUNCTION(exists), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool is_directory(const string &in path)", asFUNCTION(is_directory), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_current_path() property", asFUNCTION(get_current_path), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_current_path(const string &in path) property", asFUNCTION(set_current_path), asCALL_CDECL);
	engine->RegisterGlobalFunction("void list_directory(const string &in path = current_path, array<string>@ files = null, array<string>@ folders = null, const string &in file_pattern = '*', const string &in folder_pattern = '*')", asFUNCTION(list_directory), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool create_directory(const string &in path)", asFUNCTION(create_directory), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool remove(const string &in path)", asFUNCTION(ScriptRemove), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool rename(const string &in old, const string &in new)", asFUNCTION(ScriptRename), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_file_extension(const string &in path)", asFUNCTION(get_file_extension), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool copy(const string &in source, const string &in des)", asFUNCTION(copy), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool move(const string &in source, const string &in des)", asFUNCTION(move), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_file_size(const string &in path)", asFUNCTION(get_file_size), asCALL_CDECL);
	engine->RegisterGlobalFunction("datetime get_last_modified_time(const string &in path)", asFUNCTION(get_last_modified_time), asCALL_CDECL);
}

