//NGTAUDIO
#include "sound.h"
#include "ngt.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "AL/al.h"
#include "AL/alc.h"
bool inited= false;
HINSTANCE openal;
HINSTANCE sndfile;
typedef ALCdevice* (*LPALCOPENDEVICE)(const ALCchar*);
typedef ALCcontext* (*LPALCCREATECONTEXT)(ALCdevice*, const ALCint*);
typedef ALCboolean(*LPALCMAKECONTEXTCURRENT)(ALCcontext*);
typedef void (*LPALCDESTROYCONTEXT)(ALCcontext*);
typedef ALCboolean(*LPALCCLOSEDEVICE)(ALCdevice*);
typedef void (*LPALLISTENER3F)(ALenum, ALfloat, ALfloat, ALfloat);
typedef void (*LPALLISTENERF)(ALenum, ALfloat);
ALCdevice* device = NULL;
ALCcontext* context = NULL;
ma_engine sound_engine;
void soundsystem_init() {
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
    ma_engine_init(NULL, &sound_engine);


}
void soundsystem_free() {
    if (openal != NULL) {

        LPALCDESTROYCONTEXT alcDestroyContext = (LPALCDESTROYCONTEXT)GetProcAddress(openal, "alcDestroyContext");
        LPALCCLOSEDEVICE alcCloseDevice = (LPALCCLOSEDEVICE)GetProcAddress(openal, "alcCloseDevice");
        alcDestroyContext(context);
        alcCloseDevice(device);
        FreeLibrary(openal);
    }
    if (sndfile != NULL)
        FreeLibrary(sndfile);

    ma_engine_uninit(&sound_engine);


}
    std::string sound_path;
void set_sound_storage(const std::string& path) {
    sound_path = path;
}
std::string get_sound_storage() {
    return sound_path;
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

class sound {
public:
    bool is_3d_;
    bool playing = false, paused = false, active = false;
    ALuint buffer_;
    ALuint source_;
    short audiosystem = 0;
    ma_sound handle_;
    void construct() {
    }

    void destruct() {
    }

    sound() {
        if (!inited) {
            soundsystem_init();
            inited = true;
        }
    }

    bool load(const std::string& filename, bool set3d) {
        std::string result;
        if (sound_path != "") {
            result = sound_path + "/" + filename.c_str();
        }
        else {
            result = filename;
        }
        if (active) {
            this->close();
            active = false;
        }
        if (set3d)
            ma_sound_init_from_file(&sound_engine, result.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
        else
            ma_sound_init_from_file(&sound_engine, result.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
        ma_sound_set_rolloff(&handle_, 0.1);
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
    bool load_from_memory(const std::string& data, bool set3d) {
        return false;

    }

    bool play() {
        if (!active)return false;
        if (audiosystem == 0) {
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
    bool play_looped() {
        if (!active)return false;
        if (audiosystem == 0) {

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
    bool set_faid_parameters(float volume_beg, float volume_end, unsigned int time) {
        if (!active)return false;
        ma_sound_set_fade_in_milliseconds(&handle_, volume_beg, volume_end, time);
        ma_sound_set_fade_start_in_milliseconds(&handle_, volume_beg, volume_end, time, time);
        return true;
    }

    bool pause() {
        if (!active)return false;
        if (audiosystem == 0) {
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
    bool play_wait() {
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

    bool stop() {
        if (!active)return false;
        if (audiosystem == 0) {
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
    bool close() {
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

    void set_position(float s_x, float s_y, float s_z) {
        if (!active)return;
        ma_sound_set_position(&handle_, s_x, s_y, s_z);
        if (openal != NULL) {
            typedef void (*PFNALSOURCE3FPROC)(ALuint, ALenum, ALfloat, ALfloat, ALfloat);
            PFNALSOURCE3FPROC alSource3f = (PFNALSOURCE3FPROC)GetProcAddress(openal, "alSource3f");
            alSource3f(source_, AL_POSITION, s_x, s_y, s_z);
        }
    }
    void set_position(ngtvector* v) {
        if (!active)return;
        ma_sound_set_position(&handle_, v->x, v->y, v->z);
        if (openal != NULL) {

            typedef void (*PFNALSOURCE3FPROC)(ALuint, ALenum, ALfloat, ALfloat, ALfloat);
            PFNALSOURCE3FPROC alSource3f = (PFNALSOURCE3FPROC)GetProcAddress(openal, "alSource3f");
            alSource3f(source_, AL_POSITION, v->x, v->y, v->z);
        }
    }


    void set_hrtf(bool hrtf) {
        bool state = this->is_playing();
        this->stop();
        if (hrtf and openal != NULL)
            this->audiosystem = 1;
        else
            this->audiosystem = 0;
        if (state == true) {
            this->play();
        }

}
    void set_reverb(float dry, float wet, float time) {
    }

    bool seek(double new_position) {
        if (!active)return false;
        ma_sound_seek_to_pcm_frame(&handle_, static_cast<unsigned int>(new_position * 100));
        return true;
    }

    void cancel_reverb() {
    }

    double get_pan() const {
        if (!active)return -17435;

        double pan=0;
        pan = ma_sound_get_pan(&handle_);
        return pan * 100;
    }

    void set_pan(double pan) {
        if (!active)return;
        ma_sound_set_pan(&handle_, pan / 100);
    }

    double get_volume() const {
        if (!active)return -17435;

        float volume=0;
        if (audiosystem == 0) {

            volume = ma_sound_get_volume(&handle_);
            return ma_volume_linear_to_db(volume);

        }
        else {
            if (openal != NULL) {

                typedef void (*PFNALGETSOURCEFPROC)(ALuint, ALenum, ALfloat*);
                PFNALGETSOURCEFPROC alGetSourcef = (PFNALGETSOURCEFPROC)GetProcAddress(openal, "alGetSourcef");
                alGetSourcef(source_, AL_GAIN, &volume);
            }
        }

        return volume;
        }
    void set_volume(double volume) {
        if (!active)return;
        if (volume > 0 or volume < -100)return;
        if (audiosystem == 0) {
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
        double get_pitch() const {
        if (!active)return -17435;
        float pitch = 0;
        if (audiosystem == 0) {
            pitch = ma_sound_get_pitch(&handle_);
        }
        else{
                if (openal != NULL) {

                    typedef void (*PFNALGETSOURCEFPROC)(ALuint, ALenum, ALfloat*);
                    PFNALGETSOURCEFPROC alGetSourcef = (PFNALGETSOURCEFPROC)GetProcAddress(openal, "alGetSourcef");
                    alGetSourcef(source_, AL_PITCH, & pitch);
                }
            }

        return pitch * 100;
    }

    void set_pitch(double pitch) {
        if (!active)return;
if(audiosystem==0)
        ma_sound_set_pitch(&handle_, pitch / 100);
else {
    if (openal != NULL) {

        typedef void (*PFNALSOURCEFPROC)(ALuint, ALenum, ALfloat);
        PFNALSOURCEFPROC alSourcef = (PFNALSOURCEFPROC)GetProcAddress(openal, "alSourcef");

        alSourcef(source_, AL_PITCH, pitch/100);
    }

}
    }

    bool is_active() const {
        return active;
    }

    bool is_playing() const {
        if (!active)return false;
        ALenum state;
        if (audiosystem == 0) {
            return ma_sound_is_playing(&handle_);
        }
        else {
            if (openal != NULL) {

                typedef void (*PFNALGETSOURCEIPROC)(ALuint, ALenum, ALint*);
                PFNALGETSOURCEIPROC alGetSourcei = (PFNALGETSOURCEIPROC)GetProcAddress(openal, "alGetSourcei");
                alGetSourcei(source_, AL_SOURCE_STATE, &state);
                return state == AL_PLAYING;
            }
        }

        return true;
    }
    bool is_paused() const {
        if (!active)return false;
        return paused;
    }

    double get_position() const {
        if (!active)return -17435;

        double position=0;
        return position;
    }

    double get_length() const {
        if (!active)return-17435;
    }
    double get_sample_rate() const {
        float rate = 0;
        return rate;
    }

};
sound* fsound() { return new sound; }

void register_sound(asIScriptEngine* engine) {
    engine->RegisterGlobalFunction("void set_sound_storage(const string &in)", asFUNCTION(set_sound_storage), asCALL_CDECL);

    engine->RegisterGlobalFunction("string get_sound_storage()", asFUNCTION(get_sound_storage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_master_volume(float)", asFUNCTION(set_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("float get_master_volume()", asFUNCTION(get_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(float, float, float)", asFUNCTIONPR(set_listener_position, (float, float, float), void), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_listener_position(vector@=null)", asFUNCTIONPR(set_listener_position, (ngtvector*), void), asCALL_CDECL);

    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_REF);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_FACTORY, "sound@ s()", asFUNCTION(fsound), asCALL_CDECL);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_ADDREF, "void f()", asMETHOD(sound, construct), asCALL_THISCALL);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_RELEASE, "void f()", asMETHOD(sound, destruct), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool load(const string &in, bool=false)", asMETHOD(sound, load), asCALL_THISCALL);
    //    engine->RegisterObjectMethod("sound", "bool load_from_memory(const string &in, bool=false)", asMETHOD(sound, load), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play()", asMETHOD(sound, play), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_looped()", asMETHOD(sound, play_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool pause()", asMETHOD(sound, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_wait()", asMETHOD(sound, play_wait), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool set_faid_parameters(float, float, uint)", asMETHOD(sound, set_faid_parameters), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stop()", asMETHOD(sound, stop), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool close()", asMETHOD(sound, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_position(float, float, float)", asMETHODPR(sound, set_position, (float, float, float), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_position(vector@=null)", asMETHODPR(sound, set_position, (ngtvector*), void), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_reverb(float, float, float)", asMETHOD(sound, set_reverb), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "void set_hrtf(bool)const property", asMETHOD(sound, set_hrtf), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool seek(double)", asMETHOD(sound, seek), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void cancel_reverb()", asMETHOD(sound, cancel_reverb), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "double get_pan() const property", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(double)const property", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_volume() const property", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(double)const property", asMETHOD(sound, set_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_pitch() const property", asMETHOD(sound, get_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch(double)const property", asMETHOD(sound, set_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_active() const property", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_playing() const property", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_paused() const property", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_position() const property", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_length() const property", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "double get_sample_rate() const property", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);

}