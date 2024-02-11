#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include <random>
#include <filesystem>
#include <thread>
#include "Tolk.h"
#include <chrono>
#include <string>
#include"sdl/SDL.h"
#include<map>
#include <stdio.h>
#include "NGT.H"
#include "scriptfile/scriptfilesystem.h"
#include <locale>
#include <codecvt>
#include<fstream>
#include <cstdlib>
#include <cstring>
HINSTANCE sndfile;
HINSTANCE openal;
bool engine_is_active= false;
SDL_Window* win = NULL;
bool window_is_focused;
std::wstring wstr(const std::string& utf8String)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
}
std::wstring reader;
std::string sound_path;
std::unordered_map<SDL_Keycode, bool> keys;
bool keyhook = false;
std::string inputtext;
ma_engine sound_engine;
typedef ALCdevice* (*LPALCOPENDEVICE)(const ALCchar*);
typedef ALCcontext* (*LPALCCREATECONTEXT)(ALCdevice*, const ALCint*);
typedef ALCboolean(*LPALCMAKECONTEXTCURRENT)(ALCcontext*);
typedef void (*LPALCDESTROYCONTEXT)(ALCcontext*);
typedef ALCboolean(*LPALCCLOSEDEVICE)(ALCdevice*);
typedef void (*LPALLISTENER3F)(ALenum, ALfloat, ALfloat, ALfloat);
typedef void (*LPALLISTENERF)(ALenum, ALfloat);
ALCdevice* device = NULL;
ALCcontext* context = NULL;
void init_engine(){
    sndfile = LoadLibrary(L"sndfile.dll");
    openal = LoadLibrary(L"OpenAL32.dll");
    Tolk_Load();
    if (openal != NULL) {
    LPALCOPENDEVICE alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(openal, "alcOpenDevice");
    LPALCCREATECONTEXT alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(openal, "alcCreateContext");
    LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(openal, "alcMakeContextCurrent");

        device = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
    }
    ma_engine_init(NULL, &sound_engine);
    SDL_Init(SDL_INIT_EVERYTHING);
    enet_initialize();
    Tolk_TrySAPI(true);

    reader=Tolk_DetectScreenReader();
    if (!Tolk_HasSpeech()) {
        Tolk_PreferSAPI(true);
}
}
void set_library_path(const std::string& path) {
    if (Tolk_IsLoaded())Tolk_Unload();
    std::filesystem::path current_dir = std::filesystem::current_path();
    std::filesystem::path new_dir= std::filesystem::current_path()/path;

    std::filesystem::current_path(new_dir);

    Tolk_Load();
    if (openal != NULL) {

        LPALCDESTROYCONTEXT alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(openal, "alcDestroyContext");
        LPALCCLOSEDEVICE alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(openal, "alcCloseDevice");
        alcDestroyContext(context);
        alcCloseDevice(device);
        FreeLibrary(openal);
    }
    if (sndfile != NULL)
        FreeLibrary(sndfile);
    sndfile = LoadLibrary(L"sndfile.dll");
    openal = LoadLibrary(L"OpenAL32.dll");
    if (openal != NULL) {
        LPALCOPENDEVICE alcOpenDevice = (LPALCOPENDEVICE)GetProcAddress(openal, "alcOpenDevice");
        LPALCCREATECONTEXT alcCreateContext = (LPALCCREATECONTEXT)GetProcAddress(openal, "alcCreateContext");
        LPALCMAKECONTEXTCURRENT alcMakeContextCurrent = (LPALCMAKECONTEXTCURRENT)GetProcAddress(openal, "alcMakeContextCurrent");

        device = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
    }
    std::filesystem::current_path(current_dir);
}

std::random_device rd;
std::mt19937 gen(rd());
long random(long min, long max) {
    static_assert(std::is_integral<long>::value, "Type must be integral");

    std::uniform_int_distribution<long> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}
double randomDouble(double min, double max) {
    static_assert(std::is_floating_point<double>::value, "Type must be floating point");

    std::uniform_real_distribution<double> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}

bool random_bool() {
    return rand() % 2 == 0;
}
int get_last_error() {
    return 0;
}
void speak(const std::string & text, bool stop) {
    std::wstring textstr = wstr(text);
    Tolk_Speak(textstr.c_str(), stop);
}
void speak_wait(const std::string & text, bool stop) {
    speak(text, stop);
    while (Tolk_IsSpeaking()) {
        update_game_window();
    }
}

void stop_speech() {
        Tolk_Silence();
    }
bool window_closable;
SDL_Event e;
bool show_game_window(const std::string & title,int width, int height, bool closable)
{
    if (win != NULL)
        return false;
    if (reader == L"JAWS") {
        win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_KEYBOARD_GRABBED);
        SDL_SetHint(SDL_HINT_ALLOW_ALT_TAB_WHILE_GRABBED, "1");
    }
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
bool focus_game_window() {
    SDL_SetWindowInputFocus(win);
    return true;
}
void hide_game_window() {
    SDL_StopTextInput();

    SDL_DestroyWindow(win);
}
void set_game_window_title(const std::string & new_title) {
    SDL_SetWindowTitle(win, new_title.c_str());
}
void set_game_window_closable(bool set_closable) {
    window_closable = set_closable;
}
void update_game_window()
{
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT and window_closable == true)
        {
            exit_engine();
        }
        if (e.type == SDL_TEXTINPUT)
            inputtext += e.text.text;

        if (e.type == SDL_KEYDOWN)
        {
            keys[e.key.keysym.sym] = true;
        }
        if (e.type == SDL_KEYUP)
        {
            keys[e.key.keysym.sym] = false;
        }
        if (e.type == SDL_WINDOWEVENT_FOCUS_GAINED)
            window_is_focused == true;
        if (e.type == SDL_WINDOWEVENT_FOCUS_LOST)
            window_is_focused == false;
    }
bool is_game_window_active() {
    return window_is_focused;
}
void exit_engine(int return_number)
{
    if (openal != NULL) {

        LPALCDESTROYCONTEXT alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(openal, "alcDestroyContext");
        LPALCCLOSEDEVICE alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(openal, "alcCloseDevice");
        alcDestroyContext(context);
        alcCloseDevice(device);
        FreeLibrary(openal);
    }
    if(sndfile!=NULL)
    FreeLibrary(sndfile);

    ma_engine_uninit(&sound_engine);

    SDL_StopTextInput();
    SDL_DestroyWindow(win);
win=NULL;
SDL_UnregisterApp();
enet_deinitialize();
Tolk_Unload();
SDL_Quit();
exit(return_number);
}
std::string read_environment_variable(const std::string& path) {
#ifdef _WIN32
    // Use _dupenv_s on Windows
    char* value;
    size_t size;
    if (_dupenv_s(&value, &size, path.c_str()) != 0 || value == nullptr) {
        // Environment variable not found or error occurred
        return "";
    }
    const std::string & result(value);
    free(value);  // Free the allocated memory
    return result;
#else
    // Use getenv for non-Windows platforms
    char* value = std::getenv(path.c_str());
    if (value == nullptr) {
        // Environment variable not found
        return "";
    }
    return const std::string &(value);
#endif
}
bool clipboard_copy_text(const std::string & text) {
    SDL_SetClipboardText(text.c_str());
    return true;
}
std::string clipboard_read_text() {
    return SDL_GetClipboardText();
}
std::string get_input() {
    std::string temp=inputtext;
    inputtext="";
    return temp;
}
bool key_pressed(SDL_Keycode key_code)
{
    if (e.key.state == SDL_PRESSED)
    {
        if (e.key.keysym.sym == key_code and e.key.repeat==0){
            return true;
    }
    }
return false;
}
bool key_released(SDL_Keycode key_code)
{
    if (e.key.state== SDL_RELEASED)
    {
if(e.key.keysym.sym==key_code){
    return true;
}
    }
    return false;
}
bool key_down(SDL_Keycode key_code)
{
    if (keys.find(key_code) != keys.begin()or keys.find(key_code) != keys.end()) {
        return keys[key_code];
    }
return false;
}
bool key_repeat(SDL_Keycode key_code)
{
    if (e.type == SDL_KEYDOWN)
    {
if(e.key.keysym.sym==key_code){
    return true;
}
    }
    return false;
}
bool alert(const std::string & title, const std::string & text, const std::string &button_name)
{
    SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, button_name.c_str()},
    };

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        win,
        title.c_str(),
        text.c_str(),
        SDL_arraysize(buttons),
        buttons,
        NULL
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("Error displaying message box");
        return false;
    }



return true;
}
int question(const std::string& title, const std::string& text) {
    SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No" },
    };

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        win,
        title.c_str(),
        text.c_str(),
        SDL_arraysize(buttons),
        buttons,
        NULL
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("Error displaying message box");
        return -1;
    }

    return buttonid;
}


void set_listener_position(float l_x, float l_y, float l_z) {
    if (openal != NULL) {

        LPALLISTENER3F alListener3f = (LPALLISTENER3F)GetProcAddress(openal, "alListener3f");
        alListener3f(AL_POSITION, l_x, l_y, l_z);
    }
    ma_engine_listener_set_position(&sound_engine, 0, l_x, l_y, l_z);
}
void set_listener_position(ngtvector* v) {
    if (openal != NULL) {

        LPALLISTENER3F alListener3f = (LPALLISTENER3F)GetProcAddress(openal, "alListener3f");
        alListener3f(AL_POSITION, v->x, v->y, v->z);
    }
    ma_engine_listener_set_position(&sound_engine, 0, v->x, v->y, v->z);
}

void wait(int time) {
    timer waittimer;
    int el = 0;
    while (el < time) {
        update_game_window();
        el = waittimer.elapsed_millis();
    }
}
void delay(int ms)
{
SDL_Delay(ms);
}
void set_sound_storage(const std::string & path) {
    sound_path = path;
}
 std::string get_sound_storage() {
    return sound_path;
}
void set_master_volume(float volume) {

    if (volume > 0 or volume < -100)return;
    ma_engine_set_gain_db(&sound_engine, volume);
    if (openal != NULL) {
        LPALLISTENERF alListenerf = (LPALLISTENERF)GetProcAddress(openal, "alListenerf");
        alListenerf(AL_GAIN, static_cast<float>(volume / 2));
    }
}
float get_master_volume() {
    return ma_engine_get_gain_db(&sound_engine);
}
void sound::construct() {
}

void sound::destruct() {
}
bool sound::load(const std::string & filename, bool set3d) {
    std::string result;
    if (sound_path != "") {
        result = sound_path + "/" + filename.c_str();
    }
    else {
        result = filename;
    }
    if (active){
        this->close();
    active = false;
}

    if (set3d)
            ma_sound_init_from_file(&sound_engine, result.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
        else
            ma_sound_init_from_file(&sound_engine, result.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
    ma_sound_set_rolloff(&handle_, 0.1);
        active = true;
        if (sndfile != NULL and openal != NULL) {
        typedef SNDFILE* (cdecl* sf_open_func)(const char*, int, SF_INFO*);
        typedef int(cdecl* sf_close_func)(SNDFILE*);
        typedef sf_count_t(__cdecl* sf_readf_short_func)(SNDFILE*, short*, sf_count_t);
            sf_open_func sf_open = (sf_open_func)GetProcAddress(sndfile, "sf_open");
            sf_close_func sf_close = (sf_close_func)GetProcAddress(sndfile, "sf_close");
            sf_readf_short_func sf_readf_short = (sf_readf_short_func)GetProcAddress(sndfile, "sf_readf_short");

            SNDFILE* sndfile;
            SF_INFO sfinfo;
            std::vector<short> samples;

            if (sf_open && sf_close && sf_readf_short) {
                sndfile = sf_open(result.c_str(), SFM_READ, &sfinfo);
                if (!sndfile) {
                    active = false;
                    return false;
                }

                samples.resize(sfinfo.channels * sfinfo.frames);

                sf_readf_short(sndfile, &samples[0], sfinfo.frames);
                sf_close(sndfile);

                if (sfinfo.channels > 1) {
                    std::vector<short> monoSamples(sfinfo.frames);
                    for (int i = 0; i < sfinfo.frames; i++) {
                        int index = i * sfinfo.channels;
                        monoSamples[i] = (samples[index] + samples[index + 1]) / 2;
                    }
                    samples = monoSamples;
                    sfinfo.channels = 1;
                }
            }
            typedef void (*PFNALGENBUFFERSPROC)(ALsizei, ALuint*);
            typedef void (*PFNALBUFFERDATAPROC)(ALuint, ALenum, const ALvoid*, ALsizei, ALsizei);
            typedef void (*PFNALGENSOURCESPROC)(ALsizei, ALuint*);
            typedef void (*PFNALSOURCEIPROC)(ALuint, ALenum, ALint);
            typedef void (*PFNALSOURCEFPROC)(ALuint, ALenum, ALfloat);
            typedef void (*PFNALSOURCE3FPROC)(ALuint, ALenum, ALfloat, ALfloat, ALfloat);
            PFNALGENBUFFERSPROC alGenBuffers = (PFNALGENBUFFERSPROC)GetProcAddress(openal, "alGenBuffers");
            PFNALBUFFERDATAPROC alBufferData = (PFNALBUFFERDATAPROC)GetProcAddress(openal, "alBufferData");
            PFNALGENSOURCESPROC alGenSources = (PFNALGENSOURCESPROC)GetProcAddress(openal, "alGenSources");
            PFNALSOURCEIPROC alSourcei = (PFNALSOURCEIPROC)GetProcAddress(openal, "alSourcei");
            PFNALSOURCEFPROC alSourcef = (PFNALSOURCEFPROC)GetProcAddress(openal, "alSourcef");
            PFNALSOURCE3FPROC alSource3f = (PFNALSOURCE3FPROC)GetProcAddress(openal, "alSource3f");

            alGenBuffers(1, &buffer_);
            alBufferData(buffer_,
                (sfinfo.channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
                &samples[0],
                static_cast<ALsizei>(sfinfo.frames * sfinfo.channels * sizeof(short)),
                sfinfo.samplerate);

            alGenSources(1, &source_);

            // Set source properties
            alSourcei(source_, AL_BUFFER, buffer_);
            alSourcef(source_, AL_PITCH, 1.0f);
            alSourcef(source_, AL_GAIN, 1.0f);
            alSourcei(source_, AL_LOOPING, AL_FALSE);

            // Set 3D properties if requested
            if (set3d) {
                is_3d_ = true;
                alSource3f(source_, AL_POSITION, 0.0f, 0.0f, 0.0f);
                alSource3f(source_, AL_VELOCITY, 0.0f, 0.0f, 0.0f);

                alSourcei(source_, AL_SOURCE_RELATIVE, AL_FALSE);
                alSourcef(source_, AL_ROLLOFF_FACTOR, 0.1f);
                alSourcei(source_, AL_REFERENCE_DISTANCE, 1);
                alSourcei(source_, AL_MAX_DISTANCE, 500);
            }

            active = true;
            return true;
        }
}


    bool sound::load_from_memory(const std::string & data, bool set3d) {
        return false;
    }

    bool sound::play() {
        if (!active)return false;
        if (audio_system == 0) {
            stop();
            ma_sound_set_looping(&handle_, false);
            ma_sound_start(&handle_);
            return true;
        }
        else {
            if (openal != NULL) {
                typedef void (*PFNALSOURCEIPROC)(ALuint, ALenum, ALint);
                typedef void (*PFNALSOURCEPLAYPROC)(ALuint);
                PFNALSOURCEIPROC alSourcei = (PFNALSOURCEIPROC)GetProcAddress(openal, "alSourcei");
                PFNALSOURCEPLAYPROC alSourcePlay = (PFNALSOURCEPLAYPROC)GetProcAddress(openal, "alSourcePlay");
                alSourcei(source_, AL_LOOPING, AL_FALSE);
                alSourcePlay(source_);
                return true;
            }
}

    }

    bool sound::play_looped() {
        if (!active)return false;
        if (audio_system == 0) {
            stop();

            ma_sound_set_looping(&handle_, true);
            ma_sound_start(&handle_);
            return true;
        }
        else {
            if (openal != NULL) {

                typedef void (*PFNALSOURCEIPROC)(ALuint, ALenum, ALint);
                typedef void (*PFNALSOURCEPLAYPROC)(ALuint);
                PFNALSOURCEIPROC alSourcei = (PFNALSOURCEIPROC)GetProcAddress(openal, "alSourcei");
                PFNALSOURCEPLAYPROC alSourcePlay = (PFNALSOURCEPLAYPROC)GetProcAddress(openal, "alSourcePlay");
                alSourcei(source_, AL_LOOPING, AL_TRUE);
                alSourcePlay(source_);
                return true;
            }
            }
        }
    bool sound::set_faid_parameters(float volume_beg, float volume_end, unsigned int time) {
        if(!active)return false;
        ma_sound_set_fade_in_milliseconds(&handle_, volume_beg, volume_end, time);
        ma_sound_set_fade_start_in_milliseconds(&handle_, volume_beg, volume_end, time, time);
        return true;
    }

    bool sound::pause() {
        if (!active)return false;
        if (audio_system == 0) {
            ma_sound_stop(&handle_);
            return true;
        }
        else {
            if (openal != NULL) {
                typedef void (*PFNALSOURCEPAUSEPROC)(ALuint);
                PFNALSOURCEPAUSEPROC alSourcePause = (PFNALSOURCEPAUSEPROC)GetProcAddress(openal, "alSourcePause");
                alSourcePause(source_);
                return true;
            }
            }
    }

    bool sound::play_wait() {
        this->play();
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
        if (audio_system == 0) {
            ma_sound_stop(&handle_);
            ma_sound_seek_to_pcm_frame(&handle_, 0);
        }
        else {
            if (openal != NULL) {
                typedef void (*PFNALSOURCEIPROC)(ALuint, ALenum, ALint);
                typedef void (*PFNALSOURCESTOPPROC)(ALuint);
                PFNALSOURCEIPROC alSourcei = (PFNALSOURCEIPROC)GetProcAddress(openal, "alSourcei");
                PFNALSOURCESTOPPROC alSourceStop = (PFNALSOURCESTOPPROC)GetProcAddress(openal, "alSourceStop");

                alSourceStop(source_);
                alSourcei(source_, AL_BUFFER, 0);
            }
            }
        return true;
    }
    bool sound::close() {
        if (!active)return false;
            ma_sound_uninit(&handle_);
            if (openal != NULL) {

                typedef ALvoid(AL_APIENTRY* PFNALDELETEBUFFERS)(ALsizei, const ALuint*);
                typedef ALvoid(AL_APIENTRY* PFNALDELETESOURCES)(ALsizei, const ALuint*);
                PFNALDELETEBUFFERS alDeleteBuffers = (PFNALDELETEBUFFERS)GetProcAddress(openal, "alDeleteBuffers");
                PFNALDELETESOURCES alDeleteSources = (PFNALDELETESOURCES)GetProcAddress(openal, "alDeleteSources");

                if (source_) {
                    alDeleteSources(1, &source_);
                    source_ = 0;
                }

                if (buffer_) {
                    alDeleteBuffers(1, &buffer_);
                    buffer_ = 0;
                }
            }
            active = false;
        return true;
    }

    void sound::set_sound_position(float s_x, float s_y, float s_z) {
        if (!active)return;
        ma_sound_set_position(&handle_, s_x, s_y, s_z);
        if (openal != NULL) {
            typedef void (*PFNALSOURCE3FPROC)(ALuint, ALenum, ALfloat, ALfloat, ALfloat);
            PFNALSOURCE3FPROC alSource3f = (PFNALSOURCE3FPROC)GetProcAddress(openal, "alSource3f");
            alSource3f(source_, AL_POSITION, s_x, s_y, s_z);
        }
        }
    void sound::set_sound_position(ngtvector*v) {
        if (!active)return;
        ma_sound_set_position(&handle_, v->x, v->y, v->z);
        if (openal != NULL) {

            typedef void (*PFNALSOURCE3FPROC)(ALuint, ALenum, ALfloat, ALfloat, ALfloat);
            PFNALSOURCE3FPROC alSource3f = (PFNALSOURCE3FPROC)GetProcAddress(openal, "alSource3f");
            alSource3f(source_, AL_POSITION, v->x, v->y, v->z);
        }
        }

    void sound::set_sound_reverb(reverb* r){
        (ma_node_graph*)&sound_engine;
        ma_reverb_node_config rconf;
        ma_uint32 channels;
        ma_uint32 sampleRate;

        channels = ma_engine_get_channels(&sound_engine);
        sampleRate = ma_engine_get_sample_rate(&sound_engine);

         rconf= ma_reverb_node_config_init(channels, sampleRate);
         rconf = r->c;
         ma_reverb_node_init(ma_engine_get_node_graph(&sound_engine), &rconf, NULL, &rev);
         ma_node_attach_output_bus(&rev, 0, ma_engine_get_endpoint(&sound_engine), 0);
         ma_node_attach_output_bus(&handle_, 0, &rev, 0);

    }
    void sound::set_sound_hrtf(bool hrtf) {
        bool state = this->is_playing();
        this->stop();
        if (hrtf and openal!=NULL)
            this->audio_system = 1;
        else 
            this->audio_system = 0;
        if (state == true) {
            this->play();
}
    }
    bool sound::seek(double new_position) {
        if (!active)return false;
        ma_sound_seek_to_pcm_frame(&handle_, static_cast<unsigned int>(new_position*100));
    }

    void sound::cancel_reverb() {
        ma_reverb_node_uninit(&rev, NULL);
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

        float volume;
        if (audio_system == 0) {

            volume = ma_sound_get_volume(&handle_);
            return ma_volume_linear_to_db(volume);

        }
        else {
            if (openal != NULL) {

                typedef void (*PFNALGETSOURCEFPROC)(ALuint, ALenum, ALfloat*);
                PFNALGETSOURCEFPROC alGetSourcef = (PFNALGETSOURCEFPROC)GetProcAddress(openal, "alGetSourcef");
                alGetSourcef(source_, AL_GAIN, &volume);
                return volume;
            }
            }
    }
    void sound::set_volume(double volume) {
        if (!active)return;
        if (volume > 0 or volume < -100)return;
        if(audio_system == 0) {
            ma_sound_set_volume(&handle_, ma_volume_db_to_linear(volume));
        }
        else {
            if (openal != NULL) {

                typedef void (*PFNALSOURCEFPROC)(ALuint, ALenum, ALfloat);
                PFNALSOURCEFPROC alSourcef = (PFNALSOURCEFPROC)GetProcAddress(openal, "alSourcef");

                alSourcef(source_, AL_GAIN, static_cast<float>(volume / 100));
            }
        }
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
        ALenum state;
        if(audio_system==0)
        return ma_sound_is_playing(&handle_);
        else {
            if (openal != NULL) {

                typedef void (*PFNALGETSOURCEIPROC)(ALuint, ALenum, ALint*);
                PFNALGETSOURCEIPROC alGetSourcei = (PFNALGETSOURCEIPROC)GetProcAddress(openal, "alGetSourcei");
                alGetSourcei(source_, AL_SOURCE_STATE, &state);
                return state == AL_PLAYING;
            }
            }
    }
    bool sound::is_paused() const {
        ALenum state;

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
        ma_uint64 length;
//        ma_sound_get_length_in_pcm_frames(&handle_, &length);
    }
        double sound::get_sample_rate() const {
    float rate=0;
    return rate;
}
void timer::construct() {
}

void timer::destruct() {
}
    uint64_t timer::elapsed_seconds() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::seconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_minutes() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::minutes>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::minutes>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_hours() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::hours>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::hours>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_millis() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }

    uint64_t timer::elapsed_micros() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::microseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_nanos() {
        return pausedNanos != 0 ? pausedNanos 
                                : std::chrono::duration_cast<std::chrono::nanoseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    void timer::force_seconds(uint64_t seconds) {
        inittime = std::chrono::steady_clock::now() - std::chrono::seconds(seconds);
        pausedNanos = 0;
    }
    void timer::force_minutes(uint64_t minutes) {
        inittime = std::chrono::steady_clock::now() - std::chrono::minutes(minutes);
        pausedNanos = 0;
    }
    void timer::force_hours(uint64_t hours) {
        inittime = std::chrono::steady_clock::now() - std::chrono::hours(hours);
        pausedNanos = 0;
    }
    void timer::force_millis(uint64_t millis) {
        inittime = std::chrono::steady_clock::now() - std::chrono::milliseconds(millis);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in microseconds
    void timer::force_micros(uint64_t micros) {
        inittime = std::chrono::steady_clock::now() - std::chrono::microseconds(micros);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in nanoseconds
    void timer::force_nanos(uint64_t nanos) {
        inittime = std::chrono::steady_clock::now() - std::chrono::nanoseconds(nanos);
        pausedNanos = 0;
    }

    void timer::restart() {
        inittime = std::chrono::steady_clock::now();
        pausedNanos = 0;
    }
    void timer::pause() {
        if (pausedNanos == 0) {
            pausedNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          std::chrono::steady_clock::now() - inittime).count();
        }
}
bool timer::is_running() {
    return pausedNanos == 0;
}

    void timer::resume() {
        if (pausedNanos != 0) {
            inittime = std::chrono::steady_clock::now() - std::chrono::nanoseconds(pausedNanos);
            pausedNanos = 0;
        }
    }

        void reverb::construct() {
        }

        void reverb::destruct() {
        }



        void reverb::set_input_gain(float input_gain) {
            c.dryVolume = input_gain;
        }
        void reverb::set_reverb_mix(float reverb_mix) {
            c.wetVolume = reverb_mix;
        }       
        void reverb::set_reverb_time(float reverb_time) {
            c.roomSize = reverb_time;
        }
        float reverb::get_input_gain() {
            return c.dryVolume;
        }
        float reverb::get_reverb_mix() {
            return c.wetVolume;
        }
        float reverb::get_reverb_time() {
            return c.roomSize;
        }
        void network_event::construct() {}
        void network_event::destruct() {}

        void network::construct() {}
        void network::destruct() { }
    unsigned int network::connect(const std::string& hostAddress, int port) {
        ENetAddress enetAddress;
        enet_address_set_host(&enetAddress, hostAddress.c_str());
        enetAddress.port = port;

        ENetPeer* peer = enet_host_connect(host, &enetAddress, 1, 0);
        if (peer == NULL) {
            return 0; // Connection failed
        }

        return peer->incomingPeerID;
    }

    bool network::destroy() {
        if (host) {
            enet_host_destroy(host);
            host = NULL;
            return true;
        }
        return false;
    }

        void library::construct() {}
                void library::destruct() {  }
                bool library::load(const std::string & libname) {
                    lib = LoadLibraryA(libname.c_str());
                    if (lib != NULL)
                        return true;
                    return false;
                }
                CScriptDictionary* library::call(const std::string function_name, ...){
                CScriptDictionary* CallResult;
                }
                    void library::unload() {
                    FreeLibrary(lib);
                }
                void instance::construct() {}
                void instance::destruct() {  }

                bool instance::is_running() {
                            DWORD result = WaitForSingleObject(mutex, 0);
                            if (result == WAIT_OBJECT_0) {
                                ReleaseMutex(mutex);
                                return false;
                            }
                            return true;
                        }

user_idle::user_idle() {}
uint64_t user_idle::elapsed_millis() {
    return get_idle_time();
}
uint64_t user_idle::elapsed_seconds() {
    return get_idle_time() / 1000;
}
uint64_t user_idle::elapsed_minutes() {
    return elapsed_seconds() / 60;
}
uint64_t user_idle::elapsed_hours() {
    return elapsed_minutes() / 60;
}
uint64_t user_idle::elapsed_days() {
    return elapsed_hours() / 24;
}
uint64_t user_idle::elapsed_weeks() {
    return elapsed_days() / 7;
}
uint64_t user_idle::get_idle_time() {
    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
    GetLastInputInfo(&lii);
    DWORD currentTick = GetTickCount();
    return (currentTick - lii.dwTime);
}

uint64_t get_time_stamp_millis() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

uint64_t get_time_stamp_seconds() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return seconds;
}
void ngtvector::construct() {}
void ngtvector::destruct() {}
void sqlite3statement::construct() {}
void sqlite3statement::destruct() {}
void ngtsqlite3::construct() {}
void ngtsqlite3::destruct() {}

int sqlite3statement::step() { return sqlite3_step(stmt); }
    int sqlite3statement::reset() { return sqlite3_reset(stmt); }
    std::string sqlite3statement::get_expanded_sql_statement() const { return sqlite3_expanded_sql(stmt); }
    std::string sqlite3statement::get_sql_statement() const { return sqlite3_sql(stmt); }
    int sqlite3statement::get_bind_param_count() const { return sqlite3_bind_parameter_count(stmt); }
    int sqlite3statement::get_column_count() const { return sqlite3_column_count(stmt); }
    int sqlite3statement::bind_blob(int index, const std::string& value, bool copy) { return sqlite3_bind_blob(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::bind_double(int index, double value) { return sqlite3_bind_double(stmt, index, value); }
    int sqlite3statement::bind_int(int index, int value) { return sqlite3_bind_int(stmt, index, value); }
    int sqlite3statement::bind_int64(int index, int64_t value) { return sqlite3_bind_int64(stmt, index, value); }
    int sqlite3statement::bind_null(int index) { return sqlite3_bind_null(stmt, index); }
    int sqlite3statement::bind_param_index(const std::string& name) { return sqlite3_bind_parameter_index(stmt, name.c_str()); }
    std::string sqlite3statement::bind_param_name(int index) { return sqlite3_bind_parameter_name(stmt, index); }
    int sqlite3statement::bind_text(int index, const std::string& value, bool copy) { return sqlite3_bind_text(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::clear_bindings() { return sqlite3_clear_bindings(stmt); }
    std::string sqlite3statement::column_blob(int index) { return std::string(reinterpret_cast<const char*>(sqlite3_column_blob(stmt, index)), sqlite3_column_bytes(stmt, index)); }
    int sqlite3statement::column_bytes(int index) { return sqlite3_column_bytes(stmt, index); }
    std::string sqlite3statement::column_decltype(int index) { return sqlite3_column_decltype(stmt, index); }
    double sqlite3statement::column_double(int index) { return sqlite3_column_double(stmt, index); }
    int sqlite3statement::column_int(int index) { return sqlite3_column_int(stmt, index); }
    int64_t sqlite3statement::column_int64(int index) { return sqlite3_column_int64(stmt, index); }
    std::string sqlite3statement::column_name(int index) { return sqlite3_column_name(stmt, index); }
    int sqlite3statement::column_type(int index) { return sqlite3_column_type(stmt, index); }
    std::string sqlite3statement::column_text(int index) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, index)); }


    int ngtsqlite3::close() { return sqlite3_close(db); }

    int ngtsqlite3::open(const std::string& filename, int flags) { return sqlite3_open_v2(filename.c_str(), &db, flags, nullptr); }
    sqlite3statement* ngtsqlite3::prepare(const std::string& name, int& out) {
        sqlite3statement* statement = new sqlite3statement();
        if (sqlite3_prepare_v2(db, name.c_str(), -1, &statement->stmt, 0) != SQLITE_OK)
        {
            out = sqlite3_errcode(db);
            return nullptr;
        }
        return statement;
    }

    int ngtsqlite3::execute(const std::string& sql)
    {
        return sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    }

    int64_t ngtsqlite3::get_rows_changed() const { return sqlite3_changes(db); }

    int64_t ngtsqlite3::get_total_rows_changed() const { return sqlite3_total_changes(db); }

    int ngtsqlite3::limit(int id, int val) { return sqlite3_limit(db, id, val); }

    int ngtsqlite3::set_authorizer(sqlite3_authorizer* callback, const std::string& arg) { return 0; }

    int64_t ngtsqlite3::get_last_insert_rowid() const { return sqlite3_last_insert_rowid(db); }

    void ngtsqlite3::set_last_insert_rowid(int64_t id) { sqlite3_set_last_insert_rowid(db, id); }

    int ngtsqlite3::get_last_error() { return sqlite3_errcode(db); }

    std::string ngtsqlite3::get_last_error_text() { return sqlite3_errmsg(db); }

    bool ngtsqlite3::get_active() const { return db != nullptr; }

