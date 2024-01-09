#define DLLCALL   __declspec( dllimport )
//#define MA_NO_OPUS      /* Disable the (not yet implemented) built-in Opus decoder to ensure the libopus decoder is picked. */
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include <random>
#include <type_traits>
#include <Windows.h>
#include <thread>
#include "Tolk.h"
#include <chrono>
#include <string>
#include"sdl/SDL.h"
#include <commctrl.h>
#include<map>
#include <stdio.h>
#include "NGT.H"
#include <locale>
#include <codecvt>
#include<fstream>
SDL_Window* win = NULL;
HMODULE bass;
const short PAN_AUDIO = 0;
const short HRTF_AUDIO = 1;
std::wstring wstr(const std::string& utf8String)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
}
std::wstring reader;
std::string sound_path;
std::map<SDL_Keycode,bool> keys;
bool keyhook = false;
std::string inputtext;
short audio_system = PAN_AUDIO;
ma_engine sound_engine;
void init_engine() {
    ma_engine_init(NULL, &sound_engine);
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    Tolk_TrySAPI(true);

    Tolk_Load();
    reader=Tolk_DetectScreenReader();
    if (!Tolk_HasSpeech()) {
        Tolk_PreferSAPI(true);
}
}
std::random_device rd;
std::mt19937 gen(rd());
int random(int min, int max) {
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}
double random(double min, double max) {
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}
int get_last_error() {return 0; 
}
void speak(std::string text, bool stop) {
    std::wstring textstr = wstr(text);
    Tolk_Speak(textstr.c_str(), stop);
}
void speak_wait(std::string text, bool stop) {
    std::wstring textstr = wstr(text);
    Tolk_Speak(textstr.c_str(), stop);
    while (Tolk_IsSpeaking()) {
        update_game_window();
    }
}

void stop_speech() {
        Tolk_Silence();
    }
bool window_closable;
SDL_Event e;
bool show_game_window(std::string title,int width, int height, bool closable)
{
if(reader==L"JAWS")
    win=SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN | SDL_WINDOW_KEYBOARD_GRABBED);
else
win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
SDL_RegisterApp("NGTWindow", 0, win);

SDL_StartTextInput();
window_closable = closable;
if (win!=NULL)
{
    update_game_window();
    return true;
}
return false;
}
void hide_game_window() {
    SDL_StopTextInput();

    SDL_DestroyWindow(win);
}
void set_game_window_title(std::string new_title) {
    SDL_SetWindowTitle(win, new_title.c_str());
}
void update_game_window()
{
        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT and window_closable == true)
        {
            quit();
        }
        else if (e.type == SDL_TEXTINPUT)
            inputtext += e.text.text;

        else if (e.type == SDL_KEYDOWN)
        {
            keys[e.key.keysym.sym] = true;
        }
        else if (e.type == SDL_KEYUP)
        {
            keys[e.key.keysym.sym] = false;
        }
    }
     void quit()
{
    ma_engine_uninit(&sound_engine);

    SDL_StopTextInput();
    SDL_DestroyWindow(win);
win=NULL;
SDL_UnregisterApp();
Tolk_Unload();
SDL_Quit();
        SDLNet_Quit();
exit(0);
}
bool clipboard_copy_text(std::string text) {
    SDL_SetClipboardText(text.c_str());
    return true;
}
std::string clipboard_read_text() {
    return SDL_GetClipboardText();
}
std::string get_input() {
    return inputtext;
}
bool key_pressed(SDL_Keycode key_code)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == key_code && e.key.repeat == 0)
        {
            return true;
        }
    }
return false;
}
bool key_released(SDL_Keycode key_code)
{
    if (e.type == SDL_KEYUP)
    {
        if (e.key.keysym.sym == key_code)
        {
            return true;
        }
    }
    return false;
}
bool key_down(SDL_Keycode key_code)
{
    if (keys.find(key_code) != keys.end()) {
        return keys[key_code];
    }
return false;
}
bool key_repeat(SDL_Keycode key_code)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.sym == key_code)
        {
            return true;
        }
    }
    return false;
}
bool alert(std::string title, std::string text, unsigned int flag)
{
    SDL_ShowSimpleMessageBox(flag , title.c_str(), text.c_str(), NULL);
return true;
}
void set_listener_position(float l_x, float l_y, float l_z) {
    ma_engine_listener_set_position(&sound_engine, 0, l_x, l_y, l_z);
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
void set_sound_storage(std::string path) {
    sound_path = path;
}
std::string get_sound_storage() {
    return sound_path;
}
void set_master_volume(float volume) {
}
float get_master_volume() {
    return 0.0;
}
void sound::construct() {
}

void sound::destruct() {
}
void switch_audio_system(short system) {
if(system==PAN_AUDIO or system==HRTF_AUDIO)
    audio_system = system;
}
BOOL sound_check(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool sound::load(std::string filename, bool set3d) {
    if (active == true)
        return false;
        std::string result;
    if (sound_path != "") {
        result = sound_path + "/" + filename.c_str();
    }
    else {
        result = filename;
    }
    std::wstring filenameW = wstr(result);
    if (!sound_check(filenameW.c_str())) {
        active = false;
        return false;
    }
    if (set3d)
        ma_sound_init_from_file(&sound_engine, result.c_str(), 0, NULL, NULL, &handle_);
    else
        ma_sound_init_from_file(&sound_engine, result.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &handle_);
    active = true;
    return true;
    }

    bool sound::load_from_memory(std::string data, bool set3d) {
        return false;
    }

    bool sound::play() {
        if (!active)return false;
        stop();
        ma_sound_set_looping(&handle_, false);
        ma_sound_start(&handle_);
        return true;
    }

    bool sound::play_looped() {
        if (!active)return false;
        stop();

        ma_sound_set_looping(&handle_, true);
        ma_sound_start(&handle_);
        return true;
    }

    bool sound::pause() {
        if (!active)return false;
        ma_sound_stop(&handle_);
    }

    bool sound::play_wait() {
        stop();
        ma_sound_set_looping(&handle_, false);
        ma_sound_start(&handle_);
        if (!active)return false;
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
        if (!active)return false;
        ma_sound_stop(&handle_);
        ma_sound_seek_to_pcm_frame(&handle_, 0);
    }
    bool sound::close() {
        if (!active)return false;
        ma_sound_uninit(&handle_);
        return true;
    }

    void sound::set_sound_position(float s_x, float s_y, float s_z) {
        if (!active)return;
        ma_sound_set_position(&handle_, s_x, s_y, s_z);
    }
    void sound::set_sound_reverb(float input_gain, float reverb_mix, float reverb_time) {
        return;
    }
    void sound::cancel_reverb() {
        return;
    }

    double sound::get_pan() const {
        if (!active)return -17435;

        double pan;
        pan=ma_sound_get_pan(&handle_);
        return pan*100;
    }

    void sound::set_pan(double pan) {
        if (!active)return;
        ma_sound_set_pan(&handle_, pan/100);
    }

    double sound::get_volume() const {
        if (!active)return -17435;

        double volume;
        volume = ma_sound_get_volume(&handle_);
        return ma_volume_linear_to_db(volume);
    }

    void sound::set_volume(double volume) {
        if (!active)return;
        if (volume > 0 or volume < -100)return;
        ma_sound_set_volume(&handle_, ma_volume_db_to_linear(volume));
    }

    double sound::get_pitch() const {
        if (!active)return -17435;

        double pitch;
        pitch = ma_sound_get_pitch(&handle_);
        return pitch*100;
    }

    void sound::set_pitch(double pitch) {
        if (!active)return;
        ma_sound_set_pitch(&handle_, pitch/100);
    }

    bool sound::is_active() const {
        return active;
    }

    bool sound::is_playing() const {
        return ma_sound_is_playing(&handle_);
    }

    bool sound::is_paused() const {
        return paused;
    }

    double sound::get_position() const {
        if (!active)return -17435;

        double position;
        position = ma_sound_get_positioning(&handle_);
        return position;
    }

    double sound::get_length() const {
        if (!active)return-17435;

        float length;
//        ma_sound_get_length_in_seconds(&handle_, length);
    }
        double sound::get_sample_rate() const {
    float rate=0;
    return rate;
}
void timer::construct() {
}

void timer::destruct() {
}



uint64_t timer::elapsed() {
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
        void reverb::construct() {
        }

        void reverb::destruct() {
        }



        void reverb::set_input_gain(float input_gain) {
        }
        void reverb::set_reverb_mix(float reverb_mix) {
        }       
        void reverb::set_reverb_time(float reverb_time) {
        }
        float reverb::get_input_gain() {
            return 2;
        }
        float reverb::get_reverb_mix() {
            return 2;
        }
        float reverb::get_reverb_time() {
            return 2;
        }
