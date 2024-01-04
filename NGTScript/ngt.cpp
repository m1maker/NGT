#define DLLCALL   __declspec( dllimport )
#include <random>
#include <type_traits>
#include <Windows.h>
#include <thread>
#include "Tolk.h"
#include"synthizer.h"
#include "synthizer_constants.h"
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
std::string sound_path;
std::map<SDL_Keycode,bool> keys;
bool keyhook = false;
std::string inputtext;
short audio_system = PAN_AUDIO;
struct syz_LibraryConfig library_config;
void init_engine() {
    syz_libraryConfigSetDefaults(&library_config);
    library_config.log_level = SYZ_LOG_LEVEL_DEBUG;
    library_config.logging_backend = SYZ_LOGGING_BACKEND_STDERR;
    syz_initializeWithConfig(&library_config);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    Tolk_TrySAPI(true);

    Tolk_Load();
    Tolk_DetectScreenReader();
    if (!Tolk_HasSpeech()) {
        Tolk_PreferSAPI(true);
}
}
std::random_device rd;
std::mt19937 gen(rd());
long random(long min, long max) {
    static_assert(std::is_arithmetic<double>::value, "T must be an arithmetic type");

    std::uniform_real_distribution<double> dis(min, max+1);

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
SDL_Event e;
bool show_game_window(std::string title,int width, int height)
{
    SDL_RegisterApp("NGTWindow", 0, win);

    win=SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);

SDL_StartTextInput();
if (win!=NULL)
{
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
if (e.type==SDL_QUIT)
{
quit();
}
else if (e.type == SDL_TEXTINPUT)
inputtext += e.text.text;

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

    syz_shutdown();

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

bool sound::load(std::string filename, bool set3d) {
    std::string result;
    if (sound_path != "") {
         result = sound_path + "/" + filename.c_str();

    }
    else
        result = filename;
    syz_createContext(&handle_, NULL, NULL);
    syz_createBufferGenerator(&generator, handle_, NULL, NULL, NULL);
if(!set3d)
    syz_createDirectSource(&source, handle_, NULL, NULL, NULL);
else
syz_createScalarPannedSource(&source, handle_, SYZ_PANNER_STRATEGY_HRTF, 0.0, NULL, NULL, NULL);

syz_sourceAddGenerator(source, generator);

    syz_createStreamHandleFromStreamParams(&stream, "file", result.c_str(), NULL, NULL, NULL);
    syz_createBufferFromStreamHandle(&buffer, stream, NULL, NULL);
    return false;
}
bool sound::load_from_memory(std::string data, bool set3d) {
    return false;
}

bool sound::play() {
    syz_setI(generator, SYZ_P_LOOPING, 0);

    return syz_setO(generator, SYZ_P_BUFFER, buffer);

}
bool sound::play_looped() {
    syz_setI(generator, SYZ_P_LOOPING, 1);
    return syz_setO(generator, SYZ_P_BUFFER, buffer);
}
bool sound::pause() {
    return syz_pause(generator);
}

bool sound::play_wait() {
    syz_setI(generator, SYZ_P_LOOPING, 0);

    syz_setO(generator, SYZ_P_BUFFER, buffer);
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
        return syz_handleDecRef(buffer);
    }

    bool sound::close() {
        syz_handleDecRef(handle_);
        syz_handleDecRef(generator);
        syz_handleDecRef(buffer);
        syz_handleDecRef(source);
        return true;
    }
void sound::set_sound_position(float s_x, float s_y, float s_z) {

}
void sound::set_sound_reverb(float input_gain, float reverb_mix, float reverb_time){
}
void sound::cancel_reverb() {
}
double sound::get_pan() const {
    double pan;
    syz_getD(&pan, source, SYZ_P_PANNING_SCALAR);

    return pan;
}

void sound::set_pan(double pan) {
    syz_setD(source, SYZ_P_PANNING_SCALAR, pan);

}

double sound::get_volume() const {
    double volume=0;
    return volume;
}

void sound::set_volume(double volume) {
}

double sound::get_pitch() const {
    float pitch=0;
    return pitch;
}

void sound::set_pitch(double pitch) {
}

bool sound::is_active() const {
    return false;
}

bool sound::is_playing() const 

{
    return false;
}

bool sound::is_paused() const {
    return false;
}

double sound::get_position() const {
    return 2;
}

double sound::get_length() const {
    return 2;

}

double sound::get_sample_rate() const {
    float rate=0;
    return rate;
}
void timer::construct() {
}

void timer::destruct() {
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