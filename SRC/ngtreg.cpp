#include "autowrapper/aswrappedcall.h"
#include "filesystem.h"
#include "Graphics.h"
#include "IOStreams.h"
#include "MemoryStream.h"
#include "ngtreg.h"
#include "obfuscate.h"
#include "print_func/print_func.h"
#include "scriptbuilder/scriptbuilder.h"
#include "Scripting.h"
#include "scriptstdstring/scriptstdstring.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <Poco/UnicodeConverter.h>
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include <windows.h>
#endif
#include <assert.h>  // assert()
#include "angelscript.h"
#include "ngt.h"
#include "pack.h"
#include "sound.h"
#ifdef _WIN32
HWND g_hwndEdit;
HWND hwnd;
HWND buttonc;
WNDPROC originalEditProc;
#endif

template <class T>
void ObjectAddRef(T* ptr) {
	RefObject* o = RefObjectGet(ptr);
	if (o) asAtomicInc(o->refcount);
}

template <class T>
void ObjectRelease(T* ptr) {
	RefObject* o = RefObjectGet(ptr);
	if (!o)return;
	asAtomicDec(o->refcount);
	if (o->refcount < 1) {
		ptr->~T();
		free(o);
	}
}

template <class T, typename... A> T* ObjectFactory(A... args) { return new (ObgectCreate<T>()) T(args...); }

const int EVENT_NONE = ENET_EVENT_TYPE_NONE;
const int EVENT_CONNECT = ENET_EVENT_TYPE_CONNECT;
const int EVENT_RECEIVE = ENET_EVENT_TYPE_RECEIVE;
const int EVENT_DISCONNECT = ENET_EVENT_TYPE_DISCONNECT;
using namespace std;
#ifdef _WIN32
std::string convertNewlines(const std::string& input) {
	std::string output;
	for (char c : input) {
		if (c == '\n') {
			output += "\r\n"; // Add \r\n for each \n found
		}
		else {
			output += c; // Add the current character
		}
	}
	return output;
}



LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_KEYDOWN:
		if (GetKeyState(VK_CONTROL) & 0x8000 && wParam == 'A') {
			SendMessage(hwnd, EM_SETSEL, 0, -1);
		}
		if (wParam == VK_TAB)
		{
			SetFocus(buttonc); // Set focus to the next control
		}
		else if (wParam == VK_ESCAPE or (wParam == VK_RETURN) and (GetFocus() == g_hwndEdit))
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	}
	return CallWindowProc(originalEditProc, hWnd, uMsg, wParam, lParam);
}
int currentLine;
int currentLineUp;
std::wstring result_message;
int nCmdShow;
HINSTANCE hInstance;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
HWND g_CurrentFocus;
#endif
void show_message(bool include_info, bool include_warning, bool include_error)
{
	std::string title;
	std::string messages;
	if (!include_error && !include_warning) {
		title = "Message";
	}
	else if (include_warning && !include_error) {
		title = "Warning";
	}
	else {
		title = "Error";
	}

	if (include_info) {
		messages += g_ScriptMessagesInfo;
	}
	if (include_error) {
		messages += g_ScriptMessagesError;
	}

	if (include_warning) {
		messages += g_ScriptMessagesWarning;
	}
	cout << title << endl << messages;
#if defined(_WIN32)
	if (GetConsoleWindow() != 0)return;
	wstring title_u;
	title_u = wstr(title);
	result_message = wstr(convertNewlines(messages));

	const wchar_t CLASS_NAME[] = L"NGTTextbox";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		title_u.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
	{
		return;
	}

	buttonc = CreateWindow(
		L"BUTTON",
		L"&Close",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
		10, 220, 100, 30,
		hwnd,
		(HMENU)1,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);
	SetFocus(g_hwndEdit);
	originalEditProc = (WNDPROC)SetWindowLongPtr(g_hwndEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

	MSG messagege;
	while (GetMessage(&messagege, NULL, 0, 0))
	{
		if (!IsDialogMessage(hwnd, &messagege))
		{
			TranslateMessage(&messagege);
			DispatchMessage(&messagege);
			if (GetForegroundWindow() == hwnd)
				g_CurrentFocus = GetFocus();
		}

	}
#else 
	return;// Shutting down
#endif
	g_ScriptMessagesError = "";
	g_ScriptMessagesWarning = "";
	g_ScriptMessagesInfo = "";
}
#ifdef _WIN32
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static int lineCount = 0;

	switch (msg)
	{
	case WM_SETFOCUS: {
		SetFocus(g_CurrentFocus);
		break;
	}
	case WM_CREATE:
		g_hwndEdit = CreateWindowEx(
			0, L"EDIT",
			L"",
			WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_TABSTOP | ES_AUTOVSCROLL | WS_VSCROLL | WS_HSCROLL,
			10, 10, 400, 200,
			hwnd,
			NULL,
			hInstance,
			NULL
		);
		SendMessage(g_hwndEdit, WM_SETTEXT, false, (LPARAM)result_message.c_str());

		break;


	case WM_COMMAND:
		if (LOWORD(wParam) == 1)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);

}
#endif
timer* ftimer() { return new timer; }
user_idle* fuser_idle() { return new user_idle; }
library* flibrary() { return new library; }
void fscript_thread(asIScriptGeneric* gen) {
	asIScriptFunction* func = (asIScriptFunction*)gen->GetArgAddress(0);
	CScriptDictionary* args = (CScriptDictionary*)gen->GetArgAddress(1);
	script_thread* th = new script_thread(func, args);
	gen->SetReturnObject(th);
}

TTSVoice* ftts_voice() { return new TTSVoice(); }
network_event* fnetwork_event() { return new network_event; }
ngtvector* fngtvector() { return new ngtvector; }
sqlite3statement* fsqlite3statement() { return new sqlite3statement; }
ngtsqlite3* fngtsqlite3() { return new ngtsqlite3; }
network* fnetwork() { return new network; }

template <class T> void RegisterMutex(asIScriptEngine* engine, const std::string& type) {
	RegisterObject<T>(engine, type.c_str());
	if constexpr (!std::is_same<T, NamedMutex>::value) {
		engine->RegisterObjectMethod(type.c_str(), _O("void lock(int)"), asMETHODPR(T, lock, (long), void), asCALL_THISCALL);
		engine->RegisterObjectMethod(type.c_str(), _O("bool try_lock(int)"), asMETHODPR(T, tryLock, (long), bool), asCALL_THISCALL);
	}
	engine->RegisterObjectMethod(type.c_str(), _O("void lock()"), asMETHODPR(T, lock, (), void), asCALL_THISCALL);
	engine->RegisterObjectMethod(type.c_str(), _O("bool try_lock()"), asMETHODPR(T, tryLock, (), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod(type.c_str(), _O("void unlock()"), asMETHOD(T, unlock), asCALL_THISCALL);
}




std::string g_ScriptMessagesInfo;
std::string g_ScriptMessagesWarning;
std::string g_ScriptMessagesError;

void MessageCallback(const asSMessageInfo* msg, void* param)
{
	std::string messageTemplate = "File: %f\r\nLine: %l, Col: %c\r\n%t: %m\r\n\r\n";

	std::string formattedMessage = messageTemplate;
	formattedMessage.replace(formattedMessage.find("%f"), 2, msg->section);
	formattedMessage.replace(formattedMessage.find("%l"), 2, std::to_string(msg->row));
	formattedMessage.replace(formattedMessage.find("%c"), 2, std::to_string(msg->col));
	formattedMessage.replace(formattedMessage.find("%t"), 2,
		(msg->type == asMSGTYPE_WARNING ? "WARNING" :
			(msg->type == asMSGTYPE_INFORMATION ? "INFO" : "ERR")));


	formattedMessage.replace(formattedMessage.find("%m"), 2, msg->message);

	if (msg->type == asMSGTYPE_WARNING) {
		g_ScriptMessagesWarning += formattedMessage;
	}
	else if (msg->type == asMSGTYPE_INFORMATION) {
		g_ScriptMessagesInfo += formattedMessage;
	}
	else {
		g_ScriptMessagesError += formattedMessage;
	}
}

SDL_Rect* frect() { return new SDL_Rect; }
// One line sizeof
void as_sizeof(asIScriptGeneric* gen) {
	gen->SetReturnQWord(gen->GetEngine()->GetSizeOfPrimitiveType(gen->GetArgTypeId(0)));
}

void RegisterFunctions(asIScriptEngine* engine)
{
	engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
	engine->RegisterTypedef("long", "int64");
	engine->RegisterTypedef("ulong", "uint64");
	engine->RegisterTypedef("char", "int8");
	engine->RegisterTypedef("uchar", "uint8");
	engine->RegisterTypedef("short", "int16");
	engine->RegisterTypedef("ushort", "uint16");
	engine->RegisterTypedef("size_t", "uint64");
	engine->RegisterFuncdef("int exit_callback()");
	engine->RegisterObjectType("vector", sizeof(ngtvector), asOBJ_REF);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_FACTORY, "vector@ v()", asFUNCTION(fngtvector), asCALL_CDECL);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_ADDREF, "void f()", asMETHOD(ngtvector, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_RELEASE, "void f()", asMETHOD(ngtvector, release), asCALL_THISCALL);
	engine->RegisterObjectProperty(_O("vector"), "float x", asOFFSET(ngtvector, x));
	engine->RegisterObjectProperty(_O("vector"), "float y", asOFFSET(ngtvector, y));
	engine->RegisterObjectProperty(_O("vector"), "float z", asOFFSET(ngtvector, z));
	engine->RegisterObjectMethod(_O("vector"), "float get_length()const property", asMETHOD(ngtvector, get_length), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("vector"), "vector &opAssign(const vector&in)", asMETHOD(ngtvector, operator=), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("vector"), "void reset()const", asMETHOD(ngtvector, reset), asCALL_THISCALL);
	Print::asRegister(engine, true);
	AS_BEGIN(engine, "os");
	engine->RegisterGlobalFunction("int get_cpu_count()property", asFUNCTION(get_cpu_count), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_system_ram()property", asFUNCTION(get_system_ram), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_platform()property", asFUNCTION(get_platform), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("uint64 get_time_stamp_millis()property", asFUNCTION(get_time_stamp_millis), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_time_stamp_seconds()property", asFUNCTION(get_time_stamp_seconds), asCALL_CDECL);

	engine->RegisterGlobalFunction("long random(long min, long max)", asFUNCTIONPR(random, (int64_t, int64_t), int64_t), asCALL_CDECL);
	engine->RegisterGlobalFunction("double random(double min, double max)", asFUNCTIONPR(randomDouble, (double, double), double), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool random_bool()", asFUNCTION(random_bool), asCALL_CDECL);

	engine->RegisterGlobalFunction("int get_last_error()property", asFUNCTION(get_last_error), asCALL_CDECL);
	AS_BEGIN(engine, "screen_reader");
	engine->RegisterGlobalFunction("void set_interrupt(bool)property", asFUNCTION(set_screen_reader_interrupt), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_interrupt()property", asFUNCTION(get_screen_reader_interrupt), asCALL_CDECL);

	engine->RegisterGlobalFunction("bool speak(const string &in text, bool interrupt = interrupt)", asFUNCTION(speak), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool braille(const string &in text)", asFUNCTION(braille), asCALL_CDECL);

	engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
	engine->RegisterGlobalFunction("string detect()", asFUNCTION(screen_reader_detect), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("void set_exit_callback(exit_callback@=null)", asFUNCTION(set_exit_callback), asCALL_CDECL);

	engine->RegisterGlobalFunction("bool show_window(const string &in title, int width = 640, int height = 480, bool enable_renderer = false)", asFUNCTION(show_window), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_window_handle() property", asFUNCTION(get_window_handle), asCALL_CDECL);
	engine->RegisterGlobalFunction("void hide_window()", asFUNCTION(hide_window), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_update_window_freq(long)property", asFUNCTION(set_update_window_freq), asCALL_CDECL);
	engine->RegisterGlobalFunction("long get_update_window_freq()property", asFUNCTION(get_update_window_freq), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_window_title(const string &in)property", asFUNCTION(set_window_title), asCALL_CDECL);
	engine->RegisterGlobalFunction("void window_present()", asFUNCTION(window_present), asCALL_CDECL);

	engine->RegisterGlobalFunction("void garbage_collect()", asFUNCTION(garbage_collect), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_window_active()property", asFUNCTION(get_window_active), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_window_fullscreen(bool)property", asFUNCTION(set_window_fullscreen), asCALL_CDECL);

	engine->RegisterGlobalFunction("bool mouse_pressed(uint8 button)", asFUNCTION(mouse_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool mouse_released(uint8 button)", asFUNCTION(mouse_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool mouse_down(uint8 button)", asFUNCTION(mouse_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_X()property", asFUNCTION(get_MOUSE_X), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_Y()property", asFUNCTION(get_MOUSE_Y), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_Z()property", asFUNCTION(get_MOUSE_Z), asCALL_CDECL);
	AS_BEGIN(engine, "internet");
	engine->RegisterGlobalFunction("void ftp_download(const string& in url, const string &in filename)", asFUNCTION(ftp_download), asCALL_CDECL);
	engine->RegisterEnum("smtp_login_method");
	engine->RegisterEnumValue("smtp_login_method", "AUTH_NONE", Poco::Net::SMTPClientSession::AUTH_NONE);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_CRAM_MD5", Poco::Net::SMTPClientSession::AUTH_CRAM_MD5);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_CRAM_SHA1", Poco::Net::SMTPClientSession::AUTH_CRAM_SHA1);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_LOGIN", Poco::Net::SMTPClientSession::AUTH_LOGIN);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_PLAIN", Poco::Net::SMTPClientSession::AUTH_PLAIN);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_XOAUTH2", Poco::Net::SMTPClientSession::AUTH_XOAUTH2);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_NTLM", Poco::Net::SMTPClientSession::AUTH_NTLM);
	engine->RegisterGlobalFunction("void mail_send(smtp_login_method, uint port, const string&in username, const string&in password, const string&in mailhost, const string&in sender, const string&in recipient, const string&in subject, const string&in content, const string&in attachment = \"\")", WRAP_FN(mail_send), asCALL_GENERIC);
	AS_END(engine);
	engine->RegisterGlobalFunction("void exit(int retcode = 0)", asFUNCTION(exit_engine), asCALL_CDECL);
	engine->RegisterGlobalFunction("string number_to_words(uint64 number, bool include_and)", asFUNCTION(number_to_words), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool clipboard_copy_text(const string &in text)", asFUNCTION(clipboard_copy_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string clipboard_read_text()", asFUNCTION(clipboard_read_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_input()", asFUNCTION(get_input), asCALL_CDECL);
	engine->RegisterGlobalFunction("string input_box(const string &in title, const string &in text, const string &in default_text = \"\", bool secure = false, bool multiline = false)", asFUNCTION(input_box), asCALL_CDECL);

	engine->RegisterEnum("keycode");
	engine->RegisterGlobalFunction("bool key_pressed(keycode)", asFUNCTION(key_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_released(keycode)", asFUNCTION(key_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_down(keycode)", asFUNCTION(key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_repeat(keycode)", asFUNCTION(key_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_quit_requested() property", asFUNCTION(quit_requested), asCALL_CDECL);

	engine->RegisterGlobalFunction("int[]@ keys_pressed()", asFUNCTION(keys_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_released()", asFUNCTION(keys_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_down()", asFUNCTION(keys_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_repeat()", asFUNCTION(keys_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("string key_to_string(int key_code)", WRAP_FN(key_to_string), asCALL_GENERIC);
	engine->RegisterGlobalFunction("int string_to_key(const string&in key_name)", WRAP_FN(string_to_key), asCALL_GENERIC);
	engine->RegisterGlobalFunction("bool force_key_down(keycode)", asFUNCTION(force_key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool force_key_up(keycode)", asFUNCTION(force_key_up), asCALL_CDECL);
	engine->RegisterGlobalFunction("void reset_keyboard()", asFUNCTION(reset_keyboard), asCALL_CDECL);

	engine->RegisterGlobalFunction("string string_encrypt(const string&in the_string, string key)", asFUNCTION(string_encrypt), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_decrypt(const string&in the_string, string key)", asFUNCTION(string_decrypt), asCALL_CDECL);
	AS_BEGIN(engine, "internet");
	engine->RegisterGlobalFunction("string url_decode(const string &in url)", asFUNCTION(url_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_encode(const string &in url)", asFUNCTION(url_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_get(const string &in url)", asFUNCTION(url_get), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_post(const string &in url, const string &in parameters)", asFUNCTION(url_post), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("string ascii_to_character(int ascii)", asFUNCTION(ascii_to_character), asCALL_CDECL);
	engine->RegisterGlobalFunction("int character_to_ascii(const string      &in character)", asFUNCTION(character_to_ascii), asCALL_CDECL);
	engine->RegisterGlobalFunction("string hex_to_string(const string&in the_hexadecimal_sequence)", asFUNCTION(hex_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("string number_to_hex_string(int64 the_number)", asFUNCTION(number_to_hex_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base64_decode(const string&in base64_encoded_string)", asFUNCTION(string_base64_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base64_encode(const string&in the_string)", asFUNCTION(string_base64_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base32_decode(const string&in base32_encoded_string)", asFUNCTION(string_base32_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base32_encode(const string&in the_string)", asFUNCTION(string_base32_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_to_hex(const string&in the_string)", asFUNCTION(string_to_hex), asCALL_CDECL);
	engine->RegisterEnum(_O("message_box_flags"));
	engine->RegisterEnumValue(_O("message_box_flags"), _O("MESSAGE_BOX_ERROR"), SDL_MESSAGEBOX_ERROR);
	engine->RegisterEnumValue(_O("message_box_flags"), _O("MESSAGE_BOX_WARNING"), SDL_MESSAGEBOX_WARNING);
	engine->RegisterEnumValue(_O("message_box_flags"), _O("MESSAGE_BOX_INFORMATION"), SDL_MESSAGEBOX_INFORMATION);
	engine->RegisterEnumValue(_O("message_box_flags"), _O("MESSAGE_BOX_BUTTONS_LEFT_TO_RIGHT"), SDL_MESSAGEBOX_BUTTONS_LEFT_TO_RIGHT);
	engine->RegisterEnumValue(_O("message_box_flags"), _O("MESSAGE_BOX_BUTTONS_RIGHT_TO_LEFT"), SDL_MESSAGEBOX_BUTTONS_RIGHT_TO_LEFT);
	engine->RegisterGlobalFunction("int message_box(const string& in title, const string& in text, array<string>@ buttons, uint flags = 0)", asFUNCTION(message_box_script), asCALL_CDECL);

	engine->RegisterGlobalFunction("bool alert(const string &in title, const string &in text, const string &in button_name = \"OK\")", asFUNCTION(alert), asCALL_CDECL);
	engine->RegisterGlobalFunction("int question(const string &in title, const string &in text)", asFUNCTION(question), asCALL_CDECL);

	engine->RegisterGlobalFunction("void wait(uint64 ms)", asFUNCTION(wait), asCALL_CDECL);
	engine->RegisterGlobalFunction("void wait_event()", asFUNCTION(wait_event), asCALL_CDECL);

	engine->RegisterGlobalFunction("string read_environment_variable(const string&in var_name)", asFUNCTION(read_environment_variable), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool write_environment_variable(const string&in var_name, const string &in value)", asFUNCTION(write_environment_variable), asCALL_CDECL);

	engine->RegisterGlobalFunction("string serialize(dictionary@ handle = null)", asFUNCTION(serialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("dictionary@ deserialize(const string &in the_serialized_dictionary)", asFUNCTION(deserialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("string serialize_array(?&in array)", asFUNCTION(serialize_array), asCALL_GENERIC);
	engine->RegisterGlobalFunction("bool deserialize_array(const string &in the_serialized_array, ?&out array)", asFUNCTION(deserialize_array), asCALL_GENERIC);

	engine->RegisterGlobalFunction("bool urlopen(const string &in url)", asFUNCTION(urlopen), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 malloc(size_t alloc_size)", asFUNCTION(malloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 calloc(uint64, uint64)", asFUNCTION(calloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 realloc(uint64, uint64)", asFUNCTION(realloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("void free(uint64)", asFUNCTION(free), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 memcpy(uint64, uint64, size_t)", asFUNCTION(memcpy), asCALL_CDECL);
	engine->RegisterGlobalFunction("string c_str_to_string(uint64 array_pointer, size_t string_length = 0)", asFUNCTION(c_str_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("wstring wc_str_to_wstring(uint64 array_pointer, size_t string_length = 0)", asFUNCTION(wc_str_to_wstring), asCALL_CDECL);

	engine->RegisterGlobalFunction("size_t c_str_len(uint64 array_pointer)", asFUNCTION(strlen), asCALL_CDECL);
	engine->RegisterGlobalFunction("size_t wc_str_len(uint64 array_pointer)", asFUNCTION(wcslen), asCALL_CDECL);
	engine->RegisterGlobalFunction("void unicode_convert(const string &in utf8_string, wstring &out wide_string)", asFUNCTIONPR(Poco::UnicodeConverter::convert, (const std::string&, std::wstring&), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void unicode_convert(const wstring &in wide_string, string &out utf8_string)", asFUNCTIONPR(Poco::UnicodeConverter::convert, (const std::wstring&, std::string&), void), asCALL_CDECL);
	register_pack(engine);
	register_sound(engine);
	RegisterScriptGraphics(engine);
	RegisterScriptIOStreams(engine);
	RegisterMemstream(engine);
	AS_BEGIN(engine, "scripting");
	RegisterScripting(engine);
	AS_END(engine);
	engine->RegisterObjectType("tts_voice", sizeof(TTSVoice), asOBJ_REF);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_FACTORY, "tts_voice@ v()", asFUNCTION(ftts_voice), asCALL_CDECL);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_ADDREF, "void f()", asMETHOD(TTSVoice, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_RELEASE, "void f()", asMETHOD(TTSVoice, release), asCALL_THISCALL);


	engine->RegisterObjectMethod(_O("tts_voice"), "void speak(const string& in text)const", asMETHOD(TTSVoice, speak), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void speak_wait(const string& in text)const", asMETHOD(TTSVoice, speak_wait), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void speak_interrupt(const string& in text)const", asMETHOD(TTSVoice, speak_interrupt), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void speak_interrupt_wait(const string& in text)const", asMETHOD(TTSVoice, speak_interrupt_wait), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "string speak_to_memory(const string& in text, size_t&out buffer_size)const", asMETHOD(TTSVoice, speak_to_memory), asCALL_THISCALL);

	engine->RegisterObjectMethod(_O("tts_voice"), "int get_rate()const property", asMETHOD(TTSVoice, get_rate), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void set_rate(int)property", asMETHOD(TTSVoice, set_rate), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "int get_volume()const property", asMETHOD(TTSVoice, get_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void set_volume(int)property", asMETHOD(TTSVoice, set_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "void set_voice(uint64)property", asMETHOD(TTSVoice, set_voice), asCALL_THISCALL);
	engine->RegisterObjectMethod(_O("tts_voice"), "array<string>@ get_voice_names()const property", asMETHOD(TTSVoice, get_voice_names), asCALL_THISCALL);

	engine->RegisterObjectType("user_idle", sizeof(user_idle), asOBJ_REF);
	engine->RegisterObjectBehaviour("user_idle", asBEHAVE_FACTORY, "user_idle@ u()", asFUNCTION(fuser_idle), asCALL_CDECL);
	engine->RegisterObjectBehaviour("user_idle", asBEHAVE_ADDREF, "void f()", asMETHOD(user_idle, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("user_idle", asBEHAVE_RELEASE, "void f()", asMETHOD(user_idle, release), asCALL_THISCALL);

	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_millis()const property", asMETHOD(user_idle, elapsed_millis), asCALL_THISCALL);
	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_seconds()const property", asMETHOD(user_idle, elapsed_seconds), asCALL_THISCALL);
	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_minutes()const property", asMETHOD(user_idle, elapsed_minutes), asCALL_THISCALL);
	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_hours()const property", asMETHOD(user_idle, elapsed_hours), asCALL_THISCALL);
	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_days()const property", asMETHOD(user_idle, elapsed_days), asCALL_THISCALL);
	engine->RegisterObjectMethod("user_idle", "uint64 get_elapsed_weeks()const property", asMETHOD(user_idle, elapsed_weeks), asCALL_THISCALL);

	engine->RegisterObjectType("timer", sizeof(timer), asOBJ_REF);
	engine->RegisterObjectBehaviour("timer", asBEHAVE_FACTORY, "timer@ t()", asFUNCTION(ftimer), asCALL_CDECL);
	engine->RegisterObjectBehaviour("timer", asBEHAVE_ADDREF, "void f()", asMETHOD(timer, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("timer", asBEHAVE_RELEASE, "void f()", asMETHOD(timer, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_seconds()const property", asMETHOD(timer, elapsed_seconds), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_minutes()const property", asMETHOD(timer, elapsed_minutes), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_hours()const property", asMETHOD(timer, elapsed_hours), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_millis()const property", asMETHOD(timer, elapsed_millis), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_micros()const property", asMETHOD(timer, elapsed_micros), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "uint64 get_elapsed_nanos()const property", asMETHOD(timer, elapsed_nanos), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_seconds(uint64)const", asMETHOD(timer, force_seconds), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_minutes(uint64)const", asMETHOD(timer, force_minutes), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_hours(uint64)const", asMETHOD(timer, force_hours), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_millis(uint64)const", asMETHOD(timer, force_millis), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_micros(uint64)const", asMETHOD(timer, force_micros), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void force_nanos(uint64)const", asMETHOD(timer, force_nanos), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void restart()const", asMETHOD(timer, restart), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void pause()const", asMETHOD(timer, pause), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "void resume()const", asMETHOD(timer, resume), asCALL_THISCALL);
	engine->RegisterObjectMethod("timer", "bool get_running()const property", asMETHOD(timer, is_running), asCALL_THISCALL);

	engine->RegisterObjectType("library", sizeof(library), asOBJ_REF);
	engine->RegisterObjectBehaviour("library", asBEHAVE_FACTORY, "library@ l()", asFUNCTION(flibrary), asCALL_CDECL);
	engine->RegisterObjectBehaviour("library", asBEHAVE_ADDREF, "void f()", asMETHOD(library, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("library", asBEHAVE_RELEASE, "void f()", asMETHOD(library, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "bool load(const string&in library_path)const", asMETHOD(library, load), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "bool get_active()const property", asMETHOD(library, active), asCALL_THISCALL);

	engine->RegisterObjectMethod("library", "void unload()const", asMETHOD(library, unload), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "uint64 get_function_pointer(const string &in function_name)const", asMETHOD(library, get_function_pointer), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "void clear_functions()const", asMETHOD(library, clear_functions), asCALL_THISCALL);

	engine->RegisterFuncdef("void thread_func(dictionary@ args = null)");
	engine->RegisterObjectType("thread", sizeof(script_thread), asOBJ_REF);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_FACTORY, "thread@ t(thread_func@ factory, dictionary@ args = null)", asFUNCTION(fscript_thread), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_ADDREF, "void f()", asMETHOD(script_thread, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_RELEASE, "void f()", asMETHOD(script_thread, release), asCALL_THISCALL);

	engine->RegisterObjectMethod("thread", "void detach()const", asMETHOD(script_thread, detach), asCALL_THISCALL);
	engine->RegisterObjectMethod("thread", "void join()const", asMETHOD(script_thread, join), asCALL_THISCALL);
	engine->RegisterObjectMethod("thread", "void destroy()const", asMETHOD(script_thread, destroy), asCALL_THISCALL);
	AS_BEGIN(engine, "this_thread");
	engine->RegisterGlobalFunction("void yield()", asFUNCTION(std::this_thread::yield), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_id() property", asFUNCTION(std::this_thread::get_id), asCALL_CDECL);
	AS_END(engine);

	AS_BEGIN(engine, "filesystem");
	RegisterScriptFileSystem(engine);
	AS_END(engine);
	engine->RegisterGlobalProperty("const int EVENT_NONE", (void*)&EVENT_NONE);
	engine->RegisterGlobalProperty("const int EVENT_CONNECT", (void*)&EVENT_CONNECT);
	engine->RegisterGlobalProperty("const int EVENT_RECEIVE", (void*)&EVENT_RECEIVE);
	engine->RegisterGlobalProperty("const int EVENT_DISCONNECT", (void*)&EVENT_DISCONNECT);

	engine->RegisterObjectType("network_event", sizeof(network_event), asOBJ_REF);
	engine->RegisterObjectBehaviour("network_event", asBEHAVE_FACTORY, "network_event@ e()", asFUNCTION(fnetwork_event), asCALL_CDECL);
	engine->RegisterObjectBehaviour("network_event", asBEHAVE_ADDREF, "void f()", asMETHOD(network_event, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("network_event", asBEHAVE_RELEASE, "void f()", asMETHOD(network_event, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("network_event", "network_event &opAssign(const network_event&in)", asMETHOD(network_event, operator=), asCALL_THISCALL);

	engine->RegisterObjectProperty("network_event", "const int type", asOFFSET(network_event, m_type));
	engine->RegisterObjectProperty("network_event", "const uint64 peer_id", asOFFSET(network_event, m_peerId));
	engine->RegisterObjectProperty("network_event", "const int channel", asOFFSET(network_event, m_channel));
	engine->RegisterObjectProperty("network_event", "const string message", asOFFSET(network_event, m_message));


	engine->RegisterObjectType("network", sizeof(network), asOBJ_REF);
	engine->RegisterObjectBehaviour("network", asBEHAVE_FACTORY, "network@ n()", asFUNCTION(fnetwork), asCALL_CDECL);
	engine->RegisterObjectBehaviour("network", asBEHAVE_ADDREF, "void f()", asMETHOD(network, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("network", asBEHAVE_RELEASE, "void f()", asMETHOD(network, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "uint64 connect(string&in host, int port)", asMETHOD(network, connect), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool destroy()", asMETHOD(network, destroy), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool disconnect_peer(uint64 peer_id)", asMETHOD(network, disconnect_peer), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool disconnect_peer_forcefully(uint64 peer_id)", asMETHOD(network, disconnect_peer_forcefully), asCALL_THISCALL);
	//    engine->RegisterObjectMethod("network", "bool disconnect_peer_softly(uint)", asMETHOD(network, disconnect_peer_softly), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "string get_peer_address(uint64 peer_id)const property", asMETHOD(network, get_peer_address), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "double get_peer_average_round_trip_time(uint64 peer_id)const property", asMETHOD(network, get_peer_average_round_trip_time), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "uint64[]@ get_peer_list()", asMETHOD(network, get_peer_list), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "network_event@ request(int timeout = 0)", asMETHOD(network, request), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool send_reliable(uint64 peer_id, string&in packet, int channel)", asMETHOD(network, send_reliable), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool send_unreliable(uint64 peer_id, string&in packet, int channel)", asMETHOD(network, send_unreliable), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth)", asMETHOD(network, set_bandwidth_limits), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool setup_client(int channels, int64 max_peers)", asMETHOD(network, setup_client), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool setup_server(int port, int channels, int64 max_peers)", asMETHOD(network, setup_server), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "void flush()const", asMETHOD(network, flush), asCALL_THISCALL);

	engine->RegisterObjectMethod("network", "int64 get_connected_peers() const property", asMETHOD(network, get_connected_peers), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "double get_bytes_sent() const property", asMETHOD(network, get_bytes_sent), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "double get_bytes_received() const property", asMETHOD(network, get_bytes_received), asCALL_THISCALL);
	engine->RegisterFuncdef("int sqlite3authorizer(string, int, string, string, string, string)");
	engine->RegisterObjectType("sqlite3statement", sizeof(sqlite3statement), asOBJ_REF);
	engine->RegisterObjectBehaviour("sqlite3statement", asBEHAVE_FACTORY, "sqlite3statement@ s()", asFUNCTION(fsqlite3statement), asCALL_CDECL);
	engine->RegisterObjectBehaviour("sqlite3statement", asBEHAVE_ADDREF, "void f()", asMETHOD(sqlite3statement, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("sqlite3statement", asBEHAVE_RELEASE, "void f()", asMETHOD(sqlite3statement, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int step()", asMETHOD(sqlite3statement, step), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int reset()", asMETHOD(sqlite3statement, reset), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string get_expanded_sql_statement() property", asMETHOD(sqlite3statement, get_expanded_sql_statement), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string get_sql_statement() property", asMETHOD(sqlite3statement, get_sql_statement), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int get_bind_param_count() property", asMETHOD(sqlite3statement, get_bind_param_count), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int get_column_count() property", asMETHOD(sqlite3statement, get_column_count), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_blob(int, const string&in, bool=true)", asMETHOD(sqlite3statement, bind_blob), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_double(int, double)", asMETHOD(sqlite3statement, bind_double), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_int(int, int)", asMETHOD(sqlite3statement, bind_int), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_int64(int, int64)", asMETHOD(sqlite3statement, bind_int64), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_null(int)", asMETHOD(sqlite3statement, bind_null), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_param_index(const string&in)", asMETHOD(sqlite3statement, bind_param_index), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string bind_param_name(int)", asMETHOD(sqlite3statement, bind_param_name), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int bind_text(int, const string&in, bool=true)", asMETHOD(sqlite3statement, bind_text), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int clear_bindings()", asMETHOD(sqlite3statement, clear_bindings), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string column_blob(int)", asMETHOD(sqlite3statement, column_blob), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int column_bytes(int)", asMETHOD(sqlite3statement, column_bytes), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string column_decltype(int)", asMETHOD(sqlite3statement, column_decltype), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "double column_double(int)", asMETHOD(sqlite3statement, column_double), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int column_int(int)", asMETHOD(sqlite3statement, column_int), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int64 column_int64(int)", asMETHOD(sqlite3statement, column_int64), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string column_name(int)", asMETHOD(sqlite3statement, column_name), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "int column_type(int)", asMETHOD(sqlite3statement, column_type), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3statement", "string column_text(int)", asMETHOD(sqlite3statement, column_text), asCALL_THISCALL);
	engine->RegisterObjectType("sqlite3", sizeof(ngtsqlite3), asOBJ_REF);
	engine->RegisterObjectBehaviour("sqlite3", asBEHAVE_FACTORY, "sqlite3@ db()", asFUNCTION(fngtsqlite3), asCALL_CDECL);
	engine->RegisterObjectBehaviour("sqlite3", asBEHAVE_ADDREF, "void f()", asMETHOD(ngtsqlite3, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("sqlite3", asBEHAVE_RELEASE, "void f()", asMETHOD(ngtsqlite3, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int close()", asMETHOD(ngtsqlite3, close), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int open(const string&in, int=6)", asMETHOD(ngtsqlite3, open), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "sqlite3statement@ prepare(const string&in, int&out=void)", asMETHOD(ngtsqlite3, prepare), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int execute(const string&in)", asMETHOD(ngtsqlite3, execute), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int64 get_rows_changed() property", asMETHOD(ngtsqlite3, get_rows_changed), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int64 get_total_rows_changed() property", asMETHOD(ngtsqlite3, get_total_rows_changed), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int limit(int id, int val)", asMETHOD(ngtsqlite3, limit), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int set_authorizer(sqlite3authorizer@, const string&in=\"\")", asMETHOD(ngtsqlite3, set_authorizer), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int64 get_last_insert_rowid() property", asMETHOD(ngtsqlite3, get_last_insert_rowid), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "void set_last_insert_rowid(int64) property", asMETHOD(ngtsqlite3, set_last_insert_rowid), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "int get_last_error()", asMETHOD(ngtsqlite3, get_last_error), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "string get_last_error_text()", asMETHOD(ngtsqlite3, get_last_error_text), asCALL_THISCALL);
	engine->RegisterObjectMethod("sqlite3", "bool get_active() property", asMETHOD(ngtsqlite3, get_active), asCALL_THISCALL);
	engine->RegisterObjectMethod("string", "uint64 c_str()", asMETHOD(std::string, c_str), asCALL_THISCALL);
	engine->RegisterObjectMethod("wstring", "uint64 c_str()", asMETHOD(std::wstring, c_str), asCALL_THISCALL);
	engine->RegisterGlobalFunction("size_t sizeof(const ?&in = null)", asFUNCTION(as_sizeof), asCALL_GENERIC);
	RegisterMutex<Poco::FastMutex>(engine, "fast_mutex");
	RegisterMutex<Poco::Mutex>(engine, "mutex");
	RegisterMutex<Poco::NamedMutex>(engine, "named_mutex");
	engine->RegisterObjectBehaviour("mutex", asBEHAVE_FACTORY, "mutex@ m()", asFUNCTION(ObjectFactory<Poco::Mutex>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("fast_mutex", asBEHAVE_FACTORY, "fast_mutex@ m()", asFUNCTION(ObjectFactory<Poco::FastMutex>), asCALL_CDECL);
	engine->RegisterObjectBehaviour("named_mutex", asBEHAVE_FACTORY, "named_mutex@ m(const string&in name)", asFUNCTION((ObjectFactory<Poco::NamedMutex, const std::string&>)), asCALL_CDECL);


	engine->RegisterEnumValue(_O("keycode"), "KEY_UNKNOWN", SDL_SCANCODE_UNKNOWN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_A", SDL_SCANCODE_A);
	engine->RegisterEnumValue(_O("keycode"), "KEY_B", SDL_SCANCODE_B);
	engine->RegisterEnumValue(_O("keycode"), "KEY_C", SDL_SCANCODE_C);
	engine->RegisterEnumValue(_O("keycode"), "KEY_D", SDL_SCANCODE_D);
	engine->RegisterEnumValue(_O("keycode"), "KEY_E", SDL_SCANCODE_E);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F", SDL_SCANCODE_F);
	engine->RegisterEnumValue(_O("keycode"), "KEY_G", SDL_SCANCODE_G);
	engine->RegisterEnumValue(_O("keycode"), "KEY_H", SDL_SCANCODE_H);
	engine->RegisterEnumValue(_O("keycode"), "KEY_I", SDL_SCANCODE_I);
	engine->RegisterEnumValue(_O("keycode"), "KEY_J", SDL_SCANCODE_J);
	engine->RegisterEnumValue(_O("keycode"), "KEY_K", SDL_SCANCODE_K);
	engine->RegisterEnumValue(_O("keycode"), "KEY_L", SDL_SCANCODE_L);
	engine->RegisterEnumValue(_O("keycode"), "KEY_M", SDL_SCANCODE_M);
	engine->RegisterEnumValue(_O("keycode"), "KEY_N", SDL_SCANCODE_N);
	engine->RegisterEnumValue(_O("keycode"), "KEY_O", SDL_SCANCODE_O);
	engine->RegisterEnumValue(_O("keycode"), "KEY_P", SDL_SCANCODE_P);
	engine->RegisterEnumValue(_O("keycode"), "KEY_Q", SDL_SCANCODE_Q);
	engine->RegisterEnumValue(_O("keycode"), "KEY_R", SDL_SCANCODE_R);
	engine->RegisterEnumValue(_O("keycode"), "KEY_S", SDL_SCANCODE_S);
	engine->RegisterEnumValue(_O("keycode"), "KEY_T", SDL_SCANCODE_T);
	engine->RegisterEnumValue(_O("keycode"), "KEY_U", SDL_SCANCODE_U);
	engine->RegisterEnumValue(_O("keycode"), "KEY_V", SDL_SCANCODE_V);
	engine->RegisterEnumValue(_O("keycode"), "KEY_W", SDL_SCANCODE_W);
	engine->RegisterEnumValue(_O("keycode"), "KEY_X", SDL_SCANCODE_X);
	engine->RegisterEnumValue(_O("keycode"), "KEY_Y", SDL_SCANCODE_Y);
	engine->RegisterEnumValue(_O("keycode"), "KEY_Z", SDL_SCANCODE_Z);
	engine->RegisterEnumValue(_O("keycode"), "KEY_1", SDL_SCANCODE_1);
	engine->RegisterEnumValue(_O("keycode"), "KEY_2", SDL_SCANCODE_2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_3", SDL_SCANCODE_3);
	engine->RegisterEnumValue(_O("keycode"), "KEY_4", SDL_SCANCODE_4);
	engine->RegisterEnumValue(_O("keycode"), "KEY_5", SDL_SCANCODE_5);
	engine->RegisterEnumValue(_O("keycode"), "KEY_6", SDL_SCANCODE_6);
	engine->RegisterEnumValue(_O("keycode"), "KEY_7", SDL_SCANCODE_7);
	engine->RegisterEnumValue(_O("keycode"), "KEY_8", SDL_SCANCODE_8);
	engine->RegisterEnumValue(_O("keycode"), "KEY_9", SDL_SCANCODE_9);
	engine->RegisterEnumValue(_O("keycode"), "KEY_0", SDL_SCANCODE_0);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RETURN", SDL_SCANCODE_RETURN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_ESCAPE", SDL_SCANCODE_ESCAPE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_BACK", SDL_SCANCODE_BACKSPACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_TAB", SDL_SCANCODE_TAB);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SPACE", SDL_SCANCODE_SPACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MINUS", SDL_SCANCODE_MINUS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_EQUALS", SDL_SCANCODE_EQUALS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LEFTBRACKET", SDL_SCANCODE_LEFTBRACKET);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RIGHTBRACKET", SDL_SCANCODE_RIGHTBRACKET);
	engine->RegisterEnumValue(_O("keycode"), "KEY_BACKSLASH", SDL_SCANCODE_BACKSLASH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NONUSHASH", SDL_SCANCODE_NONUSHASH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SEMICOLON", SDL_SCANCODE_SEMICOLON);
	engine->RegisterEnumValue(_O("keycode"), "KEY_APOSTROPHE", SDL_SCANCODE_APOSTROPHE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_GRAVE", SDL_SCANCODE_GRAVE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_COMMA", SDL_SCANCODE_COMMA);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PERIOD", SDL_SCANCODE_PERIOD);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SLASH", SDL_SCANCODE_SLASH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CAPSLOCK", SDL_SCANCODE_CAPSLOCK);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F1", SDL_SCANCODE_F1);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F2", SDL_SCANCODE_F2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F3", SDL_SCANCODE_F3);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F4", SDL_SCANCODE_F4);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F5", SDL_SCANCODE_F5);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F6", SDL_SCANCODE_F6);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F7", SDL_SCANCODE_F7);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F8", SDL_SCANCODE_F8);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F9", SDL_SCANCODE_F9);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F10", SDL_SCANCODE_F10);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F11", SDL_SCANCODE_F11);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F12", SDL_SCANCODE_F12);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PRINTSCREEN", SDL_SCANCODE_PRINTSCREEN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SCROLLLOCK", SDL_SCANCODE_SCROLLLOCK);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PAUSE", SDL_SCANCODE_PAUSE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INSERT", SDL_SCANCODE_INSERT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_HOME", SDL_SCANCODE_HOME);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PAGEUP", SDL_SCANCODE_PAGEUP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_DELETE", SDL_SCANCODE_DELETE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_END", SDL_SCANCODE_END);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PAGEDOWN", SDL_SCANCODE_PAGEDOWN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RIGHT", SDL_SCANCODE_RIGHT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LEFT", SDL_SCANCODE_LEFT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_DOWN", SDL_SCANCODE_DOWN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_UP", SDL_SCANCODE_UP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMLOCKCLEAR", SDL_SCANCODE_NUMLOCKCLEAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_DIVIDE", SDL_SCANCODE_KP_DIVIDE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MULTIPLY", SDL_SCANCODE_KP_MULTIPLY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MINUS", SDL_SCANCODE_KP_MINUS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_PLUS", SDL_SCANCODE_KP_PLUS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_ENTER", SDL_SCANCODE_KP_ENTER);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_1", SDL_SCANCODE_KP_1);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_2", SDL_SCANCODE_KP_2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_3", SDL_SCANCODE_KP_3);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_4", SDL_SCANCODE_KP_4);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_5", SDL_SCANCODE_KP_5);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_6", SDL_SCANCODE_KP_6);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_7", SDL_SCANCODE_KP_7);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_8", SDL_SCANCODE_KP_8);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_9", SDL_SCANCODE_KP_9);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_0", SDL_SCANCODE_KP_0);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_PERIOD", SDL_SCANCODE_KP_PERIOD);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NONUSBACKSLASH", SDL_SCANCODE_NONUSBACKSLASH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_APPLICATION", SDL_SCANCODE_APPLICATION);
	engine->RegisterEnumValue(_O("keycode"), "KEY_POWER", SDL_SCANCODE_POWER);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_EQUALS", SDL_SCANCODE_KP_EQUALS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F13", SDL_SCANCODE_F13);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F14", SDL_SCANCODE_F14);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F15", SDL_SCANCODE_F15);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F16", SDL_SCANCODE_F16);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F17", SDL_SCANCODE_F17);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F18", SDL_SCANCODE_F18);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F19", SDL_SCANCODE_F19);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F20", SDL_SCANCODE_F20);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F21", SDL_SCANCODE_F21);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F22", SDL_SCANCODE_F22);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F23", SDL_SCANCODE_F23);
	engine->RegisterEnumValue(_O("keycode"), "KEY_F24", SDL_SCANCODE_F24);
	engine->RegisterEnumValue(_O("keycode"), "KEY_EXECUTE", SDL_SCANCODE_EXECUTE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_HELP", SDL_SCANCODE_HELP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MENU", SDL_SCANCODE_MENU);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SELECT", SDL_SCANCODE_SELECT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_STOP", SDL_SCANCODE_STOP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AGAIN", SDL_SCANCODE_AGAIN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_UNDO", SDL_SCANCODE_UNDO);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CUT", SDL_SCANCODE_CUT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_COPY", SDL_SCANCODE_COPY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_PASTE", SDL_SCANCODE_PASTE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_FIND", SDL_SCANCODE_FIND);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MUTE", SDL_SCANCODE_MUTE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_VOLUMEUP", SDL_SCANCODE_VOLUMEUP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_VOLUMEDOWN", SDL_SCANCODE_VOLUMEDOWN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_COMMA", SDL_SCANCODE_KP_COMMA);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_EQUALSAS400", SDL_SCANCODE_KP_EQUALSAS400);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL1", SDL_SCANCODE_INTERNATIONAL1);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL2", SDL_SCANCODE_INTERNATIONAL2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL3", SDL_SCANCODE_INTERNATIONAL3);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL4", SDL_SCANCODE_INTERNATIONAL4);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL5", SDL_SCANCODE_INTERNATIONAL5);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL6", SDL_SCANCODE_INTERNATIONAL6);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL7", SDL_SCANCODE_INTERNATIONAL7);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL8", SDL_SCANCODE_INTERNATIONAL8);
	engine->RegisterEnumValue(_O("keycode"), "KEY_INTERNATIONAL9", SDL_SCANCODE_INTERNATIONAL9);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG1", SDL_SCANCODE_LANG1);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG2", SDL_SCANCODE_LANG2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG3", SDL_SCANCODE_LANG3);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG4", SDL_SCANCODE_LANG4);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG5", SDL_SCANCODE_LANG5);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG6", SDL_SCANCODE_LANG6);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG7", SDL_SCANCODE_LANG7);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG8", SDL_SCANCODE_LANG8);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LANG9", SDL_SCANCODE_LANG9);
	engine->RegisterEnumValue(_O("keycode"), "KEY_ALTERASE", SDL_SCANCODE_ALTERASE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SYSREQ", SDL_SCANCODE_SYSREQ);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CANCEL", SDL_SCANCODE_CANCEL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CLEAR", SDL_SCANCODE_CLEAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SDL_PRIOR", SDL_SCANCODE_PRIOR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RETURN2", SDL_SCANCODE_RETURN2);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SEPARATOR", SDL_SCANCODE_SEPARATOR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_OUT", SDL_SCANCODE_OUT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_OPER", SDL_SCANCODE_OPER);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CLEARAGAIN", SDL_SCANCODE_CLEARAGAIN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CRSEL", SDL_SCANCODE_CRSEL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_EXSEL", SDL_SCANCODE_EXSEL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_00", SDL_SCANCODE_KP_00);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_000", SDL_SCANCODE_KP_000);
	engine->RegisterEnumValue(_O("keycode"), "KEY_THOUSANDSSEPARATOR", SDL_SCANCODE_THOUSANDSSEPARATOR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_DECIMALSEPARATOR", SDL_SCANCODE_DECIMALSEPARATOR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CURRENCYUNIT", SDL_SCANCODE_CURRENCYUNIT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CURRENCYSUBUNIT", SDL_SCANCODE_CURRENCYSUBUNIT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_LEFTPAREN", SDL_SCANCODE_KP_LEFTPAREN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_RIGHTPAREN", SDL_SCANCODE_KP_RIGHTPAREN);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_LEFTBRACE", SDL_SCANCODE_KP_LEFTBRACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_RIGHTBRACE", SDL_SCANCODE_KP_RIGHTBRACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_TAB", SDL_SCANCODE_KP_TAB);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_BACKSPACE", SDL_SCANCODE_KP_BACKSPACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_A", SDL_SCANCODE_KP_A);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_B", SDL_SCANCODE_KP_B);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_C", SDL_SCANCODE_KP_C);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_D", SDL_SCANCODE_KP_D);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_E", SDL_SCANCODE_KP_E);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_F", SDL_SCANCODE_KP_F);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_XOR", SDL_SCANCODE_KP_XOR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_POWER", SDL_SCANCODE_KP_POWER);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_PERCENT", SDL_SCANCODE_KP_PERCENT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_LESS", SDL_SCANCODE_KP_LESS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_GREATER", SDL_SCANCODE_KP_GREATER);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_AMPERSAND", SDL_SCANCODE_KP_AMPERSAND);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_DBLAMPERSAND", SDL_SCANCODE_KP_DBLAMPERSAND);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_VERTICALBAR", SDL_SCANCODE_KP_VERTICALBAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_DBLVERTICALBAR", SDL_SCANCODE_KP_DBLVERTICALBAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_COLON", SDL_SCANCODE_KP_COLON);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_HASH", SDL_SCANCODE_KP_HASH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_SPACE", SDL_SCANCODE_KP_SPACE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_AT", SDL_SCANCODE_KP_AT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_EXCLAM", SDL_SCANCODE_KP_EXCLAM);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMSTORE", SDL_SCANCODE_KP_MEMSTORE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMRECALL", SDL_SCANCODE_KP_MEMRECALL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMCLEAR", SDL_SCANCODE_KP_MEMCLEAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMADD", SDL_SCANCODE_KP_MEMADD);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMSUBTRACT", SDL_SCANCODE_KP_MEMSUBTRACT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMMULTIPLY", SDL_SCANCODE_KP_MEMMULTIPLY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_MEMDIVIDE", SDL_SCANCODE_KP_MEMDIVIDE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_PLUSMINUS", SDL_SCANCODE_KP_PLUSMINUS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_CLEAR", SDL_SCANCODE_KP_CLEAR);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_CLEARENTRY", SDL_SCANCODE_KP_CLEARENTRY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_BINARY", SDL_SCANCODE_KP_BINARY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_OCTAL", SDL_SCANCODE_KP_OCTAL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_DECIMAL", SDL_SCANCODE_KP_DECIMAL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_NUMPAD_HEXADECIMAL", SDL_SCANCODE_KP_HEXADECIMAL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LCTRL", SDL_SCANCODE_LCTRL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LSHIFT", SDL_SCANCODE_LSHIFT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LALT", SDL_SCANCODE_LALT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_LGUI", SDL_SCANCODE_LGUI);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RCTRL", SDL_SCANCODE_RCTRL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RSHIFT", SDL_SCANCODE_RSHIFT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RALT", SDL_SCANCODE_RALT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_RGUI", SDL_SCANCODE_RGUI);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MODE", SDL_SCANCODE_MODE);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_NEXT_TRACK", SDL_SCANCODE_MEDIA_NEXT_TRACK);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_PREVIOUS_TRACK", SDL_SCANCODE_MEDIA_PREVIOUS_TRACK);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_STOP", SDL_SCANCODE_MEDIA_STOP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_PLAY", SDL_SCANCODE_MEDIA_PLAY);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_SELECT", SDL_SCANCODE_MEDIA_SELECT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_SEARCH", SDL_SCANCODE_AC_SEARCH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_HOME", SDL_SCANCODE_AC_HOME);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_BACK", SDL_SCANCODE_AC_BACK);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_FORWARD", SDL_SCANCODE_AC_FORWARD);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_STOP", SDL_SCANCODE_AC_STOP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_REFRESH", SDL_SCANCODE_AC_REFRESH);
	engine->RegisterEnumValue(_O("keycode"), "KEY_AC_BOOKMARKS", SDL_SCANCODE_AC_BOOKMARKS);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_EJECT", SDL_SCANCODE_MEDIA_EJECT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SLEEP", SDL_SCANCODE_SLEEP);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_REWIND", SDL_SCANCODE_MEDIA_REWIND);
	engine->RegisterEnumValue(_O("keycode"), "KEY_MEDIA_FAST_FORWARD", SDL_SCANCODE_MEDIA_FAST_FORWARD);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SOFTLEFT", SDL_SCANCODE_SOFTLEFT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_SOFTRIGHT", SDL_SCANCODE_SOFTRIGHT);
	engine->RegisterEnumValue(_O("keycode"), "KEY_CALL", SDL_SCANCODE_CALL);
	engine->RegisterEnumValue(_O("keycode"), "KEY_ENDCALL", SDL_SCANCODE_ENDCALL);

}

