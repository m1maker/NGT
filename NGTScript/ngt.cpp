#include <random>
#include <type_traits>
#pragma comment(lib, "bass.lib")
#pragma comment(lib, "nvdaControllerClient64.lib")
#include <thread>
#include "nvdaController.h"

#include <chrono>
#include "bass.h"
#include <string>
#include"sdl/SDL.h"
#include <commctrl.h>
#include<map>
#include <stdio.h>
#include "NGT.H"
#include <locale>
#include <codecvt>

std::wstring wstr(const std::string& utf8String)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
}

HWND hwnd;
std::map<SDL_Keycode,bool> keys;
bool keyhook = false;
void init_engine() {
    SDL_Init(SDL_INIT_EVERYTHING);
    BASS_Init(-1, 44100, 0, 0, NULL);
    BASS_Apply3D();
    BASS_SetConfig(BASS_CONFIG_3DALGORITHM, BASS_3DALG_OFF);
}

const int JAWS = 1;
const int NVDA = 2;
std::random_device rd;
std::mt19937 gen(rd());
float random(long min, long max) {
    static_assert(std::is_arithmetic<double>::value, "T must be an arithmetic type");

    std::uniform_real_distribution<double> dis(min, max+1);

    return dis(gen);
}

void speak(std::string text, bool stop) {
    if (stop == true){
        nvdaController_cancelSpeech();
    }
    std::wstring textstr = wstr(text);
    nvdaController_speakText(textstr.c_str());
}
void stop_speech() {
    nvdaController_cancelSpeech();
}
SDL_Window* win=NULL;
SDL_Event e;
bool show_game_window(std::string title,int width, int height)
{
win=SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);
if (win!=NULL)
{
return true;
}
return false;
}
void update_game_window()
{
SDL_PollEvent(&e);
if (e.type==SDL_QUIT)
{
quit();
}
else if (e.type==SDL_KEYDOWN)
{
keys[e.key.keysym.sym]=true;
}
else if (e.type==SDL_KEYUP)
{
keys[e.key.keysym.sym]=false;
}
}
void quit()
{
SDL_DestroyWindow(win);
win=NULL;
BASS_Free();
SDL_Quit();
exit(0);
}
bool key_pressed(SDL_Keycode key_code)
{
if (e.type==SDL_KEYDOWN)
{
if (e.key.keysym.sym==key_code && e.key.repeat==0)
{
return true;
}
return false;
}
}
bool key_released(SDL_Keycode key_code)
{
    if (e.type == SDL_KEYUP)
    {
        if (e.key.keysym.sym == key_code)
        {
            return true;
        }
        return false;
    }
}
bool key_down(SDL_Keycode key_code)
{
if (keys.find(key_code)!=keys.end())
return keys[key_code];
return false;
}
bool alert(std::string title, std::string text)
{
SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,title.c_str(),text.c_str(),NULL);
return true;
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
void delay(int ms)
{
SDL_Delay(ms);
}
bool sound::load(std::string filename, bool hrtf) {
    if (!hrtf) {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, 0);
    }
    else {
        handle_ = BASS_StreamCreateFile(false, filename.c_str(), 0, 0, BASS_SAMPLE_MONO| BASS_SAMPLE_3D);
    }

    return handle_ != 0;
}

bool sound::play() {
    BASS_ChannelFlags(handle_,BASS_SAMPLE_LOOP,0);
    return BASS_ChannelPlay(handle_, true);
}
bool sound::play_looped() {
    BASS_ChannelFlags(handle_,BASS_SAMPLE_LOOP,BASS_SAMPLE_LOOP);
    return BASS_ChannelPlay(handle_, TRUE);
}
bool sound::pause() {
    return BASS_ChannelPause(handle_);
}

bool sound::play_wait() {
    BASS_ChannelPlay(handle_, true);
    while (true) {
        update_game_window();
        delay(5);
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
