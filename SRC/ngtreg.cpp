﻿#include "MemoryStream.h"
#include "ngtreg.h"
#include "Poco/UnicodeConverter.h"
#include "scriptbuilder/scriptbuilder.h"
#include "Scripting.h"
#include "scriptstdstring/scriptstdstring.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
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
const int EVENT_NONE = ENET_EVENT_TYPE_NONE;
const int EVENT_CONNECT = ENET_EVENT_TYPE_CONNECT;
const int EVENT_RECEIVE = ENET_EVENT_TYPE_RECEIVE;
const int EVENT_DISCONNECT = ENET_EVENT_TYPE_DISCONNECT;
const int as_CDECL = 0;
const int AS_STD_CALL = 1;
using namespace std;
#ifdef _WIN32
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
void show_message()
{
	cout << "Compilation error\n" + g_ScriptMessagesError + g_ScriptMessagesWarning;
#ifdef _WIN32
	const wchar_t CLASS_NAME[] = L"NGTTextbox";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Compilation error",
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
		result_message = wstr(g_ScriptMessagesError + g_ScriptMessagesWarning);
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

}
#endif
timer* ftimer() { return new timer; }
user_idle* fuser_idle() { return new user_idle; }
library* flibrary() { return new library; }
void fscript_thread(asIScriptGeneric* gen) {
	asIScriptFunction* func = (asIScriptFunction*)gen->GetArgAddress(0);
	script_thread* th = new script_thread(func);
	gen->SetReturnObject(th);
}
TTSVoice* ftts_voice() { return new TTSVoice(); }
instance* finstance(std::string app) { return new instance(app); }
network_event* fnetwork_event() { return new network_event; }
ngtvector* fngtvector() { return new ngtvector; }
sqlite3statement* fsqlite3statement() { return new sqlite3statement; }
ngtsqlite3* fngtsqlite3() { return new ngtsqlite3; }
network* fnetwork() { return new network; }


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

void Print(const char* format, ...)
{

	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
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
	engine->RegisterTypedef("surface", "int64");
	engine->RegisterObjectType("rect", sizeof(SDL_Rect), asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectBehaviour("rect", asBEHAVE_FACTORY, "rect@ r()", asFUNCTION(frect), asCALL_CDECL);
	engine->RegisterObjectProperty("rect", "int x", asOFFSET(SDL_Rect, x));
	engine->RegisterObjectProperty("rect", "int y", asOFFSET(SDL_Rect, y));
	engine->RegisterObjectProperty("rect", "int h", asOFFSET(SDL_Rect, h));
	engine->RegisterObjectProperty("rect", "int w", asOFFSET(SDL_Rect, w));
	engine->RegisterFuncdef("int exit_callback()");
	engine->RegisterObjectType("vector", sizeof(ngtvector), asOBJ_REF);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_FACTORY, "vector@ v()", asFUNCTION(fngtvector), asCALL_CDECL);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_ADDREF, "void f()", asMETHOD(ngtvector, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("vector", asBEHAVE_RELEASE, "void f()", asMETHOD(ngtvector, release), asCALL_THISCALL);
	engine->RegisterObjectProperty("vector", "float x", asOFFSET(ngtvector, x));
	engine->RegisterObjectProperty("vector", "float y", asOFFSET(ngtvector, y));
	engine->RegisterObjectProperty("vector", "float z", asOFFSET(ngtvector, z));
	engine->RegisterObjectMethod("vector", "float get_length()const property", asMETHOD(ngtvector, get_length), asCALL_THISCALL);
	engine->RegisterObjectMethod("vector", "vector &opAssign(const vector&in)", asMETHOD(ngtvector, operator=), asCALL_THISCALL);
	engine->RegisterObjectMethod("vector", "void reset()const", asMETHOD(ngtvector, reset), asCALL_THISCALL);
	AS_BEGIN(engine, "os");
	engine->RegisterGlobalFunction("int get_cpu_count()property", asFUNCTION(get_cpu_count), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_system_ram()property", asFUNCTION(get_system_ram), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_platform()property", asFUNCTION(get_platform), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("uint64 get_time_stamp_millis()property", asFUNCTION(get_time_stamp_millis), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_time_stamp_seconds()property", asFUNCTION(get_time_stamp_seconds), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_library_path(const string &in)", asFUNCTION(set_library_path), asCALL_CDECL);

	engine->RegisterGlobalFunction("long random(long, long)", asFUNCTIONPR(random, (long, long), long), asCALL_CDECL);
	engine->RegisterGlobalFunction("double random(double, double)", asFUNCTIONPR(randomDouble, (double, double), double), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool random_bool()", asFUNCTION(random_bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void printf(string &in, ?&in var = 0, ?&in var2 = 0, ?&in var3 = 0, ?&in var4 = 0, ?&in var5 = 0, ?&in var6 = 0, ?&in var7 = 0, ?&in var8 = 0, ?&in var9 = 0, ?&in var10 = 0, ?&in var11 = 0, ?&in var12 = 0, ?&in var13 = 0, ?&in var14 = 0, ?&in var15 = 0)", asFUNCTION(as_printf), asCALL_GENERIC);

	engine->RegisterGlobalFunction("int get_last_error()property", asFUNCTION(get_last_error), asCALL_CDECL);
	AS_BEGIN(engine, "screen_reader");
	engine->RegisterGlobalFunction("void set_interrupt(bool)property", asFUNCTION(set_screen_reader_interrupt), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_interrupt()property", asFUNCTION(get_screen_reader_interrupt), asCALL_CDECL);

	engine->RegisterGlobalFunction("void speak(const string &in, bool=interrupt)", asFUNCTION(speak), asCALL_CDECL);
	engine->RegisterGlobalFunction("void speak_wait(const string &in, bool=screen_reader_interrupt)", asFUNCTION(speak_wait), asCALL_CDECL);

	engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
	engine->RegisterGlobalFunction("string detect()", asFUNCTION(screen_reader_detect), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("void set_exit_callback(exit_callback@=null)", asFUNCTION(set_exit_callback), asCALL_CDECL);

	engine->RegisterGlobalFunction("void show_console()", asFUNCTION(show_console), asCALL_CDECL);
	engine->RegisterGlobalFunction("void hide_console()", asFUNCTION(hide_console), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool show_window(const string &in,int=640,int=480, bool=true)", asFUNCTION(show_window), asCALL_CDECL);
	//	engine->RegisterGlobalFunction("bool focus_window()", asFUNCTION(focus_window), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_window_handle()", asFUNCTION(get_window_handle), asCALL_CDECL);
	engine->RegisterGlobalFunction("void hide_window()", asFUNCTION(hide_window), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_update_window_freq(long)property", asFUNCTION(set_update_window_freq), asCALL_CDECL);
	engine->RegisterGlobalFunction("long get_update_window_freq()property", asFUNCTION(get_update_window_freq), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_window_title(const string &in)property", asFUNCTION(set_window_title), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_window_closable(bool)property", asFUNCTION(set_window_closable), asCALL_CDECL);

	engine->RegisterGlobalFunction("void garbage_collect()", asFUNCTION(garbage_collect), asCALL_CDECL);
	engine->RegisterGlobalFunction("surface get_window_surface()property", asFUNCTION(get_window_surface), asCALL_CDECL);
	engine->RegisterGlobalFunction("void free_surface(surface)", asFUNCTION(SDL_DestroySurface), asCALL_CDECL);
	engine->RegisterGlobalFunction("surface load_bmp(const string &in)", asFUNCTION(load_bmp), asCALL_CDECL);
	engine->RegisterGlobalFunction("int fill_rect(surface, rect@, uint32)", asFUNCTION(SDL_FillSurfaceRect), asCALL_CDECL);
	engine->RegisterGlobalFunction("int blit_surface(surface, rect@, surface, rect@)", asFUNCTION(SDL_BlitSurface), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_window_active()property", asFUNCTION(get_window_active), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_window_fullscreen(bool)property", asFUNCTION(set_window_fullscreen), asCALL_CDECL);

	engine->RegisterGlobalFunction("bool mouse_pressed(uint8)", asFUNCTION(mouse_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool mouse_released(uint8)", asFUNCTION(mouse_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool mouse_down(uint8)", asFUNCTION(mouse_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_X()property", asFUNCTION(get_MOUSE_X), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_Y()property", asFUNCTION(get_MOUSE_Y), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_MOUSE_Z()property", asFUNCTION(get_MOUSE_Z), asCALL_CDECL);
	AS_BEGIN(engine, "internet");
	engine->RegisterGlobalFunction("void ftp_download(const string& in, const string &in)", asFUNCTION(ftp_download), asCALL_CDECL);
	engine->RegisterEnum("smtp_login_method");
	engine->RegisterEnumValue("smtp_login_method", "AUTH_NONE", Poco::Net::SMTPClientSession::AUTH_NONE);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_CRAM_MD5", Poco::Net::SMTPClientSession::AUTH_CRAM_MD5);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_CRAM_SHA1", Poco::Net::SMTPClientSession::AUTH_CRAM_SHA1);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_LOGIN", Poco::Net::SMTPClientSession::AUTH_LOGIN);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_PLAIN", Poco::Net::SMTPClientSession::AUTH_PLAIN);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_XOAUTH2", Poco::Net::SMTPClientSession::AUTH_XOAUTH2);
	engine->RegisterEnumValue("smtp_login_method", "AUTH_NTLM", Poco::Net::SMTPClientSession::AUTH_NTLM);
	engine->RegisterGlobalFunction("void mail_send(smtp_login_method, uint, const string &in, const string &in, const string&in, const string&in, const string&in, const string&in, const string&in, const string&in=\"\")", asFUNCTION(mail_send), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("void exit(int=0)", asFUNCTION(exit_engine), asCALL_CDECL);
	engine->RegisterGlobalFunction("string number_to_words(uint64, bool)", asFUNCTION(number_to_words), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool clipboard_copy_text(const string &in)", asFUNCTION(clipboard_copy_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string clipboard_read_text()", asFUNCTION(clipboard_read_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_input()", asFUNCTION(get_input), asCALL_CDECL);
	engine->RegisterGlobalFunction("string input_box(const string &in, const string &in, const string &in=\"\", bool=false, bool=false)", asFUNCTION(input_box), asCALL_CDECL);

	engine->RegisterEnum("keycode");
	engine->RegisterGlobalFunction("bool key_pressed(keycode)", asFUNCTION(key_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_released(keycode)", asFUNCTION(key_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_down(keycode)", asFUNCTION(key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_repeat(keycode)", asFUNCTION(key_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_pressed()", asFUNCTION(keys_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_released()", asFUNCTION(keys_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_down()", asFUNCTION(keys_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("int[]@ keys_repeat()", asFUNCTION(keys_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("string key_to_string(int)", asFUNCTION(key_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("int string_to_key(const string&in)", asFUNCTION(string_to_key), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool force_key_down(keycode)", asFUNCTION(force_key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool force_key_up(keycode)", asFUNCTION(force_key_up), asCALL_CDECL);
	engine->RegisterGlobalFunction("void reset_all_forced_keys()", asFUNCTION(reset_all_forced_keys), asCALL_CDECL);

	engine->RegisterGlobalFunction("string string_encrypt(const string &in, string &in)", asFUNCTION(string_encrypt), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_decrypt(const string &in, string &in)", asFUNCTION(string_decrypt), asCALL_CDECL);
	AS_BEGIN(engine, "internet");
	engine->RegisterGlobalFunction("string url_decode(const string &in)", asFUNCTION(url_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_encode(const string &in)", asFUNCTION(url_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_get(const string &in)", asFUNCTION(url_get), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_post(const string &in, const string &in)", asFUNCTION(url_post), asCALL_CDECL);
	AS_END(engine);
	engine->RegisterGlobalFunction("string ascii_to_character(int)", asFUNCTION(ascii_to_character), asCALL_CDECL);
	engine->RegisterGlobalFunction("int character_to_ascii(const string      &in)", asFUNCTION(character_to_ascii), asCALL_CDECL);
	engine->RegisterGlobalFunction("string hex_to_string(const string& in)", asFUNCTION(hex_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("string number_to_hex_string(double)", asFUNCTION(number_to_hex_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base64_decode(const string &in)", asFUNCTION(string_base64_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base64_encode(const string &in)", asFUNCTION(string_base64_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base32_decode(const string &in)", asFUNCTION(string_base32_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_base32_encode(const string &in)", asFUNCTION(string_base32_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_to_hex(const string &in)", asFUNCTION(string_to_hex), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool alert(const string &in, const string &in, const string &in=\"OK\")", asFUNCTION(alert), asCALL_CDECL);
	engine->RegisterGlobalFunction("int question(const string &in, const string &in)", asFUNCTION(question), asCALL_CDECL);

	engine->RegisterGlobalFunction("void wait(uint64)", asFUNCTION(wait), asCALL_CDECL);
	engine->RegisterGlobalFunction("void wait_event()", asFUNCTION(wait_event), asCALL_CDECL);

	engine->RegisterGlobalFunction("string read_environment_variable(const string&in)", asFUNCTION(read_environment_variable), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool write_environment_variable(const string&in, const string &in)", asFUNCTION(write_environment_variable), asCALL_CDECL);

	engine->RegisterGlobalFunction("string serialize(dictionary@=null)", asFUNCTION(serialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("dictionary@ deserialize(const string &in)", asFUNCTION(deserialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool urlopen(const string &in)", asFUNCTION(urlopen), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 malloc(uint64)", asFUNCTION(malloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 calloc(uint64, uint64)", asFUNCTION(calloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 realloc(uint64, uint64)", asFUNCTION(realloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("void free(uint64)", asFUNCTION(free), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 memcpy(uint64, uint64, size_t)", asFUNCTION(memcpy), asCALL_CDECL);

	engine->RegisterGlobalFunction("string c_str_to_string(uint64, size_t=0)", asFUNCTION(c_str_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("wstring wc_str_to_wstring(uint64, size_t=0)", asFUNCTION(wc_str_to_wstring), asCALL_CDECL);

	engine->RegisterGlobalFunction("size_t c_str_len(uint64)", asFUNCTION(strlen), asCALL_CDECL);
	engine->RegisterGlobalFunction("size_t wc_str_len(uint64)", asFUNCTION(wcslen), asCALL_CDECL);
	engine->RegisterGlobalFunction("void unicode_convert(const string &in, wstring &out)", asFUNCTIONPR(Poco::UnicodeConverter::convert, (const std::string&, std::wstring&), void), asCALL_CDECL);
	engine->RegisterGlobalFunction("void unicode_convert(const wstring &in, string &out)", asFUNCTIONPR(Poco::UnicodeConverter::convert, (const std::wstring&, std::string&), void), asCALL_CDECL);
	register_pack(engine);
	register_sound(engine);
	RegisterMemstream(engine);
	AS_BEGIN(engine, "scripting");
	RegisterScripting(engine);
	AS_END(engine);
	engine->RegisterObjectType("tts_voice", sizeof(TTSVoice), asOBJ_REF);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_FACTORY, "tts_voice@ v()", asFUNCTION(ftts_voice), asCALL_CDECL);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_ADDREF, "void f()", asMETHOD(TTSVoice, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("tts_voice", asBEHAVE_RELEASE, "void f()", asMETHOD(TTSVoice, release), asCALL_THISCALL);


	engine->RegisterObjectMethod("tts_voice", "void speak(const string& in)const", asMETHOD(TTSVoice, speak), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void speak_wait(const string& in)const", asMETHOD(TTSVoice, speak_wait), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void speak_interrupt(const string& in)const", asMETHOD(TTSVoice, speak_interrupt), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void speak_interrupt_wait(const string& in)const", asMETHOD(TTSVoice, speak_interrupt_wait), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "int get_rate()const property", asMETHOD(TTSVoice, get_rate), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void set_rate(int)property", asMETHOD(TTSVoice, set_rate), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "int get_volume()const property", asMETHOD(TTSVoice, get_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void set_volume(int)property", asMETHOD(TTSVoice, set_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "void set_voice(uint64)property", asMETHOD(TTSVoice, set_voice), asCALL_THISCALL);
	engine->RegisterObjectMethod("tts_voice", "array<string>@ get_voice_names()const property", asMETHOD(TTSVoice, get_voice_names), asCALL_THISCALL);

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
	engine->RegisterObjectMethod("library", "bool load(const string&in)const", asMETHOD(library, load), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "bool get_active()const property", asMETHOD(library, active), asCALL_THISCALL);

	engine->RegisterObjectMethod("library", "dictionary@ call(const string&in, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null)const", asFUNCTION(library_call), asCALL_GENERIC);
	engine->RegisterObjectMethod("library", "void unload()const", asMETHOD(library, unload), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "uint64 get_function_pointer(const string &in)const", asMETHOD(library, get_function_pointer), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "void clear_functions()const", asMETHOD(library, clear_functions), asCALL_THISCALL);

	engine->RegisterFuncdef("void thread_func()");
	engine->RegisterObjectType("thread", sizeof(script_thread), asOBJ_REF);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_FACTORY, "thread@ t(thread_func@)", asFUNCTION(fscript_thread), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_ADDREF, "void f()", asMETHOD(script_thread, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("thread", asBEHAVE_RELEASE, "void f()", asMETHOD(script_thread, release), asCALL_THISCALL);

	engine->RegisterObjectMethod("thread", "void detach()const", asMETHOD(script_thread, detach), asCALL_THISCALL);
	engine->RegisterObjectMethod("thread", "void join()const", asMETHOD(script_thread, join), asCALL_THISCALL);
	engine->RegisterObjectMethod("thread", "void destroy()const", asMETHOD(script_thread, destroy), asCALL_THISCALL);
	engine->RegisterObjectType("instance", sizeof(instance), asOBJ_REF);
	engine->RegisterObjectBehaviour("instance", asBEHAVE_FACTORY, "instance@ i(const string&in)", asFUNCTION(finstance), asCALL_CDECL);
	engine->RegisterObjectBehaviour("instance", asBEHAVE_ADDREF, "void f()", asMETHOD(instance, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("instance", asBEHAVE_RELEASE, "void f()", asMETHOD(instance, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("instance", "bool is_running()", asMETHOD(instance, is_running), asCALL_THISCALL);


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
	engine->RegisterObjectProperty("network_event", "const uint peer_id", asOFFSET(network_event, m_peerId));
	engine->RegisterObjectProperty("network_event", "const int channel", asOFFSET(network_event, m_channel));
	engine->RegisterObjectProperty("network_event", "const string message", asOFFSET(network_event, m_message));


	engine->RegisterObjectType("network", sizeof(network), asOBJ_REF);
	engine->RegisterObjectBehaviour("network", asBEHAVE_FACTORY, "network@ n()", asFUNCTION(fnetwork), asCALL_CDECL);
	engine->RegisterObjectBehaviour("network", asBEHAVE_ADDREF, "void f()", asMETHOD(network, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("network", asBEHAVE_RELEASE, "void f()", asMETHOD(network, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "uint connect(string&in, int)", asMETHOD(network, connect), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool destroy()", asMETHOD(network, destroy), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool disconnect_peer(uint)", asMETHOD(network, disconnect_peer), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool disconnect_peer_forcefully(uint)", asMETHOD(network, disconnect_peer_forcefully), asCALL_THISCALL);
	//    engine->RegisterObjectMethod("network", "bool disconnect_peer_softly(uint)", asMETHOD(network, disconnect_peer_softly), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "string get_peer_address(uint)const property", asMETHOD(network, get_peer_address), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "double get_peer_average_round_trip_time(uint)const property", asMETHOD(network, get_peer_average_round_trip_time), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "uint[]@ get_peer_list()", asMETHOD(network, get_peer_list), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "network_event@ request(int=0)", asMETHOD(network, request), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool send_reliable(uint, string&in, int)", asMETHOD(network, send_reliable), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool send_unreliable(uint, string&in, int)", asMETHOD(network, send_unreliable), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool set_bandwidth_limits(double, double)", asMETHOD(network, set_bandwidth_limits), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool setup_client(int, int)", asMETHOD(network, setup_client), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "bool setup_server(int, int, int)", asMETHOD(network, setup_server), asCALL_THISCALL);
	engine->RegisterObjectMethod("network", "void flush()const", asMETHOD(network, flush), asCALL_THISCALL);

	engine->RegisterObjectMethod("network", "int get_connected_peers() const property", asMETHOD(network, get_connected_peers), asCALL_THISCALL);
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
	engine->RegisterEnumValue("keycode", "KEY_UNKNOWN", SDL_SCANCODE_UNKNOWN);
	engine->RegisterEnumValue("keycode", "KEY_A", SDL_SCANCODE_A);
	engine->RegisterEnumValue("keycode", "KEY_B", SDL_SCANCODE_B);
	engine->RegisterEnumValue("keycode", "KEY_C", SDL_SCANCODE_C);
	engine->RegisterEnumValue("keycode", "KEY_D", SDL_SCANCODE_D);
	engine->RegisterEnumValue("keycode", "KEY_E", SDL_SCANCODE_E);
	engine->RegisterEnumValue("keycode", "KEY_F", SDL_SCANCODE_F);
	engine->RegisterEnumValue("keycode", "KEY_G", SDL_SCANCODE_G);
	engine->RegisterEnumValue("keycode", "KEY_H", SDL_SCANCODE_H);
	engine->RegisterEnumValue("keycode", "KEY_I", SDL_SCANCODE_I);
	engine->RegisterEnumValue("keycode", "KEY_J", SDL_SCANCODE_J);
	engine->RegisterEnumValue("keycode", "KEY_K", SDL_SCANCODE_K);
	engine->RegisterEnumValue("keycode", "KEY_L", SDL_SCANCODE_L);
	engine->RegisterEnumValue("keycode", "KEY_M", SDL_SCANCODE_M);
	engine->RegisterEnumValue("keycode", "KEY_N", SDL_SCANCODE_N);
	engine->RegisterEnumValue("keycode", "KEY_O", SDL_SCANCODE_O);
	engine->RegisterEnumValue("keycode", "KEY_P", SDL_SCANCODE_P);
	engine->RegisterEnumValue("keycode", "KEY_Q", SDL_SCANCODE_Q);
	engine->RegisterEnumValue("keycode", "KEY_R", SDL_SCANCODE_R);
	engine->RegisterEnumValue("keycode", "KEY_S", SDL_SCANCODE_S);
	engine->RegisterEnumValue("keycode", "KEY_T", SDL_SCANCODE_T);
	engine->RegisterEnumValue("keycode", "KEY_U", SDL_SCANCODE_U);
	engine->RegisterEnumValue("keycode", "KEY_V", SDL_SCANCODE_V);
	engine->RegisterEnumValue("keycode", "KEY_W", SDL_SCANCODE_W);
	engine->RegisterEnumValue("keycode", "KEY_X", SDL_SCANCODE_X);
	engine->RegisterEnumValue("keycode", "KEY_Y", SDL_SCANCODE_Y);
	engine->RegisterEnumValue("keycode", "KEY_Z", SDL_SCANCODE_Z);
	engine->RegisterEnumValue("keycode", "KEY_1", SDL_SCANCODE_1);
	engine->RegisterEnumValue("keycode", "KEY_2", SDL_SCANCODE_2);
	engine->RegisterEnumValue("keycode", "KEY_3", SDL_SCANCODE_3);
	engine->RegisterEnumValue("keycode", "KEY_4", SDL_SCANCODE_4);
	engine->RegisterEnumValue("keycode", "KEY_5", SDL_SCANCODE_5);
	engine->RegisterEnumValue("keycode", "KEY_6", SDL_SCANCODE_6);
	engine->RegisterEnumValue("keycode", "KEY_7", SDL_SCANCODE_7);
	engine->RegisterEnumValue("keycode", "KEY_8", SDL_SCANCODE_8);
	engine->RegisterEnumValue("keycode", "KEY_9", SDL_SCANCODE_9);
	engine->RegisterEnumValue("keycode", "KEY_0", SDL_SCANCODE_0);
	engine->RegisterEnumValue("keycode", "KEY_RETURN", SDL_SCANCODE_RETURN);
	engine->RegisterEnumValue("keycode", "KEY_ESCAPE", SDL_SCANCODE_ESCAPE);
	engine->RegisterEnumValue("keycode", "KEY_BACK", SDL_SCANCODE_BACKSPACE);
	engine->RegisterEnumValue("keycode", "KEY_TAB", SDL_SCANCODE_TAB);
	engine->RegisterEnumValue("keycode", "KEY_SPACE", SDL_SCANCODE_SPACE);
	engine->RegisterEnumValue("keycode", "KEY_MINUS", SDL_SCANCODE_MINUS);
	engine->RegisterEnumValue("keycode", "KEY_EQUALS", SDL_SCANCODE_EQUALS);
	engine->RegisterEnumValue("keycode", "KEY_LEFTBRACKET", SDL_SCANCODE_LEFTBRACKET);
	engine->RegisterEnumValue("keycode", "KEY_RIGHTBRACKET", SDL_SCANCODE_RIGHTBRACKET);
	engine->RegisterEnumValue("keycode", "KEY_BACKSLASH", SDL_SCANCODE_BACKSLASH);
	engine->RegisterEnumValue("keycode", "KEY_NONUSHASH", SDL_SCANCODE_NONUSHASH);
	engine->RegisterEnumValue("keycode", "KEY_SEMICOLON", SDL_SCANCODE_SEMICOLON);
	engine->RegisterEnumValue("keycode", "KEY_APOSTROPHE", SDL_SCANCODE_APOSTROPHE);
	engine->RegisterEnumValue("keycode", "KEY_GRAVE", SDL_SCANCODE_GRAVE);
	engine->RegisterEnumValue("keycode", "KEY_COMMA", SDL_SCANCODE_COMMA);
	engine->RegisterEnumValue("keycode", "KEY_PERIOD", SDL_SCANCODE_PERIOD);
	engine->RegisterEnumValue("keycode", "KEY_SLASH", SDL_SCANCODE_SLASH);
	engine->RegisterEnumValue("keycode", "KEY_CAPSLOCK", SDL_SCANCODE_CAPSLOCK);
	engine->RegisterEnumValue("keycode", "KEY_F1", SDL_SCANCODE_F1);
	engine->RegisterEnumValue("keycode", "KEY_F2", SDL_SCANCODE_F2);
	engine->RegisterEnumValue("keycode", "KEY_F3", SDL_SCANCODE_F3);
	engine->RegisterEnumValue("keycode", "KEY_F4", SDL_SCANCODE_F4);
	engine->RegisterEnumValue("keycode", "KEY_F5", SDL_SCANCODE_F5);
	engine->RegisterEnumValue("keycode", "KEY_F6", SDL_SCANCODE_F6);
	engine->RegisterEnumValue("keycode", "KEY_F7", SDL_SCANCODE_F7);
	engine->RegisterEnumValue("keycode", "KEY_F8", SDL_SCANCODE_F8);
	engine->RegisterEnumValue("keycode", "KEY_F9", SDL_SCANCODE_F9);
	engine->RegisterEnumValue("keycode", "KEY_F10", SDL_SCANCODE_F10);
	engine->RegisterEnumValue("keycode", "KEY_F11", SDL_SCANCODE_F11);
	engine->RegisterEnumValue("keycode", "KEY_F12", SDL_SCANCODE_F12);
	engine->RegisterEnumValue("keycode", "KEY_PRINTSCREEN", SDL_SCANCODE_PRINTSCREEN);
	engine->RegisterEnumValue("keycode", "KEY_SCROLLLOCK", SDL_SCANCODE_SCROLLLOCK);
	engine->RegisterEnumValue("keycode", "KEY_PAUSE", SDL_SCANCODE_PAUSE);
	engine->RegisterEnumValue("keycode", "KEY_INSERT", SDL_SCANCODE_INSERT);
	engine->RegisterEnumValue("keycode", "KEY_HOME", SDL_SCANCODE_HOME);
	engine->RegisterEnumValue("keycode", "KEY_PAGEUP", SDL_SCANCODE_PAGEUP);
	engine->RegisterEnumValue("keycode", "KEY_DELETE", SDL_SCANCODE_DELETE);
	engine->RegisterEnumValue("keycode", "KEY_END", SDL_SCANCODE_END);
	engine->RegisterEnumValue("keycode", "KEY_PAGEDOWN", SDL_SCANCODE_PAGEDOWN);
	engine->RegisterEnumValue("keycode", "KEY_RIGHT", SDL_SCANCODE_RIGHT);
	engine->RegisterEnumValue("keycode", "KEY_LEFT", SDL_SCANCODE_LEFT);
	engine->RegisterEnumValue("keycode", "KEY_DOWN", SDL_SCANCODE_DOWN);
	engine->RegisterEnumValue("keycode", "KEY_UP", SDL_SCANCODE_UP);
	engine->RegisterEnumValue("keycode", "KEY_NUMLOCKCLEAR", SDL_SCANCODE_NUMLOCKCLEAR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_DIVIDE", SDL_SCANCODE_KP_DIVIDE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MULTIPLY", SDL_SCANCODE_KP_MULTIPLY);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MINUS", SDL_SCANCODE_KP_MINUS);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_PLUS", SDL_SCANCODE_KP_PLUS);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_ENTER", SDL_SCANCODE_KP_ENTER);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_1", SDL_SCANCODE_KP_1);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_2", SDL_SCANCODE_KP_2);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_3", SDL_SCANCODE_KP_3);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_4", SDL_SCANCODE_KP_4);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_5", SDL_SCANCODE_KP_5);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_6", SDL_SCANCODE_KP_6);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_7", SDL_SCANCODE_KP_7);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_8", SDL_SCANCODE_KP_8);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_9", SDL_SCANCODE_KP_9);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_0", SDL_SCANCODE_KP_0);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_PERIOD", SDL_SCANCODE_KP_PERIOD);
	engine->RegisterEnumValue("keycode", "KEY_NONUSBACKSLASH", SDL_SCANCODE_NONUSBACKSLASH);
	engine->RegisterEnumValue("keycode", "KEY_APPLICATION", SDL_SCANCODE_APPLICATION);
	engine->RegisterEnumValue("keycode", "KEY_POWER", SDL_SCANCODE_POWER);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_EQUALS", SDL_SCANCODE_KP_EQUALS);
	engine->RegisterEnumValue("keycode", "KEY_F13", SDL_SCANCODE_F13);
	engine->RegisterEnumValue("keycode", "KEY_F14", SDL_SCANCODE_F14);
	engine->RegisterEnumValue("keycode", "KEY_F15", SDL_SCANCODE_F15);
	engine->RegisterEnumValue("keycode", "KEY_F16", SDL_SCANCODE_F16);
	engine->RegisterEnumValue("keycode", "KEY_F17", SDL_SCANCODE_F17);
	engine->RegisterEnumValue("keycode", "KEY_F18", SDL_SCANCODE_F18);
	engine->RegisterEnumValue("keycode", "KEY_F19", SDL_SCANCODE_F19);
	engine->RegisterEnumValue("keycode", "KEY_F20", SDL_SCANCODE_F20);
	engine->RegisterEnumValue("keycode", "KEY_F21", SDL_SCANCODE_F21);
	engine->RegisterEnumValue("keycode", "KEY_F22", SDL_SCANCODE_F22);
	engine->RegisterEnumValue("keycode", "KEY_F23", SDL_SCANCODE_F23);
	engine->RegisterEnumValue("keycode", "KEY_F24", SDL_SCANCODE_F24);
	engine->RegisterEnumValue("keycode", "KEY_EXECUTE", SDL_SCANCODE_EXECUTE);
	engine->RegisterEnumValue("keycode", "KEY_HELP", SDL_SCANCODE_HELP);
	engine->RegisterEnumValue("keycode", "KEY_MENU", SDL_SCANCODE_MENU);
	engine->RegisterEnumValue("keycode", "KEY_SELECT", SDL_SCANCODE_SELECT);
	engine->RegisterEnumValue("keycode", "KEY_STOP", SDL_SCANCODE_STOP);
	engine->RegisterEnumValue("keycode", "KEY_AGAIN", SDL_SCANCODE_AGAIN);
	engine->RegisterEnumValue("keycode", "KEY_UNDO", SDL_SCANCODE_UNDO);
	engine->RegisterEnumValue("keycode", "KEY_CUT", SDL_SCANCODE_CUT);
	engine->RegisterEnumValue("keycode", "KEY_COPY", SDL_SCANCODE_COPY);
	engine->RegisterEnumValue("keycode", "KEY_PASTE", SDL_SCANCODE_PASTE);
	engine->RegisterEnumValue("keycode", "KEY_FIND", SDL_SCANCODE_FIND);
	engine->RegisterEnumValue("keycode", "KEY_MUTE", SDL_SCANCODE_MUTE);
	engine->RegisterEnumValue("keycode", "KEY_VOLUMEUP", SDL_SCANCODE_VOLUMEUP);
	engine->RegisterEnumValue("keycode", "KEY_VOLUMEDOWN", SDL_SCANCODE_VOLUMEDOWN);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_COMMA", SDL_SCANCODE_KP_COMMA);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_EQUALSAS400", SDL_SCANCODE_KP_EQUALSAS400);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL1", SDL_SCANCODE_INTERNATIONAL1);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL2", SDL_SCANCODE_INTERNATIONAL2);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL3", SDL_SCANCODE_INTERNATIONAL3);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL4", SDL_SCANCODE_INTERNATIONAL4);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL5", SDL_SCANCODE_INTERNATIONAL5);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL6", SDL_SCANCODE_INTERNATIONAL6);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL7", SDL_SCANCODE_INTERNATIONAL7);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL8", SDL_SCANCODE_INTERNATIONAL8);
	engine->RegisterEnumValue("keycode", "KEY_INTERNATIONAL9", SDL_SCANCODE_INTERNATIONAL9);
	engine->RegisterEnumValue("keycode", "KEY_LANG1", SDL_SCANCODE_LANG1);
	engine->RegisterEnumValue("keycode", "KEY_LANG2", SDL_SCANCODE_LANG2);
	engine->RegisterEnumValue("keycode", "KEY_LANG3", SDL_SCANCODE_LANG3);
	engine->RegisterEnumValue("keycode", "KEY_LANG4", SDL_SCANCODE_LANG4);
	engine->RegisterEnumValue("keycode", "KEY_LANG5", SDL_SCANCODE_LANG5);
	engine->RegisterEnumValue("keycode", "KEY_LANG6", SDL_SCANCODE_LANG6);
	engine->RegisterEnumValue("keycode", "KEY_LANG7", SDL_SCANCODE_LANG7);
	engine->RegisterEnumValue("keycode", "KEY_LANG8", SDL_SCANCODE_LANG8);
	engine->RegisterEnumValue("keycode", "KEY_LANG9", SDL_SCANCODE_LANG9);
	engine->RegisterEnumValue("keycode", "KEY_ALTERASE", SDL_SCANCODE_ALTERASE);
	engine->RegisterEnumValue("keycode", "KEY_SYSREQ", SDL_SCANCODE_SYSREQ);
	engine->RegisterEnumValue("keycode", "KEY_CANCEL", SDL_SCANCODE_CANCEL);
	engine->RegisterEnumValue("keycode", "KEY_CLEAR", SDL_SCANCODE_CLEAR);
	engine->RegisterEnumValue("keycode", "KEY_SDL_PRIOR", SDL_SCANCODE_PRIOR);
	engine->RegisterEnumValue("keycode", "KEY_RETURN2", SDL_SCANCODE_RETURN2);
	engine->RegisterEnumValue("keycode", "KEY_SEPARATOR", SDL_SCANCODE_SEPARATOR);
	engine->RegisterEnumValue("keycode", "KEY_OUT", SDL_SCANCODE_OUT);
	engine->RegisterEnumValue("keycode", "KEY_OPER", SDL_SCANCODE_OPER);
	engine->RegisterEnumValue("keycode", "KEY_CLEARAGAIN", SDL_SCANCODE_CLEARAGAIN);
	engine->RegisterEnumValue("keycode", "KEY_CRSEL", SDL_SCANCODE_CRSEL);
	engine->RegisterEnumValue("keycode", "KEY_EXSEL", SDL_SCANCODE_EXSEL);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_00", SDL_SCANCODE_KP_00);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_000", SDL_SCANCODE_KP_000);
	engine->RegisterEnumValue("keycode", "KEY_THOUSANDSSEPARATOR", SDL_SCANCODE_THOUSANDSSEPARATOR);
	engine->RegisterEnumValue("keycode", "KEY_DECIMALSEPARATOR", SDL_SCANCODE_DECIMALSEPARATOR);
	engine->RegisterEnumValue("keycode", "KEY_CURRENCYUNIT", SDL_SCANCODE_CURRENCYUNIT);
	engine->RegisterEnumValue("keycode", "KEY_CURRENCYSUBUNIT", SDL_SCANCODE_CURRENCYSUBUNIT);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_LEFTPAREN", SDL_SCANCODE_KP_LEFTPAREN);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_RIGHTPAREN", SDL_SCANCODE_KP_RIGHTPAREN);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_LEFTBRACE", SDL_SCANCODE_KP_LEFTBRACE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_RIGHTBRACE", SDL_SCANCODE_KP_RIGHTBRACE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_TAB", SDL_SCANCODE_KP_TAB);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_BACKSPACE", SDL_SCANCODE_KP_BACKSPACE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_A", SDL_SCANCODE_KP_A);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_B", SDL_SCANCODE_KP_B);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_C", SDL_SCANCODE_KP_C);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_D", SDL_SCANCODE_KP_D);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_E", SDL_SCANCODE_KP_E);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_F", SDL_SCANCODE_KP_F);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_XOR", SDL_SCANCODE_KP_XOR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_POWER", SDL_SCANCODE_KP_POWER);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_PERCENT", SDL_SCANCODE_KP_PERCENT);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_LESS", SDL_SCANCODE_KP_LESS);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_GREATER", SDL_SCANCODE_KP_GREATER);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_AMPERSAND", SDL_SCANCODE_KP_AMPERSAND);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_DBLAMPERSAND", SDL_SCANCODE_KP_DBLAMPERSAND);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_VERTICALBAR", SDL_SCANCODE_KP_VERTICALBAR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_DBLVERTICALBAR", SDL_SCANCODE_KP_DBLVERTICALBAR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_COLON", SDL_SCANCODE_KP_COLON);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_HASH", SDL_SCANCODE_KP_HASH);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_SPACE", SDL_SCANCODE_KP_SPACE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_AT", SDL_SCANCODE_KP_AT);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_EXCLAM", SDL_SCANCODE_KP_EXCLAM);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMSTORE", SDL_SCANCODE_KP_MEMSTORE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMRECALL", SDL_SCANCODE_KP_MEMRECALL);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMCLEAR", SDL_SCANCODE_KP_MEMCLEAR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMADD", SDL_SCANCODE_KP_MEMADD);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMSUBTRACT", SDL_SCANCODE_KP_MEMSUBTRACT);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMMULTIPLY", SDL_SCANCODE_KP_MEMMULTIPLY);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_MEMDIVIDE", SDL_SCANCODE_KP_MEMDIVIDE);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_PLUSMINUS", SDL_SCANCODE_KP_PLUSMINUS);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_CLEAR", SDL_SCANCODE_KP_CLEAR);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_CLEARENTRY", SDL_SCANCODE_KP_CLEARENTRY);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_BINARY", SDL_SCANCODE_KP_BINARY);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_OCTAL", SDL_SCANCODE_KP_OCTAL);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_DECIMAL", SDL_SCANCODE_KP_DECIMAL);
	engine->RegisterEnumValue("keycode", "KEY_NUMPAD_HEXADECIMAL", SDL_SCANCODE_KP_HEXADECIMAL);
	engine->RegisterEnumValue("keycode", "KEY_LCTRL", SDL_SCANCODE_LCTRL);
	engine->RegisterEnumValue("keycode", "KEY_LSHIFT", SDL_SCANCODE_LSHIFT);
	engine->RegisterEnumValue("keycode", "KEY_LALT", SDL_SCANCODE_LALT);
	engine->RegisterEnumValue("keycode", "KEY_LGUI", SDL_SCANCODE_LGUI);
	engine->RegisterEnumValue("keycode", "KEY_RCTRL", SDL_SCANCODE_RCTRL);
	engine->RegisterEnumValue("keycode", "KEY_RSHIFT", SDL_SCANCODE_RSHIFT);
	engine->RegisterEnumValue("keycode", "KEY_RALT", SDL_SCANCODE_RALT);
	engine->RegisterEnumValue("keycode", "KEY_RGUI", SDL_SCANCODE_RGUI);
	engine->RegisterEnumValue("keycode", "KEY_MODE", SDL_SCANCODE_MODE);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_NEXT_TRACK", SDL_SCANCODE_MEDIA_NEXT_TRACK);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_PREVIOUS_TRACK", SDL_SCANCODE_MEDIA_PREVIOUS_TRACK);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_STOP", SDL_SCANCODE_MEDIA_STOP);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_PLAY", SDL_SCANCODE_MEDIA_PLAY);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_SELECT", SDL_SCANCODE_MEDIA_SELECT);
	engine->RegisterEnumValue("keycode", "KEY_AC_SEARCH", SDL_SCANCODE_AC_SEARCH);
	engine->RegisterEnumValue("keycode", "KEY_AC_HOME", SDL_SCANCODE_AC_HOME);
	engine->RegisterEnumValue("keycode", "KEY_AC_BACK", SDL_SCANCODE_AC_BACK);
	engine->RegisterEnumValue("keycode", "KEY_AC_FORWARD", SDL_SCANCODE_AC_FORWARD);
	engine->RegisterEnumValue("keycode", "KEY_AC_STOP", SDL_SCANCODE_AC_STOP);
	engine->RegisterEnumValue("keycode", "KEY_AC_REFRESH", SDL_SCANCODE_AC_REFRESH);
	engine->RegisterEnumValue("keycode", "KEY_AC_BOOKMARKS", SDL_SCANCODE_AC_BOOKMARKS);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_EJECT", SDL_SCANCODE_MEDIA_EJECT);
	engine->RegisterEnumValue("keycode", "KEY_SLEEP", SDL_SCANCODE_SLEEP);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_REWIND", SDL_SCANCODE_MEDIA_REWIND);
	engine->RegisterEnumValue("keycode", "KEY_MEDIA_FAST_FORWARD", SDL_SCANCODE_MEDIA_FAST_FORWARD);
	engine->RegisterEnumValue("keycode", "KEY_SOFTLEFT", SDL_SCANCODE_SOFTLEFT);
	engine->RegisterEnumValue("keycode", "KEY_SOFTRIGHT", SDL_SCANCODE_SOFTRIGHT);
	engine->RegisterEnumValue("keycode", "KEY_CALL", SDL_SCANCODE_CALL);
	engine->RegisterEnumValue("keycode", "KEY_ENDCALL", SDL_SCANCODE_ENDCALL);

}
