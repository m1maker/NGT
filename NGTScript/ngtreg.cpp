    #pragma comment(lib, "angelscript64.lib")
#include "scriptbuilder/scriptbuilder.h"
#include "scriptstdstring/scriptstdstring.h"
#include <fstream>
#include <cstdlib>

#include <iostream>

#include <assert.h>  // assert()

#include "angelscript.h"
#include "ngt.h"
HWND g_hwndEdit;

HWND hwnd;
HWND buttonc;
WNDPROC originalEditProc;

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_KEYDOWN && wParam == VK_TAB )
    {
        // Handle tab key press event
            SetFocus(buttonc); // Set focus to the next control
        return 0; // Return 0 to indicate that the keypress has been handled
    }
    if (uMsg== WM_KEYDOWN && wParam == VK_ESCAPE or (wParam == VK_RETURN) and(GetFocus()==g_hwndEdit))
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }

    // Call the original edit control procedure for default handling
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

    g_hwndEdit = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"EDIT",
        L"",
        WS_CHILD | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_TABSTOP | ES_NOHIDESEL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | WS_HSCROLL | WS_VSCROLL | WS_BORDER,
        10, 10, 400, 200,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

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
            SendMessage(g_hwndEdit, WM_SETTEXT, TRUE, (LPARAM)result_message.c_str());


        }

    }

}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static int lineCount = 0;

    switch (msg)
    {
    case WM_CREATE:

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

    return 0;
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
    engine->RegisterGlobalFunction("void init_engine()", asFUNCTION(init_engine), asCALL_CDECL);
    engine->RegisterGlobalFunction("int random(int, int)", asFUNCTION(random), asCALL_CDECL);
    engine->RegisterGlobalFunction("int get_last_error()", asFUNCTION(get_last_error), asCALL_CDECL);

    engine->RegisterGlobalFunction("void speak(string &in, bool=true)", asFUNCTION(speak), asCALL_CDECL);
    engine->RegisterGlobalFunction("void speak_wait(string &in, bool=true)", asFUNCTION(speak_wait), asCALL_CDECL);

    engine->RegisterGlobalFunction("void stop_speech()", asFUNCTION(stop_speech), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool show_game_window(string &in,int=640,int=480)", asFUNCTION(show_game_window), asCALL_CDECL);
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

    engine->RegisterGlobalFunction("bool alert(string &in, string &in)", asFUNCTION(alert), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTION(set_listener_position), asCALL_CDECL);
    engine->RegisterGlobalFunction("void wait(int)", asFUNCTION(wait), asCALL_CDECL);
    engine->RegisterGlobalFunction("void delay(int)",asFUNCTION(delay),asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_sound_storage(string &in)", asFUNCTION(set_sound_storage), asCALL_CDECL);

    engine->RegisterGlobalFunction("string get_sound_storage()", asFUNCTION(get_sound_storage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_master_volume(float)", asFUNCTION(set_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("float get_master_volume()", asFUNCTION(get_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("void switch_audio_system(int)", asFUNCTION(switch_audio_system), asCALL_CDECL);

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

    engine->RegisterObjectMethod("sound", "float get_pan() const", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(float)", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_volume() const", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(float)", asMETHOD(sound, set_volume), asCALL_THISCALL);
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
    engine->RegisterObjectType("timer", sizeof(timer), asOBJ_REF);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_FACTORY, "timer@ f()", asFUNCTION(ftimer), asCALL_CDECL);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_ADDREF, "void f()", asMETHOD(timer, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("timer", asBEHAVE_RELEASE, "void f()", asMETHOD(timer, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "int elapsed()", asMETHOD(timer, elapsed), asCALL_THISCALL);
//    engine->RegisterObjectMethod("timer", "void elapsed(int)", asMETHOD(timer, elapsed), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void restart()", asMETHOD(timer, restart), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void pause()", asMETHOD(timer, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("timer", "void resume()", asMETHOD(timer, resume), asCALL_THISCALL);
//    engine->RegisterObjectType("key_hold", sizeof(key_hold), asOBJ_VALUE | asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<key_hold>());
//    engine->RegisterObjectMethod("key_hold", "bool pressing()", asMETHOD(key_hold, pressing), asCALL_THISCALL);
}

