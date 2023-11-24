#include <random>
#include <type_traits>
#pragma comment(lib, "bass.lib")
#pragma comment(lib, "nvdaControllerClient64.lib")
#include <thread>
#include "nvdaController.h"

#include <chrono>
#include "bass.h"
#include <string>
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include "NGT.H"
HWND hwnd;
bool keys[256];
bool keys2[256];
bool down[256];
bool keyhook = false;
void sound_system_init() {
    BASS_Init(-1, 44100, 0, 0, NULL);
    BASS_Apply3D();
    BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_OFF);
}

const int JAWS = 1;
const int NVDA = 2;
float random(long min, long max) {
    static_assert(std::is_arithmetic<double>::value, "T must be an arithmetic type");

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(min, max+1);

    return dis(gen);
}

void speak(std::wstring text, bool stop) {
    if (stop == true){
        nvdaController_cancelSpeech();
    }
//    SetWindowTextW(hwnd, text.c_str());
    nvdaController_speakText(text.c_str());
}
void stop_speech() {
    nvdaController_cancelSpeech();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        down[wParam] = true;
        if(keys[wParam]==false and keys2[wParam]==true and down[wParam]==true) {
            keys[wParam] = true;
            keys2[wParam] = false;
            return 1;
        }
        else if (keys2[wParam] == false) {
            keys[wParam] = false;
            }
    break;
    case WM_KEYUP:
        down[wParam] = false;
        keys[wParam] = false;
        keys2[wParam] = true;

        break;
    case WM_KILLFOCUS:

        BASS_Pause();
        break;
    case WM_CREATE:
        break;
    case WM_SETFOCUS:

        BASS_Start();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 1;
}
bool show_game_window(std::wstring title)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEXW wc;
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"ngtgame";
    wc.hIconSm = NULL;

    if (!RegisterClassExW(&wc))
    {
        return false;
    }

    hwnd = CreateWindowExW(256, L"ngtgame", title.c_str(), WS_VISIBLE | WS_CAPTION | WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);
    if (!hwnd)
    {
        return false;
    }

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    return true;
}
void update_game_window(){ 
    Sleep(5);
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
    bool key_pressed(int key_code) {
    if (keys[key_code] == true && keys2[key_code] == false ) {
        keys[key_code] = false;
        return true;
    }
    else {
        return false;
    }
}

bool key_released(int key_code) {
    if (keys[key_code] == false && keys2[key_code] == true) {
        keys2[key_code] = keys[key_code];
        return true;
    }
    else {
        return false;
    }
}
bool key_down(int key_code) {
    return down[key_code];
}
bool alert(std::wstring title, std::wstring text)
{
    if (MessageBoxW(0, text.c_str(), title.c_str(), MB_OK) == IDOK)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void set_listener_position(float l_x, float l_y, float l_z) {
    BASS_3DVECTOR l_pos{ l_x, l_y, l_z };
    BASS_Set3DPosition(&l_pos, 0, 0, 0);
    BASS_Apply3D();
}
void wait(int time) {
    timer waittimer;
    int el = 0;
    while (el < time) {
        update_game_window();
        el = waittimer.elapsed();
    }
}
bool sound::load(std::wstring filename, bool hrtf) {
    if (!hrtf) {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, 0);
    }
    else {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, BASS_SAMPLE_MONO| BASS_SAMPLE_3D);
    }

    return handle_ != 0;
}
bool sound::load_looped(std::wstring filename, bool hrtf) {
    if (!hrtf) {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, BASS_SAMPLE_LOOP);
    }
    else {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, BASS_SAMPLE_LOOP|BASS_SAMPLE_MONO | BASS_SAMPLE_3D);
    }

    return handle_ != 0;
}


bool sound::play() {
    
return BASS_ChannelPlay(handle_, true);
}
/*
bool sound::play_looped() {
    return BASS_ChannelPlay(handle_, TRUE);
}
*/
bool sound::pause() {
    return BASS_ChannelPause(handle_);
}

bool sound::play_wait() {
    BASS_ChannelPlay(handle_, true);
    while (true) {
        update_game_window();
        bool ac = sound::is_playing();
        if (ac == false) {
            break;
        }
    }
    return true;
}
    bool sound::stop() {
    return BASS_ChannelStop(handle_);
}

bool sound::close() {
    return BASS_StreamFree(handle_);
}
void sound::set_sound_position(float s_x, float s_y, float s_z) {
    BASS_3DVECTOR s_pos{ s_x, s_y, s_z };
    BASS_ChannelSet3DPosition(handle_, &s_pos, 0, 0);
    BASS_Apply3D();
}
float sound::get_pan() const {
    float pan;
    BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_PAN, &pan);
    return pan;
}

void sound::set_pan(float pan) {
    BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_PAN, pan);
}

float sound::get_volume() const {
    float volume;
    BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_VOL, &volume);
    return volume;
}

void sound::set_volume(float volume) {
    BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_VOL, volume);
}

double sound::get_pitch() const {
    float pitch;
    BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_FREQ, &pitch);
    return pitch;
}

void sound::set_pitch(double pitch) {
    BASS_ChannelSetAttribute(handle_, BASS_ATTRIB_FREQ, pitch);
}

bool sound::is_active() const {
    return BASS_ChannelIsActive(handle_) == BASS_ACTIVE_PLAYING;
}

bool sound::is_playing() const 
{
    return BASS_ChannelIsActive(handle_) == BASS_ACTIVE_PLAYING;
}

bool sound::is_paused() const {
    return BASS_ChannelIsActive(handle_) == BASS_ACTIVE_PAUSED;
}

double sound::get_position() const {
    QWORD pos = BASS_ChannelGetPosition(handle_, BASS_POS_BYTE);
    return pos;
}

double sound::get_length() const {
    QWORD length = BASS_ChannelGetLength(handle_, BASS_POS_BYTE);
    return length;
}

double sound::get_sample_rate() const {
    float rate;
    BASS_ChannelGetAttribute(handle_, BASS_ATTRIB_FREQ, &rate);
    return rate;
}
    int timer::elapsed() {
        if (paused != 0) {
            return paused;
        }
        else {
            return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - inittime).count();
        }
    }
/*
    void timer::elapsed(int amount) {
        if (paused == 0) {
            inittime = std::chrono::steady_clock::now() - std::chrono::milliseconds(amount);
        }
        else {
            paused = amount;
        }
    }
*/
    void timer::restart() {
        inittime = std::chrono::steady_clock::now();
        paused = 0;
    }

    void timer::pause() {
        paused = elapsed();
    }

    void timer::resume() {
        inittime = std::chrono::steady_clock::now() - std::chrono::milliseconds(paused);
        paused = 0;
    }

        bool key_hold::pressing()
        {
            status = key_down(key_code);
            if (status == false)
            {
                repeat_time = 0;
                key_timer.restart();
                key_flag = 0;
                return false;
            }
            int kh = key_timer.elapsed();
            if (kh >= repeat_time)
            {
                    switch (key_flag)
                {
                case 0:
                    key_flag = 1;
                    repeat_time = setting_1;
                    key_timer.restart();
                    break;
                case 1:
                    repeat_time = setting_2;
                    key_timer.restart();

                    break;
                }
                return true;
            }
            return false;
        }
