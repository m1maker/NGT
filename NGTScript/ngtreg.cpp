    #pragma comment(lib, "angelscript64.lib")
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()
#include<Windows.h>
#include "angelscript.h"
#include "ngt.h"
HWND g_hwndEdit;
HWND hwnd;
HWND buttonc;
WNDPROC originalEditProc;

const int AS_SDLK_UNKNOWN = SDLK_UNKNOWN;
const int AS_SDLK_BACKSPACE = SDLK_BACKSPACE;
const int AS_SDLK_TAB = SDLK_TAB;
const int AS_SDLK_RETURN = SDLK_RETURN;
const int AS_SDLK_ESCAPE = SDLK_ESCAPE;
const int AS_SDLK_SPACE = SDLK_SPACE;
const int AS_SDLK_EXCLAIM = SDLK_EXCLAIM;
const int AS_SDLK_QUOTEDBL = SDLK_QUOTEDBL;
const int AS_SDLK_HASH = SDLK_HASH;
const int AS_SDLK_DOLLAR = SDLK_DOLLAR;
const int AS_SDLK_PERCENT = SDLK_PERCENT;
const int AS_SDLK_AMPERSAND = SDLK_AMPERSAND;
const int AS_SDLK_QUOTE = SDLK_QUOTE;
const int AS_SDLK_LEFTPAREN = SDLK_LEFTPAREN;
const int AS_SDLK_RIGHTPAREN = SDLK_RIGHTPAREN;
const int AS_SDLK_ASTERISK = SDLK_ASTERISK;
const int AS_SDLK_PLUS = SDLK_PLUS;
const int AS_SDLK_COMMA = SDLK_COMMA;
const int AS_SDLK_MINUS = SDLK_MINUS;
const int AS_SDLK_PERIOD = SDLK_PERIOD;
const int AS_SDLK_SLASH = SDLK_SLASH;
const int AS_SDLK_0 = SDLK_0;
const int AS_SDLK_1 = SDLK_1;
const int AS_SDLK_2 = SDLK_2;
const int AS_SDLK_3 = SDLK_3;
const int AS_SDLK_4 = SDLK_4;
const int AS_SDLK_5 = SDLK_5;
const int AS_SDLK_6 = SDLK_6;
const int AS_SDLK_7 = SDLK_7;
const int AS_SDLK_8 = SDLK_8;
const int AS_SDLK_9 = SDLK_9;
const int AS_SDLK_COLON = SDLK_COLON;
const int AS_SDLK_SEMICOLON = SDLK_SEMICOLON;
const int AS_SDLK_LESS = SDLK_LESS;
const int AS_SDLK_EQUALS = SDLK_EQUALS;
const int AS_SDLK_GREATER = SDLK_GREATER;
const int AS_SDLK_QUESTION = SDLK_QUESTION;
const int AS_SDLK_AT = SDLK_AT;
const int AS_SDLK_LEFTBRACKET = SDLK_LEFTBRACKET;
const int AS_SDLK_BACKSLASH = SDLK_BACKSLASH;
const int AS_SDLK_RIGHTBRACKET = SDLK_RIGHTBRACKET;
const int AS_SDLK_CARET = SDLK_CARET;
const int AS_SDLK_UNDERSCORE = SDLK_UNDERSCORE;
const int AS_SDLK_BACKQUOTE = SDLK_BACKQUOTE;
const int AS_SDLK_a = SDLK_a;
const int AS_SDLK_b = SDLK_b;
const int AS_SDLK_c = SDLK_c;
const int AS_SDLK_d = SDLK_d;
const int AS_SDLK_e = SDLK_e;
const int AS_SDLK_f = SDLK_f;
const int AS_SDLK_g = SDLK_g;
const int AS_SDLK_h = SDLK_h;
const int AS_SDLK_i = SDLK_i;
const int AS_SDLK_j = SDLK_j;
const int AS_SDLK_k = SDLK_k;
const int AS_SDLK_l = SDLK_l;
const int AS_SDLK_m = SDLK_m;
const int AS_SDLK_n = SDLK_n;
const int AS_SDLK_o = SDLK_o;
const int AS_SDLK_p = SDLK_p;
const int AS_SDLK_q = SDLK_q;
const int AS_SDLK_r = SDLK_r;
const int AS_SDLK_s = SDLK_s;
const int AS_SDLK_t = SDLK_t;
const int AS_SDLK_u = SDLK_u;
const int AS_SDLK_v = SDLK_v;
const int AS_SDLK_w = SDLK_w;
const int AS_SDLK_x = SDLK_x;
const int AS_SDLK_y = SDLK_y;
const int AS_SDLK_z = SDLK_z;
const int AS_SDLK_DELETE = SDLK_DELETE;
const int AS_SDLK_CAPSLOCK = SDLK_CAPSLOCK;
const int AS_SDLK_F1 = SDLK_F1;
const int AS_SDLK_F2 = SDLK_F2;
const int AS_SDLK_F3 = SDLK_F3;
const int AS_SDLK_F4 = SDLK_F4;
const int AS_SDLK_F5 = SDLK_F5;
const int AS_SDLK_F6 = SDLK_F6;
const int AS_SDLK_F7 = SDLK_F7;
const int AS_SDLK_F8 = SDLK_F8;
const int AS_SDLK_F9 = SDLK_F9;
const int AS_SDLK_F10 = SDLK_F10;
const int AS_SDLK_F11 = SDLK_F11;
const int AS_SDLK_F12 = SDLK_F12;
const int AS_SDLK_PRINTSCREEN = SDLK_PRINTSCREEN;
const int AS_SDLK_SCROLLLOCK = SDLK_SCROLLLOCK;
const int AS_SDLK_PAUSE = SDLK_PAUSE;
const int AS_SDLK_INSERT = SDLK_INSERT;
const int AS_SDLK_HOME = SDLK_HOME;
const int AS_SDLK_PAGEUP = SDLK_PAGEUP;
const int AS_SDLK_END = SDLK_END;
const int AS_SDLK_PAGEDOWN = SDLK_PAGEDOWN;
const int AS_SDLK_RIGHT = SDLK_RIGHT;
const int AS_SDLK_LEFT = SDLK_LEFT;
const int AS_SDLK_DOWN = SDLK_DOWN;
const int AS_SDLK_UP = SDLK_UP;
const int AS_SDLK_NUMLOCKCLEAR = SDLK_NUMLOCKCLEAR;
const int AS_SDLK_KP_DIVIDE = SDLK_KP_DIVIDE;
const int AS_SDLK_KP_MULTIPLY = SDLK_KP_MULTIPLY;
const int AS_SDLK_KP_MINUS = SDLK_KP_MINUS;
const int AS_SDLK_KP_PLUS = SDLK_KP_PLUS;
const int AS_SDLK_KP_ENTER = SDLK_KP_ENTER;
const int AS_SDLK_KP_1 = SDLK_KP_1;
const int AS_SDLK_KP_2 = SDLK_KP_2;
const int AS_SDLK_KP_3 = SDLK_KP_3;
const int AS_SDLK_KP_4 = SDLK_KP_4;
const int AS_SDLK_KP_5 = SDLK_KP_5;
const int AS_SDLK_KP_6 = SDLK_KP_6;
const int AS_SDLK_KP_7 = SDLK_KP_7;
const int AS_SDLK_KP_8 = SDLK_KP_8;
const int AS_SDLK_KP_9 = SDLK_KP_9;
const int AS_SDLK_KP_0 = SDLK_KP_0;
const int AS_SDLK_KP_PERIOD = SDLK_KP_PERIOD;
const int AS_SDLK_APPLICATION = SDLK_APPLICATION;
const int AS_SDLK_POWER = SDLK_POWER;
const int AS_SDLK_KP_EQUALS = SDLK_KP_EQUALS;
const int AS_SDLK_F13 = SDLK_F13;
const int AS_SDLK_F14 = SDLK_F14;
const int AS_SDLK_F15 = SDLK_F15;
const int AS_SDLK_F16 = SDLK_F16;
const int AS_SDLK_F17 = SDLK_F17;
const int AS_SDLK_F18 = SDLK_F18;
const int AS_SDLK_F19 = SDLK_F19;
const int AS_SDLK_F20 = SDLK_F20;
const int AS_SDLK_F21 = SDLK_F21;
const int AS_SDLK_F22 = SDLK_F22;
const int AS_SDLK_F23 = SDLK_F23;
const int AS_SDLK_F24 = SDLK_F24;
const int AS_SDLK_EXECUTE = SDLK_EXECUTE;
const int AS_SDLK_HELP = SDLK_HELP;
const int AS_SDLK_MENU = SDLK_MENU;
const int AS_SDLK_SELECT = SDLK_SELECT;
const int AS_SDLK_STOP = SDLK_STOP;
const int AS_SDLK_AGAIN = SDLK_AGAIN;
const int AS_SDLK_UNDO = SDLK_UNDO;
const int AS_SDLK_CUT = SDLK_CUT;
const int AS_SDLK_COPY = SDLK_COPY;
const int AS_SDLK_PASTE = SDLK_PASTE;
const int AS_SDLK_FIND = SDLK_FIND;
const int AS_SDLK_MUTE = SDLK_MUTE;
const int AS_SDLK_VOLUMEUP = SDLK_VOLUMEUP;
const int AS_SDLK_VOLUMEDOWN = SDLK_VOLUMEDOWN;
const int AS_SDLK_KP_COMMA = SDLK_KP_COMMA;
const int AS_SDLK_KP_EQUALSAS400 = SDLK_KP_EQUALSAS400;
const int AS_SDLK_ALTERASE = SDLK_ALTERASE;
const int AS_SDLK_SYSREQ = SDLK_SYSREQ;
const int AS_SDLK_CANCEL = SDLK_CANCEL;
const int AS_SDLK_CLEAR = SDLK_CLEAR;
const int AS_SDLK_PRIOR = SDLK_PRIOR;
const int AS_SDLK_RETURN2 = SDLK_RETURN2;
const int AS_SDLK_SEPARATOR = SDLK_SEPARATOR;
const int AS_SDLK_OUT = SDLK_OUT;
const int AS_SDLK_OPER = SDLK_OPER;
const int AS_SDLK_CLEARAGAIN = SDLK_CLEARAGAIN;
const int AS_SDLK_CRSEL = SDLK_CRSEL;
const int AS_SDLK_EXSEL = SDLK_EXSEL;
const int AS_SDLK_KP_00 = SDLK_KP_00;
const int AS_SDLK_KP_000 = SDLK_KP_000;
const int AS_SDLK_THOUSANDSSEPARATOR = SDLK_THOUSANDSSEPARATOR;
const int AS_SDLK_DECIMALSEPARATOR = SDLK_DECIMALSEPARATOR;
const int AS_SDLK_CURRENCYUNIT = SDLK_CURRENCYUNIT;
const int AS_SDLK_CURRENCYSUBUNIT = SDLK_CURRENCYSUBUNIT;
const int AS_SDLK_KP_LEFTPAREN = SDLK_KP_LEFTPAREN;
const int AS_SDLK_KP_RIGHTPAREN = SDLK_KP_RIGHTPAREN;
const int AS_SDLK_KP_LEFTBRACE = SDLK_KP_LEFTBRACE;
const int AS_SDLK_KP_RIGHTBRACE = SDLK_KP_RIGHTBRACE;
const int AS_SDLK_KP_TAB = SDLK_KP_TAB;
const int AS_SDLK_KP_BACKSPACE = SDLK_KP_BACKSPACE;
const int AS_SDLK_KP_A = SDLK_KP_A;
const int AS_SDLK_KP_B = SDLK_KP_B;
const int AS_SDLK_KP_C = SDLK_KP_C;
const int AS_SDLK_KP_D = SDLK_KP_D;
const int AS_SDLK_KP_E = SDLK_KP_E;
const int AS_SDLK_KP_F = SDLK_KP_F;
const int AS_SDLK_KP_XOR = SDLK_KP_XOR;
const int AS_SDLK_KP_POWER = SDLK_KP_POWER;
const int AS_SDLK_KP_PERCENT = SDLK_KP_PERCENT;
const int AS_SDLK_KP_LESS = SDLK_KP_LESS;
const int AS_SDLK_KP_GREATER = SDLK_KP_GREATER;
const int AS_SDLK_KP_AMPERSAND = SDLK_KP_AMPERSAND;
const int AS_SDLK_KP_DBLAMPERSAND = SDLK_KP_DBLAMPERSAND;
const int AS_SDLK_KP_VERTICALBAR = SDLK_KP_VERTICALBAR;
const int AS_SDLK_KP_DBLVERTICALBAR = SDLK_KP_DBLVERTICALBAR;
const int AS_SDLK_KP_COLON = SDLK_KP_COLON;
const int AS_SDLK_KP_HASH = SDLK_KP_HASH;
const int AS_SDLK_KP_SPACE = SDLK_KP_SPACE;
const int AS_SDLK_KP_AT = SDLK_KP_AT;
const int AS_SDLK_KP_EXCLAM = SDLK_KP_EXCLAM;
const int AS_SDLK_KP_MEMSTORE = SDLK_KP_MEMSTORE;
const int AS_SDLK_KP_MEMRECALL = SDLK_KP_MEMRECALL;
const int AS_SDLK_KP_MEMCLEAR = SDLK_KP_MEMCLEAR;
const int AS_SDLK_KP_MEMADD = SDLK_KP_MEMADD;
const int AS_SDLK_KP_MEMSUBTRACT = SDLK_KP_MEMSUBTRACT;
const int AS_SDLK_KP_MEMMULTIPLY = SDLK_KP_MEMMULTIPLY;
const int AS_SDLK_KP_MEMDIVIDE = SDLK_KP_MEMDIVIDE;
const int AS_SDLK_KP_PLUSMINUS = SDLK_KP_PLUSMINUS;
const int AS_SDLK_KP_CLEAR = SDLK_KP_CLEAR;
const int AS_SDLK_KP_CLEARENTRY = SDLK_KP_CLEARENTRY;
const int AS_SDLK_KP_BINARY = SDLK_KP_BINARY;
const int AS_SDLK_KP_OCTAL = SDLK_KP_OCTAL;
const int AS_SDLK_KP_DECIMAL = SDLK_KP_DECIMAL;
const int AS_SDLK_KP_HEXADECIMAL = SDLK_KP_HEXADECIMAL;
const int AS_SDLK_LCTRL = SDLK_LCTRL;
const int AS_SDLK_LSHIFT = SDLK_LSHIFT;
const int AS_SDLK_LALT = SDLK_LALT;
const int AS_SDLK_LGUI = SDLK_LGUI;
const int AS_SDLK_RCTRL = SDLK_RCTRL;
const int AS_SDLK_RSHIFT = SDLK_RSHIFT;
const int AS_SDLK_RALT = SDLK_RALT;
const int AS_SDLK_RGUI = SDLK_RGUI;
const int AS_SDLK_MODE = SDLK_MODE;
const int AS_SDLK_AUDIONEXT = SDLK_AUDIONEXT;
const int AS_SDLK_AUDIOPREV = SDLK_AUDIOPREV;
const int AS_SDLK_AUDIOSTOP = SDLK_AUDIOSTOP;
const int AS_SDLK_AUDIOPLAY = SDLK_AUDIOPLAY;
const int AS_SDLK_AUDIOMUTE = SDLK_AUDIOMUTE;
const int AS_SDLK_MEDIASELECT = SDLK_MEDIASELECT;
const int AS_SDLK_WWW = SDLK_WWW;
const int AS_SDLK_MAIL = SDLK_MAIL;
const int AS_SDLK_CALCULATOR = SDLK_CALCULATOR;
const int AS_SDLK_COMPUTER = SDLK_COMPUTER;
const int AS_SDLK_AC_SEARCH = SDLK_AC_SEARCH;
const int AS_SDLK_AC_HOME = SDLK_AC_HOME;
const int AS_SDLK_AC_BACK = SDLK_AC_BACK;
const int AS_SDLK_AC_FORWARD = SDLK_AC_FORWARD;
const int AS_SDLK_AC_STOP = SDLK_AC_STOP;
const int AS_SDLK_AC_REFRESH = SDLK_AC_REFRESH;
const int AS_SDLK_AC_BOOKMARKS = SDLK_AC_BOOKMARKS;
const int AS_SDLK_BRIGHTNESSDOWN = SDLK_BRIGHTNESSDOWN;
const int AS_SDLK_BRIGHTNESSUP = SDLK_BRIGHTNESSUP;
const int AS_SDLK_DISPLAYSWITCH = SDLK_DISPLAYSWITCH;
const int AS_SDLK_KBDILLUMTOGGLE = SDLK_KBDILLUMTOGGLE;
const int AS_SDLK_KBDILLUMDOWN = SDLK_KBDILLUMDOWN;
const int AS_SDLK_KBDILLUMUP = SDLK_KBDILLUMUP;
const int AS_SDLK_EJECT = SDLK_EJECT;
const int AS_SDLK_SLEEP = SDLK_SLEEP;
const int AS_MESSAGEBOX_ERROR = SDL_MESSAGEBOX_ERROR;
const int AS_MESSAGEBOX_WARN = SDL_MESSAGEBOX_WARNING;
const int AS_MESSAGEBOX_INFO = SDL_MESSAGEBOX_INFORMATION;
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_KEYDOWN:
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
using namespace std;
int nCmdShow;
HINSTANCE hInstance;
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void show_message()
{
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

    buttonc=CreateWindow(
        L"BUTTON",
        L"Close",
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


        }

    }

}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int lineCount = 0;

    switch (msg)
    {
    case WM_CREATE:
        g_hwndEdit = CreateWindowEx(
            0, L"EDIT",
            L"",
            WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_TABSTOP,
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

void add_text(std::wstring text)
{
    SetWindowText(g_hwndEdit, L"RRRRRRRRRRR");
    int len = GetWindowTextLength(g_hwndEdit);
    SendMessage(g_hwndEdit, EM_SETSEL, len, len);
    SendMessage(g_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text.c_str());
    InvalidateRect(g_hwndEdit, NULL, TRUE);
    UpdateWindow(g_hwndEdit);
}



    

    sound* fsound() { return new sound; }
reverb* freverb() { return new reverb; }
timer* ftimer() { return new timer; }
user_idle* fuser_idle() { return new user_idle; }
library* flibrary() { return new library; }
instance* finstance(std::string app) { return new instance(app); }
network_event* fnetwork_event() { return new network_event; }

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
    std::string output = std::string(msg->section) + " (" + rowStr + ", " + colStr + ") : " + type + " : " + messageStr;
    ouou = wstr(output);
    result_message  += ouou+L"\r\n";
}
void RegisterFunctions(asIScriptEngine* engine)
{
    engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, true);
    engine->RegisterGlobalFunction("int random(int, int)", asFUNCTIONPR(random, (long, long), long), asCALL_CDECL);
    engine->RegisterGlobalFunction("double random(double, double)", asFUNCTIONPR(randomDouble, (double, double), double), asCALL_CDECL);

    engine->RegisterGlobalFunction("int get_last_error()", asFUNCTION(get_last_error), asCALL_CDECL);

    engine->RegisterGlobalFunction("void speak(string &in, bool=true)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak_wait(string &in, bool=true)", asFUNCTION(speak_wait), asCALL_CDECL);

    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("void show_game_window(string &in,int=640,int=480, bool=true)", asFUNCTION(show_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void hide_game_window()", asFUNCTION(hide_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_game_window_title(string &in)", asFUNCTION(set_game_window_title), asCALL_CDECL);

    engine->RegisterGlobalFunction("void update_game_window()", asFUNCTION(update_game_window), asCALL_CDECL);
    engine->RegisterGlobalFunction("void quit()",asFUNCTION(quit),asCALL_CDECL);
    engine->RegisterGlobalFunction("bool clipboard_copy_text(string &in)", asFUNCTION(clipboard_copy_text), asCALL_CDECL);
    engine->RegisterGlobalFunction("string clipboard_read_text()", asFUNCTION(clipboard_read_text), asCALL_CDECL);
    engine->RegisterGlobalFunction("string get_input()", asFUNCTION(get_input), asCALL_CDECL);

    engine->RegisterGlobalFunction("bool key_pressed(int)", asFUNCTION(key_pressed), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_released(int)", asFUNCTION(key_released), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_down(int)", asFUNCTION(key_down), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool key_repeat(int)", asFUNCTION(key_repeat), asCALL_CDECL);

    engine->RegisterGlobalFunction("bool alert(string &in, string &in, uint=0)", asFUNCTION(alert), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTION(set_listener_position), asCALL_CDECL);
    engine->RegisterGlobalFunction("void wait(int)", asFUNCTION(wait), asCALL_CDECL);
    engine->RegisterGlobalFunction("void delay(int)",asFUNCTION(delay),asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_sound_storage(string &in)", asFUNCTION(set_sound_storage), asCALL_CDECL);

    engine->RegisterGlobalFunction("string get_sound_storage()", asFUNCTION(get_sound_storage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_master_volume(float)", asFUNCTION(set_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("float get_master_volume()", asFUNCTION(get_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("void switch_audio_system(int)", asFUNCTION(switch_audio_system), asCALL_CDECL);
    engine->RegisterGlobalFunction("string read_environment_variable(string&in)", asFUNCTION(read_environment_variable), asCALL_CDECL);

    engine->RegisterObjectType("reverb", sizeof(reverb), asOBJ_REF);
    engine->RegisterObjectBehaviour("reverb", asBEHAVE_FACTORY, "reverb@ f()", asFUNCTION(freverb), asCALL_CDECL);
    engine->RegisterObjectBehaviour("reverb", asBEHAVE_ADDREF, "void f()", asMETHOD(reverb, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("reverb", asBEHAVE_RELEASE, "void f()", asMETHOD(reverb, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("reverb", "void set_reverb_mix(float)", asMETHOD(reverb, set_reverb_mix), asCALL_THISCALL);
    engine->RegisterObjectMethod("reverb", "void set_reverb_time(float)", asMETHOD(reverb, set_reverb_time), asCALL_THISCALL);
    engine->RegisterObjectMethod("reverb", "float get_input_gain()", asMETHOD(reverb, get_input_gain), asCALL_THISCALL);
    engine->RegisterObjectMethod("reverb", "float get_reverb_mix()", asMETHOD(reverb, get_reverb_mix), asCALL_THISCALL);
    engine->RegisterObjectMethod("reverb", "float get_reverb_time   ()", asMETHOD(reverb, get_reverb_time), asCALL_THISCALL);


    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_REF);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_FACTORY, "sound@ f()", asFUNCTION(fsound), asCALL_CDECL);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_ADDREF, "void f()", asMETHOD(sound, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_RELEASE, "void f()", asMETHOD(sound, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool load(string &in, bool=false)", asMETHOD(sound, load), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "bool load_from_memory(string &in, bool=false)", asMETHOD(sound, load), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "bool play()", asMETHOD(sound, play), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_looped()", asMETHOD(sound, play_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool pause()", asMETHOD(sound, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_wait()", asMETHOD(sound, play_wait), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stop()", asMETHOD(sound, stop), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool close()", asMETHOD(sound, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_sound_position(float, float, float)", asMETHOD(sound, set_sound_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_sound_reverb(float, float, float)", asMETHOD(sound, set_sound_reverb), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void cancel_reverb()", asMETHOD(sound, cancel_reverb), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "double get_pan() const", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(double)", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_volume() const", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(double)", asMETHOD(sound, set_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_pitch() const", asMETHOD(sound, get_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch(double)", asMETHOD(sound, set_pitch), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_pitch_lower_limit() const", asMETHOD(sound, get_pitch_lower_limit), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_active() const", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_playing() const", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool is_paused() const", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_position() const", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_length() const", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_sample_rate() const", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_channels() const", asMETHOD(sound, get_channels), asCALL_THISCALL);
//    engine->RegisterObjectMethod("sound", "double get_bits() const", asMETHOD(sound, get_bits), asCALL_THISCALL);
    engine->RegisterObjectType("user_idle", sizeof(user_idle), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("user_idle", asBEHAVE_FACTORY, "user_idle@ f()", asFUNCTION(fuser_idle), asCALL_CDECL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_millis()", asMETHOD(user_idle, elapsed_millis), asCALL_THISCALL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_seconds()", asMETHOD(user_idle, elapsed_seconds), asCALL_THISCALL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_minutes()", asMETHOD(user_idle, elapsed_minutes), asCALL_THISCALL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_hours()", asMETHOD(user_idle, elapsed_hours), asCALL_THISCALL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_days()", asMETHOD(user_idle, elapsed_days), asCALL_THISCALL);
    engine->RegisterObjectMethod("user_idle", "uint64 elapsed_weeks()", asMETHOD(user_idle, elapsed_weeks), asCALL_THISCALL);

    engine->RegisterObjectType("timer", sizeof(timer), asOBJ_REF);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_FACTORY, "timer@ f()", asFUNCTION(ftimer), asCALL_CDECL);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_ADDREF, "void f()", asMETHOD(timer, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_RELEASE, "void f()", asMETHOD(timer, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_seconds()", asMETHOD(timer, elapsed_seconds), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_minutes()", asMETHOD(timer, elapsed_minutes), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_hours()", asMETHOD(timer, elapsed_hours), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_millis()", asMETHOD(timer, elapsed_millis), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_micros()", asMETHOD(timer, elapsed_micros), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "uint64 elapsed_nanos()", asMETHOD(timer, elapsed_nanos), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_seconds(uint64)", asMETHOD(timer, force_seconds), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_minutes(uint64)", asMETHOD(timer, force_minutes), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_hours(uint64)", asMETHOD(timer, force_hours), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_millis(uint64)", asMETHOD(timer, force_millis), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_micros(uint64)", asMETHOD(timer, force_micros), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void force_nanos(uint64)", asMETHOD(timer, force_nanos), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void restart()", asMETHOD(timer, restart), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void pause()", asMETHOD(timer, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void resume()", asMETHOD(timer, resume), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "bool is_running()", asMETHOD(timer, is_running), asCALL_THISCALL);
//    engine->RegisterObjectType("key_hold", sizeof(key_hold), asOBJ_VALUE | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<key_hold>());
//    engine->RegisterObjectMethod("key_hold", "bool pressing()", asMETHOD(key_hold, pressing), asCALL_THISCALL);
    engine->RegisterObjectType("library", sizeof(library), asOBJ_REF);
    engine->RegisterObjectBehaviour("library", asBEHAVE_FACTORY, "library@ f()", asFUNCTION(flibrary), asCALL_CDECL);
    engine->RegisterObjectBehaviour("library", asBEHAVE_ADDREF, "void f()", asMETHOD(library, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("library", asBEHAVE_RELEASE, "void f()", asMETHOD(library, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("library", "bool load(string&in)", asMETHOD(library, load), asCALL_THISCALL);
    engine->RegisterObjectMethod("library", "void call(string&in, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null, ?&in=null)", asMETHOD(library, call), asCALL_THISCALL);
    engine->RegisterObjectMethod("library", "void unload()", asMETHOD(library, unload), asCALL_THISCALL);
    engine->RegisterObjectType("instance", sizeof(instance), asOBJ_REF);
    engine->RegisterObjectBehaviour("instance", asBEHAVE_FACTORY, "instance@ f(string&in)", asFUNCTION(finstance), asCALL_CDECL);
    engine->RegisterObjectBehaviour("instance", asBEHAVE_ADDREF, "void f()", asMETHOD(instance, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("instance", asBEHAVE_RELEASE, "void f()", asMETHOD(instance, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("instance", "bool is_running()", asMETHOD(instance, is_running), asCALL_THISCALL);
    engine->RegisterObjectType("network_event", sizeof(network_event), asOBJ_REF);
    engine->RegisterObjectBehaviour("network_event", asBEHAVE_FACTORY, "network_event@ f()", asFUNCTION(fnetwork_event), asCALL_CDECL);
    engine->RegisterObjectBehaviour("network_event", asBEHAVE_ADDREF, "void f()", asMETHOD(network_event, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("network_event", asBEHAVE_RELEASE, "void f()", asMETHOD(network_event, destruct), asCALL_THISCALL);
    engine->RegisterObjectProperty("network_event", "const int EVENT_NONE", asOFFSET(network_event, EVENT_NONE));
    engine->RegisterObjectProperty("network_event", "const int EVENT_CONNECT", asOFFSET(network_event, EVENT_CONNECT));
    engine->RegisterObjectProperty("network_event", "const int EVENT_RECEIVE", asOFFSET(network_event, EVENT_RECEIVE));
    engine->RegisterObjectProperty("network_event", "const int EVENT_DISCONNECT", asOFFSET(network_event, EVENT_DISCONNECT));
    engine->RegisterObjectProperty("network_event", "int type", asOFFSET(network_event, m_type));
    engine->RegisterObjectProperty("network_event", "uint peerId", asOFFSET(network_event, m_peerId));
    engine->RegisterObjectProperty("network_event", "int channel", asOFFSET(network_event, m_channel));
    engine->RegisterObjectProperty("network_event", "string message", asOFFSET(network_event, m_message));


    engine->RegisterObjectType("network", sizeof(network), asOBJ_REF);
    engine->RegisterObjectBehaviour("network", asBEHAVE_FACTORY, "network@ f()", asFUNCTION(fnetwork), asCALL_CDECL);
    engine->RegisterObjectBehaviour("network", asBEHAVE_ADDREF, "void f()", asMETHOD(network, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("network", asBEHAVE_RELEASE, "void f()", asMETHOD(network, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "uint connect(string&in, int)", asMETHOD(network, connect), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool destroy()", asMETHOD(network, destroy), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool disconnect_peer(uint)", asMETHOD(network, disconnect_peer), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool disconnect_peer_forcefully(uint)", asMETHOD(network, disconnect_peer_forcefully), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool disconnect_peer_softly(uint)", asMETHOD(network, disconnect_peer_softly), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "string get_peer_address(uint)", asMETHOD(network, get_peer_address), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "double get_peer_average_round_trip_time(uint)", asMETHOD(network, get_peer_average_round_trip_time), asCALL_THISCALL);
//    engine->RegisterObjectMethod("network", "array<uint> get_peer_list()", asMETHOD(network, get_peer_list), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "network_event@ request()", asMETHOD(network, request), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool send_reliable(uint, string&in, int)", asMETHOD(network, send_reliable), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool send_unreliable(uint, string&in, int)", asMETHOD(network, send_unreliable), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool set_bandwidth_limits(double, double)", asMETHOD(network, set_bandwidth_limits), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool setup_client(int, int)", asMETHOD(network, setup_client), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "bool setup_server(int, int, int)", asMETHOD(network, setup_server), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "int get_connected_peers() const", asMETHOD(network, get_connected_peers), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "double get_bytes_sent() const", asMETHOD(network, get_bytes_sent), asCALL_THISCALL);
    engine->RegisterObjectMethod("network", "double get_bytes_received() const", asMETHOD(network, get_bytes_received), asCALL_THISCALL);
    engine->RegisterObjectType("vector", sizeof(ngtvector), asOBJ_VALUE | asOBJ_POD);

    engine->RegisterObjectProperty("vector", "float x", asOFFSET(ngtvector, x));
    engine->RegisterObjectProperty("vector", "float y", asOFFSET(ngtvector, y));
    engine->RegisterObjectProperty("vector", "float z", asOFFSET(ngtvector, z));
    engine->RegisterGlobalProperty("const int SDLK_UNKNOWN", (void*)&AS_SDLK_UNKNOWN);
engine->RegisterGlobalProperty("const int SDLK_BACKSPACE", (void*)&AS_SDLK_BACKSPACE);
engine->RegisterGlobalProperty("const int SDLK_TAB", (void*)&AS_SDLK_TAB);
engine->RegisterGlobalProperty("const int SDLK_RETURN", (void*)&AS_SDLK_RETURN);
engine->RegisterGlobalProperty("const int SDLK_ESCAPE", (void*)&AS_SDLK_ESCAPE);
engine->RegisterGlobalProperty("const int SDLK_SPACE", (void*)&AS_SDLK_SPACE);
engine->RegisterGlobalProperty("const int SDLK_EXCLAIM", (void*)&AS_SDLK_EXCLAIM);
engine->RegisterGlobalProperty("const int SDLK_QUOTEDBL", (void*)&AS_SDLK_QUOTEDBL);
engine->RegisterGlobalProperty("const int SDLK_HASH", (void*)&AS_SDLK_HASH);
engine->RegisterGlobalProperty("const int SDLK_DOLLAR", (void*)&AS_SDLK_DOLLAR);
engine->RegisterGlobalProperty("const int SDLK_PERCENT", (void*)&AS_SDLK_PERCENT);
engine->RegisterGlobalProperty("const int SDLK_AMPERSAND", (void*)&AS_SDLK_AMPERSAND);
engine->RegisterGlobalProperty("const int SDLK_QUOTE", (void*)&AS_SDLK_QUOTE);
engine->RegisterGlobalProperty("const int SDLK_LEFTPAREN", (void*)&AS_SDLK_LEFTPAREN);
engine->RegisterGlobalProperty("const int SDLK_RIGHTPAREN", (void*)&AS_SDLK_RIGHTPAREN);
engine->RegisterGlobalProperty("const int SDLK_ASTERISK", (void*)&AS_SDLK_ASTERISK);
engine->RegisterGlobalProperty("const int SDLK_PLUS", (void*)&AS_SDLK_PLUS);
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
engine->RegisterGlobalProperty("const int SDLK_COLON", (void*)&AS_SDLK_COLON);
engine->RegisterGlobalProperty("const int SDLK_SEMICOLON", (void*)&AS_SDLK_SEMICOLON);
engine->RegisterGlobalProperty("const int SDLK_LESS", (void*)&AS_SDLK_LESS);
engine->RegisterGlobalProperty("const int SDLK_EQUALS", (void*)&AS_SDLK_EQUALS);
engine->RegisterGlobalProperty("const int SDLK_GREATER", (void*)&AS_SDLK_GREATER);
engine->RegisterGlobalProperty("const int SDLK_QUESTION", (void*)&AS_SDLK_QUESTION);
engine->RegisterGlobalProperty("const int SDLK_AT", (void*)&AS_SDLK_AT);
engine->RegisterGlobalProperty("const int SDLK_LEFTBRACKET", (void*)&AS_SDLK_LEFTBRACKET);
engine->RegisterGlobalProperty("const int SDLK_BACKSLASH", (void*)&AS_SDLK_BACKSLASH);
engine->RegisterGlobalProperty("const int SDLK_RIGHTBRACKET", (void*)&AS_SDLK_RIGHTBRACKET);
engine->RegisterGlobalProperty("const int SDLK_CARET", (void*)&AS_SDLK_CARET);
engine->RegisterGlobalProperty("const int SDLK_UNDERSCORE", (void*)&AS_SDLK_UNDERSCORE);
engine->RegisterGlobalProperty("const int SDLK_BACKQUOTE", (void*)&AS_SDLK_BACKQUOTE);
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
engine->RegisterGlobalProperty("const int SDLK_WWW", (void*)&AS_SDLK_WWW);
engine->RegisterGlobalProperty("const int SDLK_MAIL", (void*)&AS_SDLK_MAIL);
engine->RegisterGlobalProperty("const int SDLK_CALCULATOR", (void*)&AS_SDLK_CALCULATOR);
engine->RegisterGlobalProperty("const int SDLK_COMPUTER", (void*)&AS_SDLK_COMPUTER);
engine->RegisterGlobalProperty("const int SDLK_AC_SEARCH", (void*)&AS_SDLK_AC_SEARCH);
engine->RegisterGlobalProperty("const int SDLK_AC_HOME", (void*)&AS_SDLK_AC_HOME);
engine->RegisterGlobalProperty("const int SDLK_AC_BACK", (void*)&AS_SDLK_AC_BACK);
engine->RegisterGlobalProperty("const int SDLK_AC_FORWARD", (void*)&AS_SDLK_AC_FORWARD);
engine->RegisterGlobalProperty("const int SDLK_AC_STOP", (void*)&AS_SDLK_AC_STOP);
engine->RegisterGlobalProperty("const int SDLK_AC_REFRESH", (void*)&AS_SDLK_AC_REFRESH);
engine->RegisterGlobalProperty("const int SDLK_AC_BOOKMARKS", (void*)&AS_SDLK_AC_BOOKMARKS);
engine->RegisterGlobalProperty("const int SDLK_BRIGHTNESSDOWN", (void*)&AS_SDLK_BRIGHTNESSDOWN);
engine->RegisterGlobalProperty("const int SDLK_BRIGHTNESSUP", (void*)&AS_SDLK_BRIGHTNESSUP);
engine->RegisterGlobalProperty("const int SDLK_DISPLAYSWITCH", (void*)&AS_SDLK_DISPLAYSWITCH);
engine->RegisterGlobalProperty("const int SDLK_KBDILLUMTOGGLE", (void*)&AS_SDLK_KBDILLUMTOGGLE);
engine->RegisterGlobalProperty("const int SDLK_KBDILLUMDOWN", (void*)&AS_SDLK_KBDILLUMDOWN);
engine->RegisterGlobalProperty("const int SDLK_KBDILLUMUP", (void*)&AS_SDLK_KBDILLUMUP);
engine->RegisterGlobalProperty("const int SDLK_EJECT", (void*)&AS_SDLK_EJECT);
engine->RegisterGlobalProperty("const int SDLK_SLEEP", (void*)&AS_SDLK_SLEEP);
engine->RegisterGlobalProperty("const int MESSAGEBOX_ERROR", (void* )& AS_MESSAGEBOX_ERROR);
engine->RegisterGlobalProperty("const int MESSAGEBOX_WARN", (void*)&AS_MESSAGEBOX_WARN);
engine->RegisterGlobalProperty("const int MESSAGEBOX_INFO", (void*)&AS_MESSAGEBOX_INFO);
}

