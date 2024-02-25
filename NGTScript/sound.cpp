//NGTAUDIO
#include "sound.h"
#include "ngt.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdint.h> /* Required for uint32_t which is used by STEAMAUDIO_VERSION. That dependency needs to be removed from Steam Audio - use IPLuint32 or "unsigned int" instead! */

#include "phonon.h" /* Steam Audio */

#define FORMAT      ma_format_f32   /* Must be floating point. */
#define CHANNELS    2               /* Must be stereo for this example. */
#define SAMPLE_RATE 44100

static ma_result ma_result_from_IPLerror(IPLerror error)
{
    switch (error)
    {
    case IPL_STATUS_SUCCESS:      return MA_SUCCESS;
    case IPL_STATUS_OUTOFMEMORY:  return MA_OUT_OF_MEMORY;
    case IPL_STATUS_INITIALIZATION:
    case IPL_STATUS_FAILURE:
    default: return MA_ERROR;
    }
}


typedef struct
{
    ma_node_config nodeConfig;
    ma_uint32 channelsIn;
    IPLAudioSettings iplAudioSettings;
    IPLContext iplContext;
    IPLHRTF iplHRTF;   /* There is one HRTF object to many binaural effect objects. */
} ma_steamaudio_binaural_node_config;

MA_API ma_steamaudio_binaural_node_config ma_steamaudio_binaural_node_config_init(ma_uint32 channelsIn, IPLAudioSettings iplAudioSettings, IPLContext iplContext, IPLHRTF iplHRTF);


typedef struct
{
    ma_node_base baseNode;
    IPLAudioSettings iplAudioSettings;
    IPLContext iplContext;
    IPLHRTF iplHRTF;
    IPLBinauralEffect iplEffect;
    ma_vec3f direction;
    float* ppBuffersIn[2];      /* Each buffer is an offset of _pHeap. */
    float* ppBuffersOut[2];     /* Each buffer is an offset of _pHeap. */
    void* _pHeap;
} ma_steamaudio_binaural_node;

MA_API ma_result ma_steamaudio_binaural_node_init(ma_node_graph* pNodeGraph, const ma_steamaudio_binaural_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_steamaudio_binaural_node* pBinauralNode);
MA_API void ma_steamaudio_binaural_node_uninit(ma_steamaudio_binaural_node* pBinauralNode, const ma_allocation_callbacks* pAllocationCallbacks);
MA_API ma_result ma_steamaudio_binaural_node_set_direction(ma_steamaudio_binaural_node* pBinauralNode, float x, float y, float z);


MA_API ma_steamaudio_binaural_node_config ma_steamaudio_binaural_node_config_init(ma_uint32 channelsIn, IPLAudioSettings iplAudioSettings, IPLContext iplContext, IPLHRTF iplHRTF)
{
    ma_steamaudio_binaural_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();
    config.channelsIn = channelsIn;
    config.iplAudioSettings = iplAudioSettings;
    config.iplContext = iplContext;
    config.iplHRTF = iplHRTF;

    return config;
}


static void ma_steamaudio_binaural_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_steamaudio_binaural_node* pBinauralNode = (ma_steamaudio_binaural_node*)pNode;
    IPLBinauralEffectParams binauralParams;
    IPLAudioBuffer inputBufferDesc;
    IPLAudioBuffer outputBufferDesc;
    ma_uint32 totalFramesToProcess = *pFrameCountOut;
    ma_uint32 totalFramesProcessed = 0;

    binauralParams.direction.x = pBinauralNode->direction.x;
    binauralParams.direction.y = pBinauralNode->direction.y;
    binauralParams.direction.z = pBinauralNode->direction.z;
    binauralParams.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
    binauralParams.spatialBlend = 1.0f;
    binauralParams.hrtf = pBinauralNode->iplHRTF;
    binauralParams.peakDelays = NULL;
    inputBufferDesc.numChannels = (IPLint32)ma_node_get_input_channels(pNode, 0);

    /* We'll run this in a loop just in case our deinterleaved buffers are too small. */
    outputBufferDesc.numSamples = pBinauralNode->iplAudioSettings.frameSize;
    outputBufferDesc.numChannels = 2;
    outputBufferDesc.data = pBinauralNode->ppBuffersOut;

    while (totalFramesProcessed < totalFramesToProcess) {
        ma_uint32 framesToProcessThisIteration = totalFramesToProcess - totalFramesProcessed;
        if (framesToProcessThisIteration > (ma_uint32)pBinauralNode->iplAudioSettings.frameSize) {
            framesToProcessThisIteration = (ma_uint32)pBinauralNode->iplAudioSettings.frameSize;
        }

        if (inputBufferDesc.numChannels == 1) {
            /* Fast path. No need for deinterleaving since it's a mono stream. */
            pBinauralNode->ppBuffersIn[0] = (float*)ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, 1);
        }
        else {
            /* Slow path. Need to deinterleave the input data. */
            ma_deinterleave_pcm_frames(ma_format_f32, inputBufferDesc.numChannels, framesToProcessThisIteration, ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, inputBufferDesc.numChannels), (void**)pBinauralNode->ppBuffersIn);
        }

        inputBufferDesc.data = pBinauralNode->ppBuffersIn;
        inputBufferDesc.numSamples = (IPLint32)framesToProcessThisIteration;

        /* Apply the effect. */
        iplBinauralEffectApply(pBinauralNode->iplEffect, &binauralParams, &inputBufferDesc, &outputBufferDesc);

        /* Interleave straight into the output buffer. */
        ma_interleave_pcm_frames(ma_format_f32, 2, framesToProcessThisIteration, (const void**)pBinauralNode->ppBuffersOut, ma_offset_pcm_frames_ptr_f32(ppFramesOut[0], totalFramesProcessed, 2));

        /* Advance. */
        totalFramesProcessed += framesToProcessThisIteration;
    }

    (void)pFrameCountIn;    /* Unused. */
}

static ma_node_vtable g_ma_steamaudio_binaural_node_vtable =
{
    ma_steamaudio_binaural_node_process_pcm_frames,
    NULL,
    1,  /* 1 input channel. */
1    ,  /* 1 output channel. */
    0
};

MA_API ma_result ma_steamaudio_binaural_node_init(ma_node_graph* pNodeGraph, const ma_steamaudio_binaural_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_steamaudio_binaural_node* pBinauralNode)
{
    ma_result result;
    ma_node_config baseConfig;
    ma_uint32 channelsIn;
    ma_uint32 channelsOut;
    IPLBinauralEffectSettings iplBinauralEffectSettings;
    size_t heapSizeInBytes;

    if (pBinauralNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pBinauralNode);

    if (pConfig == NULL || pConfig->iplAudioSettings.frameSize == 0 || pConfig->iplContext == NULL || pConfig->iplHRTF == NULL) {
        return MA_INVALID_ARGS;
    }

    /* Steam Audio only supports mono and stereo input. */
    if (pConfig->channelsIn < 1 || pConfig->channelsIn > 2) {
        return MA_INVALID_ARGS;
    }

    channelsIn = pConfig->channelsIn;
    channelsOut = 2;    /* Always stereo output. */

    baseConfig = ma_node_config_init();
    baseConfig.vtable = &g_ma_steamaudio_binaural_node_vtable;
    baseConfig.pInputChannels = &channelsIn;
    baseConfig.pOutputChannels = &channelsOut;
    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pBinauralNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    pBinauralNode->iplAudioSettings = pConfig->iplAudioSettings;
    pBinauralNode->iplContext = pConfig->iplContext;
    pBinauralNode->iplHRTF = pConfig->iplHRTF;

    MA_ZERO_OBJECT(&iplBinauralEffectSettings);
    iplBinauralEffectSettings.hrtf = pBinauralNode->iplHRTF;

    result = ma_result_from_IPLerror(iplBinauralEffectCreate(pBinauralNode->iplContext, &pBinauralNode->iplAudioSettings, &iplBinauralEffectSettings, &pBinauralNode->iplEffect));
    if (result != MA_SUCCESS) {
        ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
        return result;
    }

    heapSizeInBytes = 0;

    /*
    Unfortunately Steam Audio uses deinterleaved buffers for everything so we'll need to use some
    intermediary buffers. We'll allocate one big buffer on the heap and then use offsets. We'll
    use the frame size from the IPLAudioSettings structure as a basis for the size of the buffer.
    */
    heapSizeInBytes += sizeof(float) * channelsOut * pBinauralNode->iplAudioSettings.frameSize; /* Output buffer. */
    heapSizeInBytes += sizeof(float) * channelsIn * pBinauralNode->iplAudioSettings.frameSize; /* Input buffer. */

    pBinauralNode->_pHeap = ma_malloc(heapSizeInBytes, pAllocationCallbacks);
    if (pBinauralNode->_pHeap == NULL) {
        iplBinauralEffectRelease(&pBinauralNode->iplEffect);
        ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
        return MA_OUT_OF_MEMORY;
    }

    pBinauralNode->ppBuffersOut[0] = (float*)pBinauralNode->_pHeap;
    pBinauralNode->ppBuffersOut[1] = (float*)ma_offset_ptr(pBinauralNode->_pHeap, sizeof(float) * pBinauralNode->iplAudioSettings.frameSize);

    {
        ma_uint32 iChannelIn;
        for (iChannelIn = 0; iChannelIn < channelsIn; iChannelIn += 1) {
            pBinauralNode->ppBuffersIn[iChannelIn] = (float*)ma_offset_ptr(pBinauralNode->_pHeap, sizeof(float) * pBinauralNode->iplAudioSettings.frameSize * (channelsOut + iChannelIn));
        }
    }

    return MA_SUCCESS;
}

MA_API void ma_steamaudio_binaural_node_uninit(ma_steamaudio_binaural_node* pBinauralNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    if (pBinauralNode == NULL) {
        return;
    }

    /* The base node is always uninitialized first. */
    ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);

    /*
    The Steam Audio objects are deleted after the base node. This ensures the base node is removed from the graph
    first to ensure these objects aren't getting used by the audio thread.
    */
    iplBinauralEffectRelease(&pBinauralNode->iplEffect);
    ma_free(pBinauralNode->_pHeap, pAllocationCallbacks);
}

MA_API ma_result ma_steamaudio_binaural_node_set_direction(ma_steamaudio_binaural_node* pBinauralNode, float x, float y, float z)
{
    if (pBinauralNode == NULL) {
        return MA_INVALID_ARGS;
    }

    pBinauralNode->direction.x = x;
    pBinauralNode->direction.y = y;
    pBinauralNode->direction.z = z;

    return MA_SUCCESS;
}



IPLAudioSettings iplAudioSettings;
IPLContextSettings iplContextSettings;
IPLContext iplContext;
IPLHRTFSettings iplHRTFSettings;
IPLHRTF iplHRTF;


bool inited= false;
ma_engine sound_engine;

ma_engine_config engineConfig;
void soundsystem_init() {
    engineConfig = ma_engine_config_init();
    engineConfig.channels = CHANNELS;
    engineConfig.sampleRate = SAMPLE_RATE;
    engineConfig.periodSizeInFrames = 256;

    ma_engine_init(&engineConfig, &sound_engine);
    MA_ZERO_OBJECT(&iplAudioSettings);
    iplAudioSettings.samplingRate = ma_engine_get_sample_rate(&sound_engine);

    iplAudioSettings.frameSize = engineConfig.periodSizeInFrames;


    /* IPLContext */
    MA_ZERO_OBJECT(&iplContextSettings);
    iplContextSettings.version = STEAMAUDIO_VERSION;
    //    iplContextSettings.flags = IPL_CONTEXTFLAGS_VALIDATION;
    ma_result_from_IPLerror(iplContextCreate(&iplContextSettings, &iplContext));
    /* IPLHRTF */
    MA_ZERO_OBJECT(&iplHRTFSettings);
    iplHRTFSettings.type = IPL_HRTFTYPE_DEFAULT;
    iplHRTFSettings.volume = 1.0f;

    ma_result_from_IPLerror(iplHRTFCreate(iplContext, &iplAudioSettings, &iplHRTFSettings, &iplHRTF));



}
void soundsystem_free() {
    ma_engine_uninit(&sound_engine);


}
    std::string sound_path;
void set_sound_storage(const std::string& path) {
    sound_path = path;
}
std::string get_sound_storage() {
    return sound_path;
}
void set_master_volume(float volume) {

    if (volume > 0 or volume < -100)return;
    ma_engine_set_gain_db(&sound_engine, volume);
}
float get_master_volume() {
    return ma_engine_get_gain_db(&sound_engine);

}

class sound {
public:
    bool is_3d_;
    bool playing = false, paused = false, active = false;
    ma_sound handle_;
    ma_steamaudio_binaural_node g_binauralNode;   /* The echo effect is achieved using a delay node. */
    ma_steamaudio_binaural_node_config binauralNodeConfig;
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
            active = true;
            return true;
        }
    bool load_from_memory(const std::string& data, bool set3d) {
        return false;

    }

    bool play() {
        if (!active)return false;
            ma_sound_set_looping(&handle_, false);
            ma_sound_start(&handle_);
            return true;
        }
    bool play_looped() {
        if (!active)return false;

            ma_sound_set_looping(&handle_, true);
            ma_sound_start(&handle_);
            return true;
        }
    bool set_faid_parameters(float volume_beg, float volume_end, unsigned int time) {
        if (!active)return false;
        ma_sound_set_fade_in_milliseconds(&handle_, volume_beg, volume_end, time);
        ma_sound_set_fade_start_in_milliseconds(&handle_, volume_beg, volume_end, time, time);
        return true;
    }

    bool pause() {
        if (!active)return false;
            ma_sound_stop(&handle_);
            return true;
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
            ma_sound_stop(&handle_);
            ma_sound_seek_to_pcm_frame(&handle_, 0);
        }
    bool close() {
        if (!active)return false;
        ma_sound_uninit(&handle_);
        active = false;
        return true;
    }

    void set_position(float l_x, float l_y, float l_z, float s_x, float s_y, float s_z) {
        if (!active)return;

        ma_vec3f direction;
        ma_engine_listener_set_position(&sound_engine, 0, l_x, l_y, l_z);
        ma_sound_set_position(&handle_, s_x, s_y, s_z);
        ma_vec3f relativePos;
            direction=ma_vec3f_init_3f(0, 0, -1);

            direction=ma_vec3f_init_3f(0, 0, -1);
            ma_spatializer_get_relative_position_and_direction(&handle_.engineNode.spatializer, &sound_engine.listeners[ma_sound_get_listener_index(&handle_)], &relativePos, NULL);

        direction=ma_vec3f_normalize(relativePos);

        ma_steamaudio_binaural_node_set_direction(&g_binauralNode, direction.x, direction.y, direction.z);

    }
    void set_position(ngtvector* listener, ngtvector* source) {
        if (!active)return;
        ma_vec3f direction;

        ma_engine_listener_set_position(&sound_engine, 0, listener->x, listener->y, listener->z);
        ma_sound_set_position(&handle_, source->x, source->y, source->z);
        ma_vec3f relativePos;
        direction = ma_vec3f_init_3f(0, 0, -1);

        direction = ma_vec3f_init_3f(0, 0, -1);
        ma_spatializer_get_relative_position_and_direction(&handle_.engineNode.spatializer, &sound_engine.listeners[ma_sound_get_listener_index(&handle_)], &relativePos, NULL);

        direction = ma_vec3f_normalize(relativePos);

        ma_steamaudio_binaural_node_set_direction(&g_binauralNode, direction.x, direction.y, direction.z);

    }
    void set_hrtf(bool hrtf) {
        if (!active)return;
        if (hrtf) {

            binauralNodeConfig = ma_steamaudio_binaural_node_config_init(CHANNELS, iplAudioSettings, iplContext, iplHRTF);

            ma_steamaudio_binaural_node_init(ma_engine_get_node_graph(&sound_engine), &binauralNodeConfig, NULL, &g_binauralNode);
            /* Connect the output of the delay node to the input of the endpoint. */
            ma_node_attach_output_bus(&g_binauralNode, 0, ma_engine_get_endpoint(&sound_engine), 0);

            ma_node_attach_output_bus(&handle_, 0, &g_binauralNode, 0);

            ma_sound_set_directional_attenuation_factor(&handle_, 0);

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

        float volume = 0;

        volume = ma_sound_get_volume(&handle_);
        return ma_volume_linear_to_db(volume);
    }
            void set_volume(double volume) {
        if (!active)return;
        if (volume > 0 or volume < -100)return;
            ma_sound_set_volume(&handle_, ma_volume_db_to_linear(volume));
        }
        double get_pitch() const {
        if (!active)return -17435;
        float pitch = 0;
            pitch = ma_sound_get_pitch(&handle_);
        return pitch * 100;
    }

        void set_pitch(double pitch) {
            if (!active)return;
            ma_sound_set_pitch(&handle_, pitch / 100);
        }
        bool is_active() const {
        return active;
    }

    bool is_playing() const {
        if (!active)return false;
            return ma_sound_is_playing(&handle_);
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
    engine->RegisterObjectMethod("sound", "void set_position(float, float, float, float, float, float)", asMETHODPR(sound, set_position, (float, float, float, float, float, float), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_position(vector@=null, vector@=null)", asMETHODPR(sound, set_position, (ngtvector*, ngtvector*), void), asCALL_THISCALL);

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