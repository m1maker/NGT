#include "MemoryStream.h"
#include "scriptbuilder/scriptbuilder.h"
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
const int AS_SDLK_UNKNOWN = SDL_SCANCODE_UNKNOWN;
const int AS_SDLK_BACKSPACE = SDL_SCANCODE_BACKSPACE;
const int AS_SDLK_TAB = SDL_SCANCODE_TAB;
const int AS_SDLK_RETURN = SDL_SCANCODE_RETURN;
const int AS_SDLK_ESCAPE = SDL_SCANCODE_ESCAPE;
const int AS_SDLK_SPACE = SDL_SCANCODE_SPACE;
const int AS_SDLK_COMMA = SDL_SCANCODE_COMMA;
const int AS_SDLK_MINUS = SDL_SCANCODE_MINUS;
const int AS_SDLK_PERIOD = SDL_SCANCODE_PERIOD;
const int AS_SDLK_SLASH = SDL_SCANCODE_SLASH;
const int AS_SDLK_0 = SDL_SCANCODE_0;
const int AS_SDLK_1 = SDL_SCANCODE_1;
const int AS_SDLK_2 = SDL_SCANCODE_2;
const int AS_SDLK_3 = SDL_SCANCODE_3;
const int AS_SDLK_4 = SDL_SCANCODE_4;
const int AS_SDLK_5 = SDL_SCANCODE_5;
const int AS_SDLK_6 = SDL_SCANCODE_6;
const int AS_SDLK_7 = SDL_SCANCODE_7;
const int AS_SDLK_8 = SDL_SCANCODE_8;
const int AS_SDLK_9 = SDL_SCANCODE_9;
const int AS_SDLK_SEMICOLON = SDL_SCANCODE_SEMICOLON;
const int AS_SDLK_LEFTBRACKET = SDL_SCANCODE_LEFTBRACKET;
const int AS_SDLK_RIGHTBRACKET = SDL_SCANCODE_RIGHTBRACKET;
const int AS_SDLK_BACKSLASH = SDL_SCANCODE_BACKSLASH;

const int AS_SDLK_a = SDL_SCANCODE_A;
const int AS_SDLK_b = SDL_SCANCODE_B;
const int AS_SDLK_c = SDL_SCANCODE_C;
const int AS_SDLK_d = SDL_SCANCODE_D;
const int AS_SDLK_e = SDL_SCANCODE_E;
const int AS_SDLK_f = SDL_SCANCODE_F;
const int AS_SDLK_g = SDL_SCANCODE_G;
const int AS_SDLK_h = SDL_SCANCODE_H;
const int AS_SDLK_i = SDL_SCANCODE_I;
const int AS_SDLK_j = SDL_SCANCODE_J;
const int AS_SDLK_k = SDL_SCANCODE_K;
const int AS_SDLK_l = SDL_SCANCODE_L;
const int AS_SDLK_m = SDL_SCANCODE_M;
const int AS_SDLK_n = SDL_SCANCODE_N;
const int AS_SDLK_o = SDL_SCANCODE_O;
const int AS_SDLK_p = SDL_SCANCODE_P;
const int AS_SDLK_q = SDL_SCANCODE_Q;
const int AS_SDLK_r = SDL_SCANCODE_R;
const int AS_SDLK_s = SDL_SCANCODE_S;
const int AS_SDLK_t = SDL_SCANCODE_T;
const int AS_SDLK_u = SDL_SCANCODE_U;
const int AS_SDLK_v = SDL_SCANCODE_V;
const int AS_SDLK_w = SDL_SCANCODE_W;
const int AS_SDLK_x = SDL_SCANCODE_X;
const int AS_SDLK_y = SDL_SCANCODE_Y;
const int AS_SDLK_z = SDL_SCANCODE_Z;
const int AS_SDLK_DELETE = SDL_SCANCODE_DELETE;
const int AS_SDLK_CAPSLOCK = SDL_SCANCODE_CAPSLOCK;
const int AS_SDLK_F1 = SDL_SCANCODE_F1;
const int AS_SDLK_F2 = SDL_SCANCODE_F2;
const int AS_SDLK_F3 = SDL_SCANCODE_F3;
const int AS_SDLK_F4 = SDL_SCANCODE_F4;
const int AS_SDLK_F5 = SDL_SCANCODE_F5;
const int AS_SDLK_F6 = SDL_SCANCODE_F6;
const int AS_SDLK_F7 = SDL_SCANCODE_F7;
const int AS_SDLK_F8 = SDL_SCANCODE_F8;
const int AS_SDLK_F9 = SDL_SCANCODE_F9;
const int AS_SDLK_F10 = SDL_SCANCODE_F10;
const int AS_SDLK_F11 = SDL_SCANCODE_F11;
const int AS_SDLK_F12 = SDL_SCANCODE_F12;
const int AS_SDLK_PRINTSCREEN = SDL_SCANCODE_PRINTSCREEN;
const int AS_SDLK_SCROLLLOCK = SDL_SCANCODE_SCROLLLOCK;
const int AS_SDLK_PAUSE = SDL_SCANCODE_PAUSE;
const int AS_SDLK_INSERT = SDL_SCANCODE_INSERT;
const int AS_SDLK_HOME = SDL_SCANCODE_HOME;
const int AS_SDLK_PAGEUP = SDL_SCANCODE_PAGEUP;
const int AS_SDLK_END = SDL_SCANCODE_END;
const int AS_SDLK_PAGEDOWN = SDL_SCANCODE_PAGEDOWN;
const int AS_SDLK_RIGHT = SDL_SCANCODE_RIGHT;
const int AS_SDLK_LEFT = SDL_SCANCODE_LEFT;
const int AS_SDLK_DOWN = SDL_SCANCODE_DOWN;
const int AS_SDLK_UP = SDL_SCANCODE_UP;
const int AS_SDLK_NUMLOCKCLEAR = SDL_SCANCODE_NUMLOCKCLEAR;
const int AS_SDLK_KP_DIVIDE = SDL_SCANCODE_KP_DIVIDE;
const int AS_SDLK_KP_MULTIPLY = SDL_SCANCODE_KP_MULTIPLY;
const int AS_SDLK_KP_MINUS = SDL_SCANCODE_KP_MINUS;
const int AS_SDLK_KP_PLUS = SDL_SCANCODE_KP_PLUS;
const int AS_SDLK_KP_ENTER = SDL_SCANCODE_KP_ENTER;
const int AS_SDLK_KP_1 = SDL_SCANCODE_KP_1;
const int AS_SDLK_KP_2 = SDL_SCANCODE_KP_2;
const int AS_SDLK_KP_3 = SDL_SCANCODE_KP_3;
const int AS_SDLK_KP_4 = SDL_SCANCODE_KP_4;
const int AS_SDLK_KP_5 = SDL_SCANCODE_KP_5;
const int AS_SDLK_KP_6 = SDL_SCANCODE_KP_6;
const int AS_SDLK_KP_7 = SDL_SCANCODE_KP_7;
const int AS_SDLK_KP_8 = SDL_SCANCODE_KP_8;
const int AS_SDLK_KP_9 = SDL_SCANCODE_KP_9;
const int AS_SDLK_KP_0 = SDL_SCANCODE_KP_0;
const int AS_SDLK_KP_PERIOD = SDL_SCANCODE_KP_PERIOD;
const int AS_SDLK_APPLICATION = SDL_SCANCODE_APPLICATION;
const int AS_SDLK_POWER = SDL_SCANCODE_POWER;
const int AS_SDLK_KP_EQUALS = SDL_SCANCODE_KP_EQUALS;
const int AS_SDLK_F13 = SDL_SCANCODE_F13;
const int AS_SDLK_F14 = SDL_SCANCODE_F14;
const int AS_SDLK_F15 = SDL_SCANCODE_F15;
const int AS_SDLK_F16 = SDL_SCANCODE_F16;
const int AS_SDLK_F17 = SDL_SCANCODE_F17;
const int AS_SDLK_F18 = SDL_SCANCODE_F18;
const int AS_SDLK_F19 = SDL_SCANCODE_F19;
const int AS_SDLK_F20 = SDL_SCANCODE_F20;
const int AS_SDLK_F21 = SDL_SCANCODE_F21;
const int AS_SDLK_F22 = SDL_SCANCODE_F22;
const int AS_SDLK_F23 = SDL_SCANCODE_F23;
const int AS_SDLK_F24 = SDL_SCANCODE_F24;
const int AS_SDLK_EXECUTE = SDL_SCANCODE_EXECUTE;
const int AS_SDLK_HELP = SDL_SCANCODE_HELP;
const int AS_SDLK_MENU = SDL_SCANCODE_MENU;
const int AS_SDLK_SELECT = SDL_SCANCODE_SELECT;
const int AS_SDLK_STOP = SDL_SCANCODE_STOP;
const int AS_SDLK_AGAIN = SDL_SCANCODE_AGAIN;
const int AS_SDLK_UNDO = SDL_SCANCODE_UNDO;
const int AS_SDLK_CUT = SDL_SCANCODE_CUT;
const int AS_SDLK_COPY = SDL_SCANCODE_COPY;
const int AS_SDLK_PASTE = SDL_SCANCODE_PASTE;
const int AS_SDLK_FIND = SDL_SCANCODE_FIND;
const int AS_SDLK_MUTE = SDL_SCANCODE_MUTE;
const int AS_SDLK_VOLUMEUP = SDL_SCANCODE_VOLUMEUP;
const int AS_SDLK_VOLUMEDOWN = SDL_SCANCODE_VOLUMEDOWN;
const int AS_SDLK_KP_COMMA = SDL_SCANCODE_KP_COMMA;
const int AS_SDLK_KP_EQUALSAS400 = SDL_SCANCODE_KP_EQUALSAS400;
const int AS_SDLK_ALTERASE = SDL_SCANCODE_ALTERASE;
const int AS_SDLK_SYSREQ = SDL_SCANCODE_SYSREQ;
const int AS_SDLK_CANCEL = SDL_SCANCODE_CANCEL;
const int AS_SDLK_CLEAR = SDL_SCANCODE_CLEAR;
const int AS_SDLK_PRIOR = SDL_SCANCODE_PRIOR;
const int AS_SDLK_RETURN2 = SDL_SCANCODE_RETURN2;
const int AS_SDLK_SEPARATOR = SDL_SCANCODE_SEPARATOR;
const int AS_SDLK_OUT = SDL_SCANCODE_OUT;
const int AS_SDLK_OPER = SDL_SCANCODE_OPER;
const int AS_SDLK_CLEARAGAIN = SDL_SCANCODE_CLEARAGAIN;
const int AS_SDLK_CRSEL = SDL_SCANCODE_CRSEL;
const int AS_SDLK_EXSEL = SDL_SCANCODE_EXSEL;
const int AS_SDLK_KP_00 = SDL_SCANCODE_KP_00;
const int AS_SDLK_KP_000 = SDL_SCANCODE_KP_000;
const int AS_SDLK_THOUSANDSSEPARATOR = SDL_SCANCODE_THOUSANDSSEPARATOR;
const int AS_SDLK_DECIMALSEPARATOR = SDL_SCANCODE_DECIMALSEPARATOR;
const int AS_SDLK_CURRENCYUNIT = SDL_SCANCODE_CURRENCYUNIT;
const int AS_SDLK_CURRENCYSUBUNIT = SDL_SCANCODE_CURRENCYSUBUNIT;
const int AS_SDLK_KP_LEFTPAREN = SDL_SCANCODE_KP_LEFTPAREN;
const int AS_SDLK_KP_RIGHTPAREN = SDL_SCANCODE_KP_RIGHTPAREN;
const int AS_SDLK_KP_LEFTBRACE = SDL_SCANCODE_KP_LEFTBRACE;
const int AS_SDLK_KP_RIGHTBRACE = SDL_SCANCODE_KP_RIGHTBRACE;
const int AS_SDLK_KP_TAB = SDL_SCANCODE_KP_TAB;
const int AS_SDLK_KP_BACKSPACE = SDL_SCANCODE_KP_BACKSPACE;
const int AS_SDLK_KP_A = SDL_SCANCODE_KP_A;
const int AS_SDLK_KP_B = SDL_SCANCODE_KP_B;
const int AS_SDLK_KP_C = SDL_SCANCODE_KP_C;
const int AS_SDLK_KP_D = SDL_SCANCODE_KP_D;
const int AS_SDLK_KP_E = SDL_SCANCODE_KP_E;
const int AS_SDLK_KP_F = SDL_SCANCODE_KP_F;
const int AS_SDLK_KP_XOR = SDL_SCANCODE_KP_XOR;
const int AS_SDLK_KP_POWER = SDL_SCANCODE_KP_POWER;
const int AS_SDLK_KP_PERCENT = SDL_SCANCODE_KP_PERCENT;
const int AS_SDLK_KP_LESS = SDL_SCANCODE_KP_LESS;
const int AS_SDLK_KP_GREATER = SDL_SCANCODE_KP_GREATER;
const int AS_SDLK_KP_AMPERSAND = SDL_SCANCODE_KP_AMPERSAND;
const int AS_SDLK_KP_DBLAMPERSAND = SDL_SCANCODE_KP_DBLAMPERSAND;
const int AS_SDLK_KP_VERTICALBAR = SDL_SCANCODE_KP_VERTICALBAR;
const int AS_SDLK_KP_DBLVERTICALBAR = SDL_SCANCODE_KP_DBLVERTICALBAR;
const int AS_SDLK_KP_COLON = SDL_SCANCODE_KP_COLON;
const int AS_SDLK_KP_HASH = SDL_SCANCODE_KP_HASH;
const int AS_SDLK_KP_SPACE = SDL_SCANCODE_KP_SPACE;
const int AS_SDLK_KP_AT = SDL_SCANCODE_KP_AT;
const int AS_SDLK_KP_EXCLAM = SDL_SCANCODE_KP_EXCLAM;
const int AS_SDLK_KP_MEMSTORE = SDL_SCANCODE_KP_MEMSTORE;
const int AS_SDLK_KP_MEMRECALL = SDL_SCANCODE_KP_MEMRECALL;
const int AS_SDLK_KP_MEMCLEAR = SDL_SCANCODE_KP_MEMCLEAR;
const int AS_SDLK_KP_MEMADD = SDL_SCANCODE_KP_MEMADD;
const int AS_SDLK_KP_MEMSUBTRACT = SDL_SCANCODE_KP_MEMSUBTRACT;
const int AS_SDLK_KP_MEMMULTIPLY = SDL_SCANCODE_KP_MEMMULTIPLY;
const int AS_SDLK_KP_MEMDIVIDE = SDL_SCANCODE_KP_MEMDIVIDE;
const int AS_SDLK_KP_PLUSMINUS = SDL_SCANCODE_KP_PLUSMINUS;
const int AS_SDLK_KP_CLEAR = SDL_SCANCODE_KP_CLEAR;
const int AS_SDLK_KP_CLEARENTRY = SDL_SCANCODE_KP_CLEARENTRY;
const int AS_SDLK_KP_BINARY = SDL_SCANCODE_KP_BINARY;
const int AS_SDLK_KP_OCTAL = SDL_SCANCODE_KP_OCTAL;
const int AS_SDLK_KP_DECIMAL = SDL_SCANCODE_KP_DECIMAL;
const int AS_SDLK_KP_HEXADECIMAL = SDL_SCANCODE_KP_HEXADECIMAL;
const int AS_SDLK_LCTRL = SDL_SCANCODE_LCTRL;
const int AS_SDLK_LSHIFT = SDL_SCANCODE_LSHIFT;
const int AS_SDLK_LALT = SDL_SCANCODE_LALT;
const int AS_SDLK_LGUI = SDL_SCANCODE_LGUI;
const int AS_SDLK_RCTRL = SDL_SCANCODE_RCTRL;
const int AS_SDLK_RSHIFT = SDL_SCANCODE_RSHIFT;
const int AS_SDLK_RALT = SDL_SCANCODE_RALT;
const int AS_SDLK_RGUI = SDL_SCANCODE_RGUI;
const int AS_SDLK_MODE = SDL_SCANCODE_MODE;
const int AS_SDLK_AUDIONEXT = SDL_SCANCODE_MEDIA_NEXT_TRACK;
const int AS_SDLK_AUDIOPREV = SDL_SCANCODE_MEDIA_PREVIOUS_TRACK;
const int AS_SDLK_AUDIOSTOP = SDL_SCANCODE_MEDIA_STOP;
const int AS_SDLK_AUDIOPLAY = SDL_SCANCODE_MEDIA_PLAY;
const int AS_SDLK_AUDIOMUTE = SDL_SCANCODE_MEDIA_EJECT;
const int AS_SDLK_MEDIASELECT = SDL_SCANCODE_MEDIA_SELECT;
const int AS_SDLK_AC_SEARCH = SDL_SCANCODE_AC_SEARCH;
const int AS_SDLK_AC_HOME = SDL_SCANCODE_AC_HOME;
const int AS_SDLK_AC_BACK = SDL_SCANCODE_AC_BACK;
const int AS_SDLK_AC_FORWARD = SDL_SCANCODE_AC_FORWARD;
const int AS_SDLK_AC_STOP = SDL_SCANCODE_AC_STOP;
const int AS_SDLK_AC_REFRESH = SDL_SCANCODE_AC_REFRESH;
const int AS_SDLK_AC_BOOKMARKS = SDL_SCANCODE_AC_BOOKMARKS;
const int AS_SDLK_SLEEP = SDL_SCANCODE_SLEEP;
const int AS_MESSAGEBOX_ERROR = SDL_MESSAGEBOX_ERROR;
const int AS_MESSAGEBOX_WARN = SDL_MESSAGEBOX_WARNING;
const int AS_MESSAGEBOX_INFO = SDL_MESSAGEBOX_INFORMATION;
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
std::wstring ouou;
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
	std::wstring oo;
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

}
#endif
timer* ftimer() { return new timer; }
user_idle* fuser_idle() { return new user_idle; }
asIScriptFunction* fscript_function() { asIScriptFunction* r = nullptr; return r; }
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
void MessageCallback(const asSMessageInfo* msg, void* param)
{
	const char* type = "ERR ";
	if (msg->type == asMSGTYPE_WARNING)
		return;
	else if (msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";
	char rowStr[10], colStr[10];
	_itoa_s(msg->row, rowStr, 10);
	_itoa_s(msg->col, colStr, 10);
	std::string messageStr(msg->message);
	std::string output = "File: " + std::string(msg->section) + "\r\nLine (" + rowStr + ", " + colStr + ") : \r\n" + type + " : " + messageStr;
#ifdef _WIN32
	ouou = wstr(output);
	result_message += ouou + L"\r\n";
#endif
	cout << output << endl;
}


void Print(const char* format, ...)
{

	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
SDL_Rect* frect() { return new SDL_Rect; }
void RegisterFunctions(asIScriptEngine* engine)
{
	engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
	engine->RegisterTypedef("long", "int64");
	engine->RegisterTypedef("ulong", "uint64");
	engine->RegisterTypedef("short", "int8");
	engine->RegisterTypedef("ushort", "uint8");
	engine->RegisterTypedef("dword", "uint64");
	engine->RegisterTypedef("word", "uint8");
	engine->RegisterTypedef("size_t", "uint64");
	engine->RegisterTypedef("c_str", "int8");
	engine->RegisterTypedef("hwnd", "uint8");
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
	engine->RegisterGlobalFunction("int get_cpu_count()property", asFUNCTION(get_cpu_count), asCALL_CDECL);
	engine->RegisterGlobalFunction("int get_system_ram()property", asFUNCTION(get_system_ram), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_platform()property", asFUNCTION(get_platform), asCALL_CDECL);

	engine->RegisterGlobalFunction("uint64 get_time_stamp_millis()property", asFUNCTION(get_time_stamp_millis), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 get_time_stamp_seconds()property", asFUNCTION(get_time_stamp_seconds), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_library_path(const string &in)", asFUNCTION(set_library_path), asCALL_CDECL);

	engine->RegisterGlobalFunction("long random(long, long)", asFUNCTIONPR(random, (long, long), long), asCALL_CDECL);
	engine->RegisterGlobalFunction("double random(double, double)", asFUNCTIONPR(randomDouble, (double, double), double), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool random_bool()", asFUNCTION(random_bool), asCALL_CDECL);
	engine->RegisterGlobalFunction("void printf(string &in, ?&in var = 0, ?&in var2 = 0, ?&in var3 = 0, ?&in var4 = 0, ?&in var5 = 0, ?&in var6 = 0, ?&in var7 = 0, ?&in var8 = 0, ?&in var9 = 0, ?&in var10 = 0, ?&in var11 = 0, ?&in var12 = 0, ?&in var13 = 0, ?&in var14 = 0, ?&in var15 = 0)", asFUNCTION(as_printf), asCALL_GENERIC);

	engine->RegisterGlobalFunction("int get_last_error()property", asFUNCTION(get_last_error), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_screen_reader_interrupt(bool)property", asFUNCTION(set_screen_reader_interrupt), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_screen_reader_interrupt()property", asFUNCTION(get_screen_reader_interrupt), asCALL_CDECL);

	engine->RegisterGlobalFunction("void speak(const string &in, bool=screen_reader_interrupt)", asFUNCTION(speak), asCALL_CDECL);
	engine->RegisterGlobalFunction("void speak_wait(const string &in, bool=screen_reader_interrupt)", asFUNCTION(speak_wait), asCALL_CDECL);

	engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
	engine->RegisterGlobalFunction("string screen_reader_detect()", asFUNCTION(screen_reader_detect), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_exit_callback(exit_callback@=null)", asFUNCTION(set_exit_callback), asCALL_CDECL);

	engine->RegisterGlobalFunction("void show_console()", asFUNCTION(show_console), asCALL_CDECL);
	engine->RegisterGlobalFunction("void hide_console()", asFUNCTION(hide_console), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool show_window(const string &in,int=640,int=480, bool=true)", asFUNCTION(show_window), asCALL_CDECL);
	//	engine->RegisterGlobalFunction("bool focus_window()", asFUNCTION(focus_window), asCALL_CDECL);
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

	engine->RegisterGlobalFunction("void exit(int=0)", asFUNCTION(exit_engine), asCALL_CDECL);
	engine->RegisterGlobalFunction("string number_to_words(uint64, bool)", asFUNCTION(number_to_words), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool clipboard_copy_text(const string &in)", asFUNCTION(clipboard_copy_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string clipboard_read_text()", asFUNCTION(clipboard_read_text), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_input()", asFUNCTION(get_input), asCALL_CDECL);
	engine->RegisterGlobalFunction("string input_box(const string &in, const string &in, const string &in=\"\", bool=false)", asFUNCTION(input_box), asCALL_CDECL);

	engine->RegisterTypedef("keycode", "int");
	engine->RegisterGlobalFunction("bool key_pressed(keycode)", asFUNCTION(key_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_released(keycode)", asFUNCTION(key_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_down(keycode)", asFUNCTION(key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool key_repeat(keycode)", asFUNCTION(key_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("keycode[]@ keys_pressed()", asFUNCTION(keys_pressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("keycode[]@ keys_released()", asFUNCTION(keys_released), asCALL_CDECL);
	engine->RegisterGlobalFunction("keycode[]@ keys_down()", asFUNCTION(keys_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("keycode[]@ keys_repeat()", asFUNCTION(keys_repeat), asCALL_CDECL);
	engine->RegisterGlobalFunction("string key_to_string(keycode)", asFUNCTION(key_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("keycode string_to_key(const string&in)", asFUNCTION(string_to_key), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool force_key_down(keycode)", asFUNCTION(force_key_down), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool force_key_up(keycode)", asFUNCTION(force_key_up), asCALL_CDECL);
	engine->RegisterGlobalFunction("void reset_all_forced_keys()", asFUNCTION(reset_all_forced_keys), asCALL_CDECL);

	engine->RegisterGlobalFunction("string string_encrypt(string, string)", asFUNCTION(string_encrypt), asCALL_CDECL);
	engine->RegisterGlobalFunction("string string_decrypt(string, string)", asFUNCTION(string_decrypt), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_decode(const string &in)", asFUNCTION(url_decode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_encode(const string &in)", asFUNCTION(url_encode), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_get(const string &in)", asFUNCTION(url_get), asCALL_CDECL);
	engine->RegisterGlobalFunction("string url_post(const string &in, const string &in)", asFUNCTION(url_post), asCALL_CDECL);
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
	engine->RegisterGlobalFunction("string read_environment_variable(const string&in)", asFUNCTION(read_environment_variable), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool write_environment_variable(const string&in, const string &in)", asFUNCTION(write_environment_variable), asCALL_CDECL);

	engine->RegisterGlobalFunction("string serialize(dictionary@=null)", asFUNCTION(serialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("dictionary@ deserialize(const string &in)", asFUNCTION(deserialize), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool urlopen(const string &in)", asFUNCTION(urlopen), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 malloc(uint64)", asFUNCTION(malloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 calloc(uint64, uint64)", asFUNCTION(calloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 realloc(uint64, uint64)", asFUNCTION(realloc), asCALL_CDECL);
	engine->RegisterGlobalFunction("void free(uint64)", asFUNCTION(free), asCALL_CDECL);
	engine->RegisterGlobalFunction("string c_str_to_string(uint64, size_t=0)", asFUNCTION(c_str_to_string), asCALL_CDECL);
	engine->RegisterGlobalFunction("size_t c_str_len(uint64)", asFUNCTION(strlen), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 c_str_to_wc_str(uint64)", asFUNCTION(c_str_to_wc_str), asCALL_CDECL);
	engine->RegisterGlobalFunction("uint64 wc_str_to_c_str(uint64)", asFUNCTION(wc_str_to_c_str), asCALL_CDECL);
	register_pack(engine);
	register_sound(engine);
	RegisterMemstream(engine);
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
	engine->RegisterObjectType("script_function", sizeof(asIScriptFunction), asOBJ_REF);
	engine->RegisterObjectBehaviour("script_function", asBEHAVE_FACTORY, "script_function@ asIScriptFunction()", asFUNCTION(fscript_function), asCALL_CDECL);
	engine->RegisterObjectBehaviour("script_function", asBEHAVE_ADDREF, "void f()", asMETHOD(asIScriptFunction, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("script_function", asBEHAVE_RELEASE, "void f()", asMETHOD(asIScriptFunction, Release), asCALL_THISCALL);
	engine->RegisterObjectType("library", sizeof(library), asOBJ_REF);
	engine->RegisterObjectBehaviour("library", asBEHAVE_FACTORY, "library@ l()", asFUNCTION(flibrary), asCALL_CDECL);
	engine->RegisterObjectBehaviour("library", asBEHAVE_ADDREF, "void f()", asMETHOD(library, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("library", asBEHAVE_RELEASE, "void f()", asMETHOD(library, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "bool load(const string&in)const", asMETHOD(library, load), asCALL_THISCALL);
	engine->RegisterObjectMethod("library", "bool get_active()const property", asMETHOD(library, active), asCALL_THISCALL);

	engine->RegisterObjectMethod("library", "dictionary@ call(const string&in, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null)const", asFUNCTION(library_call), asCALL_GENERIC);
	engine->RegisterObjectMethod("library", "void unload()const", asMETHOD(library, unload), asCALL_THISCALL);
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

	engine->RegisterGlobalProperty("const int SDLK_UNKNOWN", (void*)&AS_SDLK_UNKNOWN);
	engine->RegisterGlobalProperty("const int SDLK_BACKSPACE", (void*)&AS_SDLK_BACKSPACE);
	engine->RegisterGlobalProperty("const int SDLK_TAB", (void*)&AS_SDLK_TAB);
	engine->RegisterGlobalProperty("const int SDLK_RETURN", (void*)&AS_SDLK_RETURN);
	engine->RegisterGlobalProperty("const int SDLK_ESCAPE", (void*)&AS_SDLK_ESCAPE);
	engine->RegisterGlobalProperty("const int SDLK_SPACE", (void*)&AS_SDLK_SPACE);
	engine->RegisterGlobalProperty("const int SDLK_COMMA", (void*)&AS_SDLK_COMMA);
	engine->RegisterGlobalProperty("const int SDLK_MINUS", (void*)&AS_SDLK_MINUS);
	engine->RegisterGlobalProperty("const int SDLK_PERIOD", (void*)&AS_SDLK_PERIOD);
	engine->RegisterGlobalProperty("const int SDLK_SLASH", (void*)&AS_SDLK_SLASH);
	engine->RegisterGlobalProperty("const int SDLK_0", (void*)&AS_SDLK_0);
	engine->RegisterGlobalProperty("const int SDLK_1", (void*)&AS_SDLK_1);
	engine->RegisterGlobalProperty("const int SDLK_2", (void*)&AS_SDLK_2);
	engine->RegisterGlobalProperty("const int SDLK_3", (void*)&AS_SDLK_3);
	engine->RegisterGlobalProperty("const int SDLK_4", (void*)&AS_SDLK_4);
	engine->RegisterGlobalProperty("const int SDLK_5", (void*)&AS_SDLK_5);
	engine->RegisterGlobalProperty("const int SDLK_6", (void*)&AS_SDLK_6);
	engine->RegisterGlobalProperty("const int SDLK_7", (void*)&AS_SDLK_7);
	engine->RegisterGlobalProperty("const int SDLK_8", (void*)&AS_SDLK_8);
	engine->RegisterGlobalProperty("const int SDLK_9", (void*)&AS_SDLK_9);
	engine->RegisterGlobalProperty("const int SDLK_SEMICOLON", (void*)&AS_SDLK_SEMICOLON);
	engine->RegisterGlobalProperty("const int SDLK_LEFTBRACKET", (void*)&AS_SDLK_LEFTBRACKET);
	engine->RegisterGlobalProperty("const int SDLK_RIGHTBRACKET", (void*)&AS_SDLK_RIGHTBRACKET);
	engine->RegisterGlobalProperty("const int SDLK_BACKSLASH", (void*)&AS_SDLK_BACKSLASH);
	engine->RegisterGlobalProperty("const int SDLK_a", (void*)&AS_SDLK_a);
	engine->RegisterGlobalProperty("const int SDLK_b", (void*)&AS_SDLK_b);
	engine->RegisterGlobalProperty("const int SDLK_c", (void*)&AS_SDLK_c);
	engine->RegisterGlobalProperty("const int SDLK_d", (void*)&AS_SDLK_d);
	engine->RegisterGlobalProperty("const int SDLK_e", (void*)&AS_SDLK_e);
	engine->RegisterGlobalProperty("const int SDLK_f", (void*)&AS_SDLK_f);
	engine->RegisterGlobalProperty("const int SDLK_g", (void*)&AS_SDLK_g);
	engine->RegisterGlobalProperty("const int SDLK_h", (void*)&AS_SDLK_h);
	engine->RegisterGlobalProperty("const int SDLK_i", (void*)&AS_SDLK_i);
	engine->RegisterGlobalProperty("const int SDLK_j", (void*)&AS_SDLK_j);
	engine->RegisterGlobalProperty("const int SDLK_k", (void*)&AS_SDLK_k);
	engine->RegisterGlobalProperty("const int SDLK_l", (void*)&AS_SDLK_l);
	engine->RegisterGlobalProperty("const int SDLK_m", (void*)&AS_SDLK_m);
	engine->RegisterGlobalProperty("const int SDLK_n", (void*)&AS_SDLK_n);
	engine->RegisterGlobalProperty("const int SDLK_o", (void*)&AS_SDLK_o);
	engine->RegisterGlobalProperty("const int SDLK_p", (void*)&AS_SDLK_p);
	engine->RegisterGlobalProperty("const int SDLK_q", (void*)&AS_SDLK_q);
	engine->RegisterGlobalProperty("const int SDLK_r", (void*)&AS_SDLK_r);
	engine->RegisterGlobalProperty("const int SDLK_s", (void*)&AS_SDLK_s);
	engine->RegisterGlobalProperty("const int SDLK_t", (void*)&AS_SDLK_t);
	engine->RegisterGlobalProperty("const int SDLK_u", (void*)&AS_SDLK_u);
	engine->RegisterGlobalProperty("const int SDLK_v", (void*)&AS_SDLK_v);
	engine->RegisterGlobalProperty("const int SDLK_w", (void*)&AS_SDLK_w);
	engine->RegisterGlobalProperty("const int SDLK_x", (void*)&AS_SDLK_x);
	engine->RegisterGlobalProperty("const int SDLK_y", (void*)&AS_SDLK_y);
	engine->RegisterGlobalProperty("const int SDLK_z", (void*)&AS_SDLK_z);
	engine->RegisterGlobalProperty("const int SDLK_DELETE", (void*)&AS_SDLK_DELETE);
	engine->RegisterGlobalProperty("const int SDLK_CAPSLOCK", (void*)&AS_SDLK_CAPSLOCK);
	engine->RegisterGlobalProperty("const int SDLK_F1", (void*)&AS_SDLK_F1);
	engine->RegisterGlobalProperty("const int SDLK_F2", (void*)&AS_SDLK_F2);
	engine->RegisterGlobalProperty("const int SDLK_F3", (void*)&AS_SDLK_F3);
	engine->RegisterGlobalProperty("const int SDLK_F4", (void*)&AS_SDLK_F4);
	engine->RegisterGlobalProperty("const int SDLK_F5", (void*)&AS_SDLK_F5);
	engine->RegisterGlobalProperty("const int SDLK_F6", (void*)&AS_SDLK_F6);
	engine->RegisterGlobalProperty("const int SDLK_F7", (void*)&AS_SDLK_F7);
	engine->RegisterGlobalProperty("const int SDLK_F8", (void*)&AS_SDLK_F8);
	engine->RegisterGlobalProperty("const int SDLK_F9", (void*)&AS_SDLK_F9);
	engine->RegisterGlobalProperty("const int SDLK_F10", (void*)&AS_SDLK_F10);
	engine->RegisterGlobalProperty("const int SDLK_F11", (void*)&AS_SDLK_F11);
	engine->RegisterGlobalProperty("const int SDLK_F12", (void*)&AS_SDLK_F12);
	engine->RegisterGlobalProperty("const int SDLK_PRINTSCREEN", (void*)&AS_SDLK_PRINTSCREEN);
	engine->RegisterGlobalProperty("const int SDLK_SCROLLLOCK", (void*)&AS_SDLK_SCROLLLOCK);
	engine->RegisterGlobalProperty("const int SDLK_PAUSE", (void*)&AS_SDLK_PAUSE);
	engine->RegisterGlobalProperty("const int SDLK_INSERT", (void*)&AS_SDLK_INSERT);
	engine->RegisterGlobalProperty("const int SDLK_HOME", (void*)&AS_SDLK_HOME);
	engine->RegisterGlobalProperty("const int SDLK_PAGEUP", (void*)&AS_SDLK_PAGEUP);
	engine->RegisterGlobalProperty("const int SDLK_END", (void*)&AS_SDLK_END);
	engine->RegisterGlobalProperty("const int SDLK_PAGEDOWN", (void*)&AS_SDLK_PAGEDOWN);
	engine->RegisterGlobalProperty("const int SDLK_RIGHT", (void*)&AS_SDLK_RIGHT);
	engine->RegisterGlobalProperty("const int SDLK_LEFT", (void*)&AS_SDLK_LEFT);
	engine->RegisterGlobalProperty("const int SDLK_DOWN", (void*)&AS_SDLK_DOWN);
	engine->RegisterGlobalProperty("const int SDLK_UP", (void*)&AS_SDLK_UP);
	engine->RegisterGlobalProperty("const int SDLK_NUMLOCKCLEAR", (void*)&AS_SDLK_NUMLOCKCLEAR);
	engine->RegisterGlobalProperty("const int SDLK_KP_DIVIDE", (void*)&AS_SDLK_KP_DIVIDE);
	engine->RegisterGlobalProperty("const int SDLK_KP_MULTIPLY", (void*)&AS_SDLK_KP_MULTIPLY);
	engine->RegisterGlobalProperty("const int SDLK_KP_MINUS", (void*)&AS_SDLK_KP_MINUS);
	engine->RegisterGlobalProperty("const int SDLK_KP_PLUS", (void*)&AS_SDLK_KP_PLUS);
	engine->RegisterGlobalProperty("const int SDLK_KP_ENTER", (void*)&AS_SDLK_KP_ENTER);
	engine->RegisterGlobalProperty("const int SDLK_KP_1", (void*)&AS_SDLK_KP_1);
	engine->RegisterGlobalProperty("const int SDLK_KP_2", (void*)&AS_SDLK_KP_2);
	engine->RegisterGlobalProperty("const int SDLK_KP_3", (void*)&AS_SDLK_KP_3);
	engine->RegisterGlobalProperty("const int SDLK_KP_4", (void*)&AS_SDLK_KP_4);
	engine->RegisterGlobalProperty("const int SDLK_KP_5", (void*)&AS_SDLK_KP_5);
	engine->RegisterGlobalProperty("const int SDLK_KP_6", (void*)&AS_SDLK_KP_6);
	engine->RegisterGlobalProperty("const int SDLK_KP_7", (void*)&AS_SDLK_KP_7);
	engine->RegisterGlobalProperty("const int SDLK_KP_8", (void*)&AS_SDLK_KP_8);
	engine->RegisterGlobalProperty("const int SDLK_KP_9", (void*)&AS_SDLK_KP_9);
	engine->RegisterGlobalProperty("const int SDLK_KP_0", (void*)&AS_SDLK_KP_0);
	engine->RegisterGlobalProperty("const int SDLK_KP_PERIOD", (void*)&AS_SDLK_KP_PERIOD);
	engine->RegisterGlobalProperty("const int SDLK_APPLICATION", (void*)&AS_SDLK_APPLICATION);
	engine->RegisterGlobalProperty("const int SDLK_POWER", (void*)&AS_SDLK_POWER);
	engine->RegisterGlobalProperty("const int SDLK_KP_EQUALS", (void*)&AS_SDLK_KP_EQUALS);
	engine->RegisterGlobalProperty("const int SDLK_F13", (void*)&AS_SDLK_F13);
	engine->RegisterGlobalProperty("const int SDLK_F14", (void*)&AS_SDLK_F14);
	engine->RegisterGlobalProperty("const int SDLK_F15", (void*)&AS_SDLK_F15);
	engine->RegisterGlobalProperty("const int SDLK_F16", (void*)&AS_SDLK_F16);
	engine->RegisterGlobalProperty("const int SDLK_F17", (void*)&AS_SDLK_F17);
	engine->RegisterGlobalProperty("const int SDLK_F18", (void*)&AS_SDLK_F18);
	engine->RegisterGlobalProperty("const int SDLK_F19", (void*)&AS_SDLK_F19);
	engine->RegisterGlobalProperty("const int SDLK_F20", (void*)&AS_SDLK_F20);
	engine->RegisterGlobalProperty("const int SDLK_F21", (void*)&AS_SDLK_F21);
	engine->RegisterGlobalProperty("const int SDLK_F22", (void*)&AS_SDLK_F22);
	engine->RegisterGlobalProperty("const int SDLK_F23", (void*)&AS_SDLK_F23);
	engine->RegisterGlobalProperty("const int SDLK_F24", (void*)&AS_SDLK_F24);
	engine->RegisterGlobalProperty("const int SDLK_EXECUTE", (void*)&AS_SDLK_EXECUTE);
	engine->RegisterGlobalProperty("const int SDLK_HELP", (void*)&AS_SDLK_HELP);
	engine->RegisterGlobalProperty("const int SDLK_MENU", (void*)&AS_SDLK_MENU);
	engine->RegisterGlobalProperty("const int SDLK_SELECT", (void*)&AS_SDLK_SELECT);
	engine->RegisterGlobalProperty("const int SDLK_STOP", (void*)&AS_SDLK_STOP);
	engine->RegisterGlobalProperty("const int SDLK_AGAIN", (void*)&AS_SDLK_AGAIN);
	engine->RegisterGlobalProperty("const int SDLK_UNDO", (void*)&AS_SDLK_UNDO);
	engine->RegisterGlobalProperty("const int SDLK_CUT", (void*)&AS_SDLK_CUT);
	engine->RegisterGlobalProperty("const int SDLK_COPY", (void*)&AS_SDLK_COPY);
	engine->RegisterGlobalProperty("const int SDLK_PASTE", (void*)&AS_SDLK_PASTE);
	engine->RegisterGlobalProperty("const int SDLK_FIND", (void*)&AS_SDLK_FIND);
	engine->RegisterGlobalProperty("const int SDLK_MUTE", (void*)&AS_SDLK_MUTE);
	engine->RegisterGlobalProperty("const int SDLK_VOLUMEUP", (void*)&AS_SDLK_VOLUMEUP);
	engine->RegisterGlobalProperty("const int SDLK_VOLUMEDOWN", (void*)&AS_SDLK_VOLUMEDOWN);
	engine->RegisterGlobalProperty("const int SDLK_KP_COMMA", (void*)&AS_SDLK_KP_COMMA);
	engine->RegisterGlobalProperty("const int SDLK_KP_EQUALSAS400", (void*)&AS_SDLK_KP_EQUALSAS400);
	engine->RegisterGlobalProperty("const int SDLK_ALTERASE", (void*)&AS_SDLK_ALTERASE);
	engine->RegisterGlobalProperty("const int SDLK_SYSREQ", (void*)&AS_SDLK_SYSREQ);
	engine->RegisterGlobalProperty("const int SDLK_CANCEL", (void*)&AS_SDLK_CANCEL);
	engine->RegisterGlobalProperty("const int SDLK_CLEAR", (void*)&AS_SDLK_CLEAR);
	engine->RegisterGlobalProperty("const int SDLK_PRIOR", (void*)&AS_SDLK_PRIOR);
	engine->RegisterGlobalProperty("const int SDLK_RETURN2", (void*)&AS_SDLK_RETURN2);
	engine->RegisterGlobalProperty("const int SDLK_SEPARATOR", (void*)&AS_SDLK_SEPARATOR);
	engine->RegisterGlobalProperty("const int SDLK_OUT", (void*)&AS_SDLK_OUT);
	engine->RegisterGlobalProperty("const int SDLK_OPER", (void*)&AS_SDLK_OPER);
	engine->RegisterGlobalProperty("const int SDLK_CLEARAGAIN", (void*)&AS_SDLK_CLEARAGAIN);
	engine->RegisterGlobalProperty("const int SDLK_CRSEL", (void*)&AS_SDLK_CRSEL);
	engine->RegisterGlobalProperty("const int SDLK_EXSEL", (void*)&AS_SDLK_EXSEL);
	engine->RegisterGlobalProperty("const int SDLK_KP_00", (void*)&AS_SDLK_KP_00);
	engine->RegisterGlobalProperty("const int SDLK_KP_000", (void*)&AS_SDLK_KP_000);
	engine->RegisterGlobalProperty("const int SDLK_THOUSANDSSEPARATOR", (void*)&AS_SDLK_THOUSANDSSEPARATOR);
	engine->RegisterGlobalProperty("const int SDLK_DECIMALSEPARATOR", (void*)&AS_SDLK_DECIMALSEPARATOR);
	engine->RegisterGlobalProperty("const int SDLK_CURRENCYUNIT", (void*)&AS_SDLK_CURRENCYUNIT);
	engine->RegisterGlobalProperty("const int SDLK_CURRENCYSUBUNIT", (void*)&AS_SDLK_CURRENCYSUBUNIT);
	engine->RegisterGlobalProperty("const int SDLK_KP_LEFTPAREN", (void*)&AS_SDLK_KP_LEFTPAREN);
	engine->RegisterGlobalProperty("const int SDLK_KP_RIGHTPAREN", (void*)&AS_SDLK_KP_RIGHTPAREN);
	engine->RegisterGlobalProperty("const int SDLK_KP_LEFTBRACE", (void*)&AS_SDLK_KP_LEFTBRACE);
	engine->RegisterGlobalProperty("const int SDLK_KP_RIGHTBRACE", (void*)&AS_SDLK_KP_RIGHTBRACE);
	engine->RegisterGlobalProperty("const int SDLK_KP_TAB", (void*)&AS_SDLK_KP_TAB);
	engine->RegisterGlobalProperty("const int SDLK_KP_BACKSPACE", (void*)&AS_SDLK_KP_BACKSPACE);
	engine->RegisterGlobalProperty("const int SDLK_KP_A", (void*)&AS_SDLK_KP_A);
	engine->RegisterGlobalProperty("const int SDLK_KP_B", (void*)&AS_SDLK_KP_B);
	engine->RegisterGlobalProperty("const int SDLK_KP_C", (void*)&AS_SDLK_KP_C);
	engine->RegisterGlobalProperty("const int SDLK_KP_D", (void*)&AS_SDLK_KP_D);
	engine->RegisterGlobalProperty("const int SDLK_KP_E", (void*)&AS_SDLK_KP_E);
	engine->RegisterGlobalProperty("const int SDLK_KP_F", (void*)&AS_SDLK_KP_F);
	engine->RegisterGlobalProperty("const int SDLK_KP_XOR", (void*)&AS_SDLK_KP_XOR);
	engine->RegisterGlobalProperty("const int SDLK_KP_POWER", (void*)&AS_SDLK_KP_POWER);
	engine->RegisterGlobalProperty("const int SDLK_KP_PERCENT", (void*)&AS_SDLK_KP_PERCENT);
	engine->RegisterGlobalProperty("const int SDLK_KP_LESS", (void*)&AS_SDLK_KP_LESS);
	engine->RegisterGlobalProperty("const int SDLK_KP_GREATER", (void*)&AS_SDLK_KP_GREATER);
	engine->RegisterGlobalProperty("const int SDLK_KP_AMPERSAND", (void*)&AS_SDLK_KP_AMPERSAND);
	engine->RegisterGlobalProperty("const int SDLK_KP_DBLAMPERSAND", (void*)&AS_SDLK_KP_DBLAMPERSAND);
	engine->RegisterGlobalProperty("const int SDLK_KP_VERTICALBAR", (void*)&AS_SDLK_KP_VERTICALBAR);
	engine->RegisterGlobalProperty("const int SDLK_KP_DBLVERTICALBAR", (void*)&AS_SDLK_KP_DBLVERTICALBAR);
	engine->RegisterGlobalProperty("const int SDLK_KP_COLON", (void*)&AS_SDLK_KP_COLON);
	engine->RegisterGlobalProperty("const int SDLK_KP_HASH", (void*)&AS_SDLK_KP_HASH);
	engine->RegisterGlobalProperty("const int SDLK_KP_SPACE", (void*)&AS_SDLK_KP_SPACE);
	engine->RegisterGlobalProperty("const int SDLK_KP_AT", (void*)&AS_SDLK_KP_AT);
	engine->RegisterGlobalProperty("const int SDLK_KP_EXCLAM", (void*)&AS_SDLK_KP_EXCLAM);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMSTORE", (void*)&AS_SDLK_KP_MEMSTORE);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMRECALL", (void*)&AS_SDLK_KP_MEMRECALL);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMCLEAR", (void*)&AS_SDLK_KP_MEMCLEAR);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMADD", (void*)&AS_SDLK_KP_MEMADD);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMSUBTRACT", (void*)&AS_SDLK_KP_MEMSUBTRACT);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMMULTIPLY", (void*)&AS_SDLK_KP_MEMMULTIPLY);
	engine->RegisterGlobalProperty("const int SDLK_KP_MEMDIVIDE", (void*)&AS_SDLK_KP_MEMDIVIDE);
	engine->RegisterGlobalProperty("const int SDLK_KP_PLUSMINUS", (void*)&AS_SDLK_KP_PLUSMINUS);
	engine->RegisterGlobalProperty("const int SDLK_KP_CLEAR", (void*)&AS_SDLK_KP_CLEAR);
	engine->RegisterGlobalProperty("const int SDLK_KP_CLEARENTRY", (void*)&AS_SDLK_KP_CLEARENTRY);
	engine->RegisterGlobalProperty("const int SDLK_KP_BINARY", (void*)&AS_SDLK_KP_BINARY);
	engine->RegisterGlobalProperty("const int SDLK_KP_OCTAL", (void*)&AS_SDLK_KP_OCTAL);
	engine->RegisterGlobalProperty("const int SDLK_KP_DECIMAL", (void*)&AS_SDLK_KP_DECIMAL);
	engine->RegisterGlobalProperty("const int SDLK_KP_HEXADECIMAL", (void*)&AS_SDLK_KP_HEXADECIMAL);
	engine->RegisterGlobalProperty("const int SDLK_LCTRL", (void*)&AS_SDLK_LCTRL);
	engine->RegisterGlobalProperty("const int SDLK_LSHIFT", (void*)&AS_SDLK_LSHIFT);
	engine->RegisterGlobalProperty("const int SDLK_LALT", (void*)&AS_SDLK_LALT);
	engine->RegisterGlobalProperty("const int SDLK_LGUI", (void*)&AS_SDLK_LGUI);
	engine->RegisterGlobalProperty("const int SDLK_RCTRL", (void*)&AS_SDLK_RCTRL);
	engine->RegisterGlobalProperty("const int SDLK_RSHIFT", (void*)&AS_SDLK_RSHIFT);
	engine->RegisterGlobalProperty("const int SDLK_RALT", (void*)&AS_SDLK_RALT);
	engine->RegisterGlobalProperty("const int SDLK_RGUI", (void*)&AS_SDLK_RGUI);
	engine->RegisterGlobalProperty("const int SDLK_MODE", (void*)&AS_SDLK_MODE);
	engine->RegisterGlobalProperty("const int SDLK_AUDIONEXT", (void*)&AS_SDLK_AUDIONEXT);
	engine->RegisterGlobalProperty("const int SDLK_AUDIOPREV", (void*)&AS_SDLK_AUDIOPREV);
	engine->RegisterGlobalProperty("const int SDLK_AUDIOSTOP", (void*)&AS_SDLK_AUDIOSTOP);
	engine->RegisterGlobalProperty("const int SDLK_AUDIOPLAY", (void*)&AS_SDLK_AUDIOPLAY);
	engine->RegisterGlobalProperty("const int SDLK_AUDIOMUTE", (void*)&AS_SDLK_AUDIOMUTE);
	engine->RegisterGlobalProperty("const int SDLK_MEDIASELECT", (void*)&AS_SDLK_MEDIASELECT);
	engine->RegisterGlobalProperty("const int SDLK_AC_SEARCH", (void*)&AS_SDLK_AC_SEARCH);
	engine->RegisterGlobalProperty("const int SDLK_AC_HOME", (void*)&AS_SDLK_AC_HOME);
	engine->RegisterGlobalProperty("const int SDLK_AC_BACK", (void*)&AS_SDLK_AC_BACK);
	engine->RegisterGlobalProperty("const int SDLK_AC_FORWARD", (void*)&AS_SDLK_AC_FORWARD);
	engine->RegisterGlobalProperty("const int SDLK_AC_STOP", (void*)&AS_SDLK_AC_STOP);
	engine->RegisterGlobalProperty("const int SDLK_AC_REFRESH", (void*)&AS_SDLK_AC_REFRESH);
	engine->RegisterGlobalProperty("const int SDLK_AC_BOOKMARKS", (void*)&AS_SDLK_AC_BOOKMARKS);
	engine->RegisterGlobalProperty("const int SDLK_SLEEP", (void*)&AS_SDLK_SLEEP);
	engine->RegisterGlobalProperty("const int MESSAGEBOX_ERROR", (void*)&AS_MESSAGEBOX_ERROR);
	engine->RegisterGlobalProperty("const int MESSAGEBOX_WARN", (void*)&AS_MESSAGEBOX_WARN);
	engine->RegisterGlobalProperty("const int MESSAGEBOX_INFO", (void*)&AS_MESSAGEBOX_INFO);
	engine->RegisterGlobalProperty("const int CDECL", (void*)&as_CDECL);
	engine->RegisterGlobalProperty("const int STD_CALL", (void*)&AS_STD_CALL);
}

