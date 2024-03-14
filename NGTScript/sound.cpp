//NGTAUDIO
#define NOMINMAX
#define CURL_STATICLIB
#include "curl/curl.h"
#include "sound.h"
#include "ngt.h"
#include <thread>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdint.h> /* Required for uint32_t which is used by STEAMAUDIO_VERSION. That dependency needs to be removed from Steam Audio - use IPLuint32 or "unsigned int" instead! */
#include<algorithm>
#include "phonon.h" /* Steam Audio */
#define FORMAT      ma_format_f32   /* Must be floating point. */
#define CHANNELS    2               /* Must be stereo for this example. */
#define SAMPLE_RATE 44100
#define VERBLIB_IMPLEMENTATION

#include "fx/verblib.h"

#ifdef __cplusplus
extern "C" {
#endif

    /*
    The reverb node has one input and one output.
    */
    typedef struct
    {
        ma_node_config nodeConfig;
        ma_uint32 channels;         /* The number of channels of the source, which will be the same as the output. Must be 1 or 2. */
        ma_uint32 sampleRate;
        float roomSize;
        float damping;
        float width;
        float wetVolume;
        float dryVolume;
        float mode;
    } ma_reverb_node_config;

    MA_API ma_reverb_node_config ma_reverb_node_config_init(ma_uint32 channels, ma_uint32 sampleRate);


    typedef struct
    {
        ma_node_base baseNode;
        verblib reverb;
    } ma_reverb_node;

    MA_API ma_result ma_reverb_node_init(ma_node_graph* pNodeGraph, const ma_reverb_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_reverb_node* pReverbNode);
    MA_API void ma_reverb_node_uninit(ma_reverb_node* pReverbNode, const ma_allocation_callbacks* pAllocationCallbacks);

#ifdef __cplusplus
}
#endif


MA_API ma_reverb_node_config ma_reverb_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float dry, float wet, float room_size)
{
    ma_reverb_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_reverb_node_init(). */
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.roomSize = room_size;
    config.damping = verblib_initialdamp;
    config.width = verblib_initialwidth;
    config.wetVolume = wet;
    config.dryVolume = dry;
    config.mode = verblib_initialmode;

    return config;
}


static void ma_reverb_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_reverb_node* pReverbNode = (ma_reverb_node*)pNode;

    (void)pFrameCountIn;

    verblib_process(&pReverbNode->reverb, ppFramesIn[0], ppFramesOut[0], *pFrameCountOut);
}

static ma_node_vtable g_ma_reverb_node_vtable =
{
    ma_reverb_node_process_pcm_frames,
    NULL,
    1,  /* 1 input channel. */
    1,  /* 1 output channel. */
0
};

MA_API ma_result ma_reverb_node_init(ma_node_graph* pNodeGraph, const ma_reverb_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_reverb_node* pReverbNode)
{
    ma_result result;
    ma_node_config baseConfig;

    if (pReverbNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pReverbNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    if (verblib_initialize(&pReverbNode->reverb, (unsigned long)pConfig->sampleRate, (unsigned int)pConfig->channels) == 0) {
        return MA_INVALID_ARGS;
    }

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_reverb_node_vtable;
    baseConfig.pInputChannels = &pConfig->channels;
    baseConfig.pOutputChannels = &pConfig->channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pReverbNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_reverb_node_uninit(ma_reverb_node* pReverbNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pReverbNode, pAllocationCallbacks);
}


/* Include ma_vocoder_node.h after miniaudio.h */
#define VOCLIB_IMPLEMENTATION
#include "fx/voclib.h"

#ifdef __cplusplus
extern "C" {
#endif


    /*
    The vocoder node has two inputs and one output. Inputs:

        Input Bus 0: The source/carrier stream.
        Input Bus 1: The excite/modulator stream.

    The source (input bus 0) and output must have the same channel count, and is restricted to 1 or 2.
    The excite (input bus 1) is restricted to 1 channel.
    */
    typedef struct
    {
        ma_node_config nodeConfig;
        ma_uint32 channels;         /* The number of channels of the source, which will be the same as the output. Must be 1 or 2. The excite bus must always have one channel. */
        ma_uint32 sampleRate;
        ma_uint32 bands;            /* Defaults to 16. */
        ma_uint32 filtersPerBand;   /* Defaults to 6. */
    } ma_vocoder_node_config;

    MA_API ma_vocoder_node_config ma_vocoder_node_config_init(ma_uint32 channels, ma_uint32 sampleRate);


    typedef struct
    {
        ma_node_base baseNode;
        voclib_instance voclib;
    } ma_vocoder_node;

    MA_API ma_result ma_vocoder_node_init(ma_node_graph* pNodeGraph, const ma_vocoder_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_vocoder_node* pVocoderNode);
    MA_API void ma_vocoder_node_uninit(ma_vocoder_node* pVocoderNode, const ma_allocation_callbacks* pAllocationCallbacks);

#ifdef __cplusplus
}
#endif


MA_API ma_vocoder_node_config ma_vocoder_node_config_init(ma_uint32 channels, ma_uint32 sampleRate)
{
    ma_vocoder_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();   /* Input and output channels will be set in ma_vocoder_node_init(). */
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.bands = 16;
    config.filtersPerBand = 6;

    return config;
}


static void ma_vocoder_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_vocoder_node* pVocoderNode = (ma_vocoder_node*)pNode;

    (void)pFrameCountIn;

    voclib_process(&pVocoderNode->voclib, ppFramesIn[0], ppFramesIn[1], ppFramesOut[0], *pFrameCountOut);
}

static ma_node_vtable g_ma_vocoder_node_vtable =
{
    ma_vocoder_node_process_pcm_frames,
    NULL,
    2,  /* 2 input channels. */
    1,  /* 1 output channel. */
    0
};

MA_API ma_result ma_vocoder_node_init(ma_node_graph* pNodeGraph, const ma_vocoder_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_vocoder_node* pVocoderNode)
{
    ma_result result;
    ma_node_config baseConfig;
    ma_uint32 inputChannels[2];
    ma_uint32 outputChannels[1];

    if (pVocoderNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pVocoderNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    if (voclib_initialize(&pVocoderNode->voclib, (unsigned char)pConfig->bands, (unsigned char)pConfig->filtersPerBand, (unsigned int)pConfig->sampleRate, (unsigned char)pConfig->channels) == 0) {
        return MA_INVALID_ARGS;
    }

    inputChannels[0] = pConfig->channels;   /* Source/carrier. */
    inputChannels[1] = 1;                   /* Excite/modulator. Must always be single channel. */
    outputChannels[0] = pConfig->channels;   /* Output channels is always the same as the source/carrier. */

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_vocoder_node_vtable;
    baseConfig.pInputChannels = inputChannels;
    baseConfig.pOutputChannels = outputChannels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pVocoderNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_vocoder_node_uninit(ma_vocoder_node* pVocoderNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node must always be initialized first. */
    ma_node_uninit(pVocoderNode, pAllocationCallbacks);
}
#ifdef __cplusplus
extern "C" {
#endif

    /*
    The trim node has one input and one output.
    */
    typedef struct
    {
        ma_node_config nodeConfig;
        ma_uint32 channels;
        float threshold;
    } ma_ltrim_node_config;

    MA_API ma_ltrim_node_config ma_ltrim_node_config_init(ma_uint32 channels, float threshold);


    typedef struct
    {
        ma_node_base baseNode;
        float threshold;
        ma_bool32 foundStart;
    } ma_ltrim_node;

    MA_API ma_result ma_ltrim_node_init(ma_node_graph* pNodeGraph, const ma_ltrim_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_ltrim_node* pTrimNode);
    MA_API void ma_ltrim_node_uninit(ma_ltrim_node* pTrimNode, const ma_allocation_callbacks* pAllocationCallbacks);

#ifdef __cplusplus
}
#endif
MA_API ma_ltrim_node_config ma_ltrim_node_config_init(ma_uint32 channels, float threshold)
{
    ma_ltrim_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_ltrim_node_init(). */
    config.channels = channels;
    config.threshold = threshold;

    return config;
}


static void ma_ltrim_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_ltrim_node* pTrimNode = (ma_ltrim_node*)pNode;
    ma_uint32 framesProcessedIn = 0;
    ma_uint32 framesProcessedOut = 0;
    ma_uint32 channelCount = ma_node_get_input_channels(pNode, 0);

    /*
    If we haven't yet found the start, skip over every input sample until we find a frame outside
    of the threshold.
    */
    if (pTrimNode->foundStart == MA_FALSE) {
        while (framesProcessedIn < *pFrameCountIn) {
            ma_uint32 iChannel = 0;
            for (iChannel = 0; iChannel < channelCount; iChannel += 1) {
                float sample = ppFramesIn[0][framesProcessedIn * channelCount + iChannel];
                if (sample < -pTrimNode->threshold || sample > pTrimNode->threshold) {
                    pTrimNode->foundStart = MA_TRUE;
                    break;
                }
            }

            if (pTrimNode->foundStart) {
                break;  /* The start has been found. Get out of this loop and finish off processing. */
            }
            else {
                framesProcessedIn += 1;
            }
        }
    }

    /* If there's anything left, just copy it over. */
    framesProcessedOut = ma_min(*pFrameCountOut, *pFrameCountIn - framesProcessedIn);
    ma_copy_pcm_frames(ppFramesOut[0], &ppFramesIn[0][framesProcessedIn], framesProcessedOut, ma_format_f32, channelCount);

    framesProcessedIn += framesProcessedOut;

    /* We always "process" every input frame, but we may only done a partial output. */
    *pFrameCountIn = framesProcessedIn;
    *pFrameCountOut = framesProcessedOut;
}

static ma_node_vtable g_ma_ltrim_node_vtable =
{
    ma_ltrim_node_process_pcm_frames,
    NULL,
    1,  /* 1 input channel. */
    1,  /* 1 output channel. */
    MA_NODE_FLAG_DIFFERENT_PROCESSING_RATES
};

MA_API ma_result ma_ltrim_node_init(ma_node_graph* pNodeGraph, const ma_ltrim_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_ltrim_node* pTrimNode)
{
    ma_result result;
    ma_node_config baseConfig;

    if (pTrimNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pTrimNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    pTrimNode->threshold = pConfig->threshold;
    pTrimNode->foundStart = MA_FALSE;

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_ltrim_node_vtable;
    baseConfig.pInputChannels = &pConfig->channels;
    baseConfig.pOutputChannels = &pConfig->channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pTrimNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_ltrim_node_uninit(ma_ltrim_node* pTrimNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pTrimNode, pAllocationCallbacks);
}
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {
        ma_node_config nodeConfig;
        ma_uint32 channels;
    } ma_channel_combiner_node_config;

    MA_API ma_channel_combiner_node_config ma_channel_combiner_node_config_init(ma_uint32 channels);


    typedef struct
    {
        ma_node_base baseNode;
    } ma_channel_combiner_node;

    MA_API ma_result ma_channel_combiner_node_init(ma_node_graph* pNodeGraph, const ma_channel_combiner_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_combiner_node* pSeparatorNode);
    MA_API void ma_channel_combiner_node_uninit(ma_channel_combiner_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks);


#ifdef __cplusplus
}
#endif
MA_API ma_channel_combiner_node_config ma_channel_combiner_node_config_init(ma_uint32 channels)
{
    ma_channel_combiner_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_channel_combiner_node_init(). */
    config.channels = channels;

    return config;
}


static void ma_channel_combiner_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_channel_combiner_node* pCombinerNode = (ma_channel_combiner_node*)pNode;

    (void)pFrameCountIn;

    ma_interleave_pcm_frames(ma_format_f32, ma_node_get_output_channels(pCombinerNode, 0), *pFrameCountOut, (const void**)ppFramesIn, (void*)ppFramesOut[0]);
}

static ma_node_vtable g_ma_channel_combiner_node_vtable =
{
    ma_channel_combiner_node_process_pcm_frames,
    NULL,
    MA_NODE_BUS_COUNT_UNKNOWN,  /* Input bus count is determined by the channel count and is unknown until the node instance is initialized. */
    1,  /* 1 output bus. */
    0   /* Default flags. */
};

MA_API ma_result ma_channel_combiner_node_init(ma_node_graph* pNodeGraph, const ma_channel_combiner_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_combiner_node* pCombinerNode)
{
    ma_result result;
    ma_node_config baseConfig;
    ma_uint32 inputChannels[MA_MAX_NODE_BUS_COUNT];
    ma_uint32 outputChannels[1];
    ma_uint32 iChannel;

    if (pCombinerNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pCombinerNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    /* All input channels are mono. */
    for (iChannel = 0; iChannel < pConfig->channels; iChannel += 1) {
        inputChannels[iChannel] = 1;
    }

    outputChannels[0] = pConfig->channels;

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_channel_combiner_node_vtable;
    baseConfig.inputBusCount = pConfig->channels; /* The vtable has an unknown channel count, so must specify it here. */
    baseConfig.pInputChannels = inputChannels;
    baseConfig.pOutputChannels = outputChannels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pCombinerNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_channel_combiner_node_uninit(ma_channel_combiner_node* pCombinerNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pCombinerNode, pAllocationCallbacks);
}
#ifdef __cplusplus
extern "C" {
#endif

    typedef struct
    {
        ma_node_config nodeConfig;
        ma_uint32 channels;
    } ma_channel_separator_node_config;

    MA_API ma_channel_separator_node_config ma_channel_separator_node_config_init(ma_uint32 channels);


    typedef struct
    {
        ma_node_base baseNode;
    } ma_channel_separator_node;

    MA_API ma_result ma_channel_separator_node_init(ma_node_graph* pNodeGraph, const ma_channel_separator_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_separator_node* pSeparatorNode);
    MA_API void ma_channel_separator_node_uninit(ma_channel_separator_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks);

#ifdef __cplusplus
}
#endif
MA_API ma_channel_separator_node_config ma_channel_separator_node_config_init(ma_uint32 channels)
{
    ma_channel_separator_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_channel_separator_node_init(). */
    config.channels = channels;

    return config;
}


static void ma_channel_separator_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_channel_separator_node* pSplitterNode = (ma_channel_separator_node*)pNode;

    (void)pFrameCountIn;

    ma_deinterleave_pcm_frames(ma_format_f32, ma_node_get_input_channels(pSplitterNode, 0), *pFrameCountOut, (const void*)ppFramesIn[0], (void**)ppFramesOut);
}

static ma_node_vtable g_ma_channel_separator_node_vtable =
{
    ma_channel_separator_node_process_pcm_frames,
    NULL,
    1,  /* 1 input bus. */
    MA_NODE_BUS_COUNT_UNKNOWN,  /* Output bus count is determined by the channel count and is unknown until the node instance is initialized. */
    0   /* Default flags. */
};

MA_API ma_result ma_channel_separator_node_init(ma_node_graph* pNodeGraph, const ma_channel_separator_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_separator_node* pSeparatorNode)
{
    ma_result result;
    ma_node_config baseConfig;
    ma_uint32 inputChannels[1];
    ma_uint32 outputChannels[MA_MAX_NODE_BUS_COUNT];
    ma_uint32 iChannel;

    if (pSeparatorNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pSeparatorNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    if (pConfig->channels > MA_MAX_NODE_BUS_COUNT) {
        return MA_INVALID_ARGS; /* Channel count cannot exceed the maximum number of buses. */
    }

    inputChannels[0] = pConfig->channels;

    /* All output channels are mono. */
    for (iChannel = 0; iChannel < pConfig->channels; iChannel += 1) {
        outputChannels[iChannel] = 1;
    }

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_channel_separator_node_vtable;
    baseConfig.outputBusCount = pConfig->channels; /* The vtable has an unknown channel count, so must specify it here. */
    baseConfig.pInputChannels = inputChannels;
    baseConfig.pOutputChannels = outputChannels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pSeparatorNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_channel_separator_node_uninit(ma_channel_separator_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pSeparatorNode, pAllocationCallbacks);
}

IPLAudioSettings iplAudioSettings;
IPLContextSettings iplContextSettings;
IPLContext iplContext;
IPLHRTFSettings iplHRTFSettings;
IPLHRTF iplHRTF;


bool inited = false;
ma_engine sound_default_mixer;

ma_engine_config engineConfig;

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
    ma_sound handle_;
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
    binauralParams.direction.y = pBinauralNode->direction.z;
    binauralParams.direction.z = pBinauralNode->direction.y;
    binauralParams.interpolation = IPL_HRTFINTERPOLATION_NEAREST;
    ma_vec3f listener = ma_engine_listener_get_position(&sound_default_mixer, ma_sound_get_listener_index(&pBinauralNode->handle_));

    float distance = sqrt((listener.x + binauralParams.direction.x) * (listener.x + binauralParams.direction.x) +
        (listener.y + binauralParams.direction.y) * (listener.y + binauralParams.direction.y) +
        (listener.z - binauralParams.direction.z) * (listener.z - binauralParams.direction.z));
    float maxDistance = 2.0f;

    float normalizedDistance = distance / maxDistance; 
    binauralParams.spatialBlend = std::min(0.0f + normalizedDistance, 1.0f);
    if (binauralParams.spatialBlend > 1.0f)
    binauralParams.spatialBlend = 1.0f;
    binauralParams.hrtf = pBinauralNode->iplHRTF;
    binauralParams.peakDelays=NULL;
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
    pBinauralNode->direction.y = -y;
    pBinauralNode->direction.z = z;

    return MA_SUCCESS;
}
void soundsystem_init() {
    engineConfig = ma_engine_config_init();
    engineConfig.channels = CHANNELS;
    engineConfig.sampleRate = SAMPLE_RATE;
    engineConfig.periodSizeInFrames = 256;

    ma_engine_init(&engineConfig, &sound_default_mixer);
    MA_ZERO_OBJECT(&iplAudioSettings);
    iplAudioSettings.samplingRate = ma_engine_get_sample_rate(&sound_default_mixer);

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
    ma_engine_uninit(&sound_default_mixer);
    iplContextRelease(&iplContext);

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
    ma_engine_set_gain_db(&sound_default_mixer, volume);
}
float get_master_volume() {
    return ma_engine_get_gain_db(&sound_default_mixer);

}
void mixer_start() {
    ma_engine_start(&sound_default_mixer);
}
void mixer_stop() {
    ma_engine_stop(&sound_default_mixer);
}
bool mixer_play_sound(const std::string& filename) {
    std::string result;
    if (sound_path != "") {
        result = sound_path + "/" + filename.c_str();
    }
    else {
        result = filename;
    }

    ma_result r;
    r=ma_engine_play_sound(&sound_default_mixer, result.c_str(), nullptr);
    if (r != MA_SUCCESS)
        return  false;
    return true;
}
void mixer_reinit() {
    soundsystem_free();
    soundsystem_init();
}
bool sound_global_hrtf = false;
struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t write_memory_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    char* ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL) {
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

std::vector<float> load_audio_from_url(const char* url) {
    CURL* curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        free(chunk.memory);
        return std::vector<float>();
    }

    std::vector<float> audioData(chunk.size / sizeof(float));
    memcpy(audioData.data(), chunk.memory, chunk.size);

    free(chunk.memory);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return audioData;
}
typedef struct
{
    ma_node_config nodeConfig;
    ma_uint32 channels;         /* The number of channels of the source, which will be the same as the output. Must be 1 or 2. */
    ma_uint32 sampleRate;
    float playbackSpeed;       /* Playback speed multiplier. */
} ma_playback_speed_node_config;
typedef struct
{
    ma_node_base baseNode;
    ma_playback_speed_node_config conf;
} ma_playback_speed_node;


MA_API ma_playback_speed_node_config ma_playback_speed_node_config_init(ma_uint32 channels, ma_uint32 sampleRate)
{
    ma_playback_speed_node_config config;

    MA_ZERO_OBJECT(&config);
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_reverb_node_init(). */
    config.channels = channels;
    config.sampleRate = sampleRate;

    return config;
}

static void ma_playback_speed_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_playback_speed_node* pPlaybackSpeedNode = (ma_playback_speed_node*)pNode;

    float playbackSpeed = pPlaybackSpeedNode->conf.playbackSpeed;

    for (ma_uint32 i = 0; i < *pFrameCountOut; i++) {
        float t = (float)i / playbackSpeed;
        int index = (int)t;
        float fraction = t - index;

        for (ma_uint32 j = 0; j < pPlaybackSpeedNode->conf.channels; j++) {
            int inputIndex1 = index * pPlaybackSpeedNode->conf.channels + j;
            int inputIndex2 = (index + 1) * pPlaybackSpeedNode->conf.channels + j;
            int outputIndex = i * pPlaybackSpeedNode->conf.channels + j;

            *ppFramesOut[outputIndex] = *ppFramesIn[inputIndex1] + fraction * (ppFramesIn[inputIndex2] - ppFramesIn[inputIndex1]);
        }
    }

}

static ma_node_vtable g_ma_playback_speed_node_vtable =
{
    ma_playback_speed_node_process_pcm_frames,
    NULL,
    1,  /* 1 input channel. */
    1,  /* 1 output channel. */
    MA_NODE_FLAG_CONTINUOUS_PROCESSING};
MA_API ma_result ma_playback_speed_node_init(ma_node_graph* pNodeGraph, const ma_playback_speed_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_playback_speed_node* pPlaybackSpeedNode)
{
    ma_result result;
    ma_node_config baseConfig;

    if (pPlaybackSpeedNode == NULL) {
        return MA_INVALID_ARGS;
    }

    MA_ZERO_OBJECT(pPlaybackSpeedNode);

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable = &g_ma_playback_speed_node_vtable;
    baseConfig.pInputChannels = &pConfig->channels;
    baseConfig.pOutputChannels = &pConfig->channels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pPlaybackSpeedNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_playback_speed_node_uninit(ma_playback_speed_node* pPlaybackSpeedNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pPlaybackSpeedNode, pAllocationCallbacks);
}





class  mixer {
public:
    ma_engine engine;
};
class sound {
public:
    bool is_3d_;
    bool playing = false, paused = false, active = false;
    ma_sound handle_;
    ma_steamaudio_binaural_node g_binauralNode;   /* The echo effect is achieved using a delay node. */
    ma_reverb_node      g_reverbNode;        /* The reverb node. */
    ma_reverb_node_config reverbNodeConfig;
    bool reverb = false;
    ma_vocoder_node     g_vocoderNode;   /* The vocoder node. */
    ma_vocoder_node_config vocoderNodeConfig;
    bool vocoder = false;
    ma_delay_node       g_delayNode;         /* The delay node. */
    ma_delay_node_config delayNodeConfig;
    bool delayf = false;
    ma_ltrim_node       g_trimNode;        /* The trim node. */
    ma_ltrim_node_config trimNodeConfig;
    bool ltrim = false;
    ma_channel_separator_node g_separatorNode;   /* The separator node. */
    bool separator = false;
    ma_channel_combiner_node  g_combinerNode;    /* The combiner node. */
    ma_channel_separator_node_config separatorNodeConfig;
    ma_channel_combiner_node_config combinerNodeConfig;
    bool combiner = false;
    ma_hpf_node highpass;
    ma_hpf_node_config highpassConfig;
    bool hp = false;
    ma_lpf_node lowpass;
    ma_lpf_node_config lowpassConfig;
    bool lp = false;
    ma_notch_node notch;
    ma_notch_node_config notchConfig;
    bool notchf = false;
    ma_playback_speed_node g_playback_speed_node;
    ma_playback_speed_node_config g_speed_config;
    bool speeding = false;
    ma_steamaudio_binaural_node_config binauralNodeConfig;
    bool sound_hrtf = false;
    ma_node* current_fx = nullptr;
    sound(const std::string  &filename="", bool set3d=false) {
        if (!inited) {
            soundsystem_init();
            inited = true;
        }
        if (filename != "")
            this->load(filename, set3d);
        current_fx = ma_engine_get_endpoint(&sound_default_mixer);
    }
    ~sound() {
        if (this->active)this->close();
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
            ma_sound_init_from_file(&sound_default_mixer, result.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
        else
            ma_sound_init_from_file(&sound_default_mixer, result.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_DECODE, NULL, NULL, &handle_);
        active = true;
        if (sound_global_hrtf)
            this->set_hrtf(true);
    return true;
        }
    bool load_from_memory(const std::string& data, bool set3d) {
        if (active) {
            this->close();
            active = false;
        }
        ma_sound_config c;

        ma_decoder decoder;
        ma_decoder_init_memory(data.c_str(), data.size(), NULL, &decoder);
        c=ma_sound_config_init();
        c.pDataSource=& decoder;

        if(!set3d)
c.flags|=MA_SOUND_FLAG_NO_SPATIALIZATION;
        ma_sound_init_ex(&sound_default_mixer, &c, &handle_);
        active = true;
        if (sound_global_hrtf)
            this->set_hrtf(true);

        return active;
    }
    bool stream(const std::string& filename, bool set3d) {
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
            ma_sound_init_from_file(&sound_default_mixer, result.c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &handle_);
        else
            ma_sound_init_from_file(&sound_default_mixer, result.c_str(), MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM, NULL, NULL, &handle_);
        active = true;
        if (sound_global_hrtf)
            this->set_hrtf(true);

        return true;
    }
    bool load_url(const std::string& url, bool set3d) {

        if (active) {
            this->close();
            active = false;
        }
        ma_sound_config c;

        ma_decoder decoder;
        std::vector<float> audio = load_audio_from_url(url.c_str());
        ma_decoder_init_memory(audio.data(), audio.size(), NULL, &decoder);
        c = ma_sound_config_init();
        c.pDataSource = &decoder;

        if (!set3d)
            c.flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;
        ma_sound_init_ex(&sound_default_mixer, &c, &handle_);
        active = true;
        if (sound_global_hrtf)
            this->set_hrtf(true);

        return active;
    }
    std::string push_memory() {
        return NULL;
    }
    void set_faid_time(float volume_beg, float volume_end, float time) {
        ma_sound_set_fade_in_milliseconds(&handle_, volume_beg/100, volume_end/100, static_cast<ma_uint64>(time));
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
    void set_fx(const std::string& fx) {
        if (!active)return;
        if (fx == "reverb") {
            reverbNodeConfig = ma_reverb_node_config_init(engineConfig.channels, engineConfig.sampleRate, 100, 100, 100);
            if (reverb) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_reverb_node_uninit(&g_reverbNode, NULL);
                reverb = false;
            }
            ma_reverb_node_init(ma_engine_get_node_graph(&sound_default_mixer), &reverbNodeConfig, NULL, &g_reverbNode);
            ma_node_attach_output_bus(&g_reverbNode, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_reverbNode, 0);
            current_fx = &g_reverbNode;
            reverb = true;
        }
        if (fx == "vocoder") {
            vocoderNodeConfig = ma_vocoder_node_config_init(engineConfig.channels, engineConfig.sampleRate);
            if (vocoder) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_vocoder_node_uninit(&g_vocoderNode, NULL);
                vocoder = false;
            }
            ma_vocoder_node_init(ma_engine_get_node_graph(&sound_default_mixer), &vocoderNodeConfig, NULL, &g_vocoderNode);
            ma_node_attach_output_bus(&g_vocoderNode, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_vocoderNode, 0);
            current_fx = &g_vocoderNode;
            vocoder = true;
        }
        if (fx == "delay") {
            delayNodeConfig = ma_delay_node_config_init(engineConfig.channels, engineConfig.sampleRate, (100 * engineConfig.sampleRate) / 1000, 0.5f);
            if (delayf) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_delay_node_uninit(&g_delayNode, NULL);
                delayf = false;
            }
            ma_delay_node_init(ma_engine_get_node_graph(&sound_default_mixer), &delayNodeConfig, NULL, &g_delayNode);
            ma_node_attach_output_bus(&g_delayNode, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_delayNode, 0);
            current_fx = &g_delayNode;
            delayf = true;
        }
        if (fx == "ltrim") {
            trimNodeConfig = ma_ltrim_node_config_init(engineConfig.channels, 0);
            trimNodeConfig.threshold = 0.3;
            if (ltrim) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_ltrim_node_uninit(&g_trimNode, NULL);
                ltrim = false;
            }
            ma_ltrim_node_init(ma_engine_get_node_graph(&sound_default_mixer), &trimNodeConfig, NULL, &g_trimNode);
            ma_node_attach_output_bus(&g_trimNode, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_trimNode, 0);
            current_fx = &g_trimNode;

            ltrim = true;
        }
        if (fx == "channelsplit") {
            combinerNodeConfig = ma_channel_combiner_node_config_init(engineConfig.channels);
            if (combiner) {
                ma_channel_combiner_node_uninit(&g_combinerNode, NULL);
                combiner = false;
            }
            ma_channel_combiner_node_init(ma_engine_get_node_graph(&sound_default_mixer), &combinerNodeConfig, NULL, &g_combinerNode);
            ma_node_attach_output_bus(&g_combinerNode, 0, ma_engine_get_endpoint(&sound_default_mixer), 0);
            combiner = true;
            separatorNodeConfig = ma_channel_separator_node_config_init(engineConfig.channels);
            if (separator) {
                ma_channel_separator_node_uninit(&g_separatorNode, NULL);
                separator = false;
            }
            ma_channel_separator_node_init(ma_engine_get_node_graph(&sound_default_mixer), &separatorNodeConfig, NULL, &g_separatorNode);
            MA_ASSERT(ma_node_get_output_bus_count(&g_separatorNode) == ma_node_get_input_bus_count(&g_combinerNode));
            for (ma_uint32 iChannel = 0; iChannel < ma_node_get_output_bus_count(&g_separatorNode); iChannel += 1) {
                ma_node_attach_output_bus(&g_separatorNode, iChannel, &g_combinerNode, iChannel);
            }

            ma_node_attach_output_bus(&handle_, 0, &g_separatorNode, 0);
            separator = true;
        }
        if (fx == "highpass") {
            highpassConfig = ma_hpf_node_config_init(engineConfig.channels, engineConfig.sampleRate, 600, -10);
            if (hp) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_hpf_node_uninit(&highpass, NULL);
                hp = false;
            }
            ma_hpf_node_init(ma_engine_get_node_graph(&sound_default_mixer), &highpassConfig, NULL, &highpass);
            ma_node_attach_output_bus(&highpass, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &highpass, 0);
            current_fx = &highpass;

            hp = true;
        }
        if (fx == "lowpass") {
            lowpassConfig = ma_lpf_node_config_init(engineConfig.channels, engineConfig.sampleRate, 600, -10);
            if (lp) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_lpf_node_uninit(&lowpass, NULL);
                lp = false;
            }
            ma_lpf_node_init(ma_engine_get_node_graph(&sound_default_mixer), &lowpassConfig, NULL, &lowpass);
            ma_node_attach_output_bus(&lowpass, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &lowpass, 0);
            current_fx = &lowpass;

            lp = true;
        }
        if (fx == "notch") {
            notchConfig=ma_notch_node_config_init(engineConfig.channels, engineConfig.sampleRate, 0, 300);
            if (notchf) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_notch_node_uninit(&notch, NULL);
                notchf = false;
            }
            ma_notch_node_init(ma_engine_get_node_graph(&sound_default_mixer), &notchConfig, NULL, &notch);
            ma_node_attach_output_bus(&notch, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &notch, 0);
            current_fx = &notch;

            notchf = true;
        }
    }
    void delete_fx(const std::string& fx) {
        if (!active)return;
        if (fx == "reverb") {
            if (reverb) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_reverb_node_uninit(&g_reverbNode, NULL);
                reverb = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);
        }
        if (fx == "vocoder") {
            if (vocoder) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_vocoder_node_uninit(&g_vocoderNode, NULL);
                vocoder = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);
        }
        if (fx == "delay") {
            if (delayf) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_delay_node_uninit(&g_delayNode, NULL);
                delayf = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);
        }
        if (fx == "ltrim") {
            if (ltrim) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_ltrim_node_uninit(&g_trimNode, NULL);
                ltrim = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);
        }
        if (fx == "channelsplit") {
            combinerNodeConfig = ma_channel_combiner_node_config_init(engineConfig.channels);
            if (combiner) {
                ma_channel_combiner_node_uninit(&g_combinerNode, NULL);
                combiner = false;
            }
            if (separator) {
                ma_channel_separator_node_uninit(&g_separatorNode, NULL);
                separator = false;
            }
        }
        if (fx == "highpass") {
            if (hp) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_hpf_node_uninit(&highpass, NULL);
                hp = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);

        }
        if (fx == "lowpass") {
            if (lp) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_lpf_node_uninit(&lowpass, NULL);
                lp = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);

        }
        if (fx == "notch") {
            if (notchf) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_notch_node_uninit(&notch, NULL);
                notchf = false;
            }
            ma_node_attach_output_bus(&handle_, 0, current_fx, 0);

        }
    }

    void set_reverb_parameters(float dry, float wet, float room_size, float damping, float mode) {
        if (!active)return;
        verblib_set_dry(&g_reverbNode.reverb, dry);
        verblib_set_wet(&g_reverbNode.reverb, wet);
        verblib_set_room_size(&g_reverbNode.reverb, room_size);
        verblib_set_damping(&g_reverbNode.reverb, damping);
        verblib_set_mode(&g_reverbNode.reverb, mode);
    }
    void set_delay_parameters(float dry, float wet, float dcay) {
        if (!active)return;
        ma_delay_node_set_dry(&g_delayNode, dry);
        ma_delay_node_set_wet(&g_delayNode, wet);
        ma_delay_node_set_decay(&g_delayNode, dcay);
    }
    void set_position(float l_x, float l_y, float l_z, float s_x, float s_y, float s_z) {
        if (!active)return;

        ma_vec3f direction;
        ma_engine_listener_set_position(&sound_default_mixer, ma_sound_get_listener_index(&handle_), l_x, l_y, l_z);
        ma_sound_set_position(&handle_, s_x, s_y, s_z);
        ma_vec3f relativePos;
            ma_spatializer_get_relative_position_and_direction(&handle_.engineNode.spatializer, &sound_default_mixer.listeners[ma_sound_get_listener_index(&handle_)], &relativePos, NULL);

        direction=ma_vec3f_normalize(relativePos);

        ma_steamaudio_binaural_node_set_direction(&g_binauralNode, direction.x, direction.y, direction.z);

    }
    void set_position(const ngtvector* listener=nullptr, const ngtvector* source=nullptr) {
        if (!active)return;
        ma_vec3f direction;

        ma_engine_listener_set_position(&sound_default_mixer, ma_sound_get_listener_index(&handle_), listener->x, listener->y, listener->z);
        ma_sound_set_position(&handle_, source->x, source->y, source->z);
        ma_vec3f relativePos;
        ma_spatializer_get_relative_position_and_direction(&handle_.engineNode.spatializer, &sound_default_mixer.listeners[ma_sound_get_listener_index(&handle_)], &relativePos, NULL);

        direction = ma_vec3f_normalize(relativePos);

        ma_steamaudio_binaural_node_set_direction(&g_binauralNode, direction.x, direction.y, direction.z);

    }
    void set_hrtf(bool hrtf) {
        if (!active)return;
        if (hrtf) {

            binauralNodeConfig = ma_steamaudio_binaural_node_config_init(CHANNELS, iplAudioSettings, iplContext, iplHRTF);
            if (sound_hrtf == true) {
                current_fx = ma_engine_get_endpoint(&sound_default_mixer);

                ma_steamaudio_binaural_node_uninit(&g_binauralNode, NULL);
                sound_hrtf = false;
            }
            g_binauralNode.handle_ = this->handle_;
            ma_steamaudio_binaural_node_init(ma_engine_get_node_graph(&sound_default_mixer), &binauralNodeConfig, NULL, &g_binauralNode);
            /* Connect the output of the delay node to the input of the endpoint. */
            ma_node_attach_output_bus(&g_binauralNode, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_binauralNode, 0);
            current_fx = &g_binauralNode;
            ma_sound_set_directional_attenuation_factor(&handle_, 0);
            sound_hrtf = true;
        }
        else {
            if (sound_hrtf == true) {

                ma_steamaudio_binaural_node_uninit(&g_binauralNode, NULL);
                sound_hrtf = false;
                ma_sound_set_directional_attenuation_factor(&handle_, 1);

                current_fx = ma_engine_get_endpoint(&sound_default_mixer);
                ma_node_attach_output_bus(&handle_, 0, current_fx, 0);

            }

        }
    }
    void set_volume_step(float volume_step) {
        if (!active)return;
        ma_sound_set_rolloff(&handle_, volume_step);
    }
    void set_pan_step(float pan_step) {
        if (!active)return;
        ma_sound_set_directional_attenuation_factor(&handle_, pan_step);
}
    void set_pitch_step(float pitch_step) {
        if (!active)return;
        ma_sound_set_doppler_factor(&handle_, pitch_step);
}
    bool seek(float new_position) {
        if (!active)return false;
        if (new_position > this->get_length())
            return false;
ma_sound_seek_to_pcm_frame(&handle_, static_cast<ma_uint64>(new_position * 100));
        return true;
    }
    void set_looping(bool looping) {
        if (!active)return;
        ma_sound_set_looping(&handle_, looping);
    }
    bool get_looping()const {
        if (!active)return false;
        return ma_sound_is_looping(&handle_);
    }
    float get_pan() const {
        if (!active)return -17435;

        float pan=0;
        pan = ma_sound_get_pan(&handle_);
        return pan * 100;
    }

    void set_pan(float pan) {
        if (!active)return;
        ma_sound_set_pan(&handle_, pan / 100);
    }

    float get_volume() const {
        if (!active)return -17435;

        float volume = 0;

        volume = ma_sound_get_volume(&handle_);
        return ma_volume_linear_to_db(volume);
    }
            void set_volume(float volume) {
        if (!active)return;
        if (volume > 0 or volume < -100)return;
            ma_sound_set_volume(&handle_, ma_volume_db_to_linear(volume));
        }
        float get_pitch() const {
        if (!active)return -17435;
        float pitch = 0;
            pitch = ma_sound_get_pitch(&handle_);
        return pitch * 100;
    }

        void set_pitch(float pitch) {
            if (!active)return;
            ma_sound_set_pitch(&handle_, pitch / 100);
        }
        void set_speed(float speed) {
            if (!speeding) {
                g_speed_config= ma_playback_speed_node_config_init(engineConfig.channels, engineConfig.sampleRate);
                ma_playback_speed_node_init(ma_engine_get_node_graph(&sound_default_mixer), &g_speed_config, NULL, &g_playback_speed_node);
            ma_node_attach_output_bus(&g_playback_speed_node, 0, current_fx, 0);
            ma_node_attach_output_bus(&handle_, 0, &g_playback_speed_node, 0);
            current_fx = &g_playback_speed_node;
            speeding = true;
        }
            g_speed_config.playbackSpeed = speed / 100;
}
        float get_speed()const {
            return g_speed_config.playbackSpeed * 100;
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
    }

    float get_position()  {
        if (!active)return -17435;

        float position=0;
        position=ma_sound_get_time_in_milliseconds(&handle_);
        return position;
    }

    float get_length()  {
        if (!active)return-17435;
        ma_uint64 length = 0;
        ma_sound_get_length_in_pcm_frames(&handle_, &length);
        return static_cast<float>(length/100);
    }
    void set_length(float length = 0.0) {
        if (!active)return;
        if (length > this->get_length())            return;
ma_sound_set_stop_time_in_pcm_frames(&handle_, static_cast<ma_uint64>(length * 100));
    }
    float get_sample_rate() const {
        float rate = 0;
        return rate;
    }
};
void set_sound_global_hrtf(bool hrtf) {
    sound_global_hrtf = hrtf;
}
bool get_sound_global_hrtf() {
    return sound_global_hrtf;
}
sound* fsound(const std::string& filename, bool set3d) { return new sound(filename, set3d); }
mixer* fmixer() { return new mixer; }
void register_sound(asIScriptEngine* engine) {
    engine->RegisterGlobalFunction("void set_sound_storage(const string &in)property", asFUNCTION(set_sound_storage), asCALL_CDECL);

    engine->RegisterGlobalFunction("string get_sound_storage()property", asFUNCTION(get_sound_storage), asCALL_CDECL);
    engine->RegisterGlobalFunction("void set_master_volume(float)property", asFUNCTION(set_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("float get_master_volume()property", asFUNCTION(get_master_volume), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_start()", asFUNCTION(mixer_start), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_stop()", asFUNCTION(mixer_stop), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool mixer_play_sound(const string &in)", asFUNCTION(mixer_play_sound), asCALL_CDECL);
    engine->RegisterGlobalFunction("void mixer_reinit()", asFUNCTION(mixer_reinit), asCALL_CDECL);
    engine->RegisterObjectType("mixer", sizeof(mixer), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("mixer", asBEHAVE_FACTORY, "mixer @m()", asFUNCTION(fmixer), asCALL_CDECL);
    engine->RegisterObjectType("sound", sizeof(sound), asOBJ_REF | asOBJ_NOCOUNT);
    engine->RegisterObjectBehaviour("sound", asBEHAVE_FACTORY, "sound@ s(const string &in=\"\", bool=false)", asFUNCTION(fsound), asCALL_CDECL);
    engine->RegisterObjectMethod("sound", "bool load(const string &in, bool=false)const", asMETHOD(sound, load), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "bool load_from_memory(const string &in, bool=false)const", asMETHOD(sound, load_from_memory), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "bool stream(const string &in, bool=false)const", asMETHOD(sound, stream), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "bool load_url(const string &in, bool=false)const", asMETHOD(sound, load_url), asCALL_THISCALL);

        engine->RegisterObjectMethod("sound", "string push_memory()const", asMETHOD(sound, push_memory), asCALL_THISCALL);

        engine->RegisterObjectMethod("sound", "void set_faid_time(float, float, float)const", asMETHOD(sound, set_faid_time), asCALL_THISCALL);

        engine->RegisterObjectMethod("sound", "bool play()const", asMETHOD(sound, play), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_looped()const", asMETHOD(sound, play_looped), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool pause()const", asMETHOD(sound, pause), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool play_wait()const", asMETHOD(sound, play_wait), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool stop()const", asMETHOD(sound, stop), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool close()const", asMETHOD(sound, close), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_fx(const string &in, int=0)const", asMETHOD(sound, set_fx), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void delete_fx(const string &in, int=0)const", asMETHOD(sound, delete_fx), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_reverb_parameters(float, float, float, float, float)const", asMETHOD(sound, set_reverb_parameters), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_delay_parameters(float, float, float)const", asMETHOD(sound, set_delay_parameters), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "void set_position(float, float, float, float, float, float)const", asMETHODPR(sound, set_position, (float, float, float, float, float, float), void), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_position(const vector@=null, const vector@=null)const", asMETHODPR(sound, set_position, (const ngtvector*, const ngtvector*), void), asCALL_THISCALL);

        engine->RegisterObjectMethod("sound", "void set_hrtf(bool=true)const property", asMETHOD(sound, set_hrtf), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "void set_volume_step(float)const property", asMETHOD(sound, set_volume_step), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "void set_pan_step(float)const property", asMETHOD(sound, set_pan_step), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "void set_pitch_step(float)const property", asMETHOD(sound, set_pitch_step), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "bool seek(float)const", asMETHOD(sound, seek), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "bool get_looping() const property", asMETHOD(sound, get_looping), asCALL_THISCALL);
        engine->RegisterObjectMethod("sound", "void set_looping(bool)const property", asMETHOD(sound, set_looping), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "float get_pan() const property", asMETHOD(sound, get_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pan(float)const property", asMETHOD(sound, set_pan), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_volume() const property", asMETHOD(sound, get_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_volume(float)const property", asMETHOD(sound, set_volume), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_pitch() const property", asMETHOD(sound, get_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_pitch(float)const property", asMETHOD(sound, set_pitch), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_speed() const property", asMETHOD(sound, get_speed), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_speed(float)const property", asMETHOD(sound, set_speed), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "bool get_active() const property", asMETHOD(sound, is_active), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_playing() const property", asMETHOD(sound, is_playing), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "bool get_paused() const property", asMETHOD(sound, is_paused), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_position() const property", asMETHOD(sound, get_position), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "float get_length() const property", asMETHOD(sound, get_length), asCALL_THISCALL);
    engine->RegisterObjectMethod("sound", "void set_length(float=0.0) const property", asMETHOD(sound, set_length), asCALL_THISCALL);

    engine->RegisterObjectMethod("sound", "float get_sample_rate() const property", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);
    engine->RegisterGlobalFunction("void set_sound_global_hrtf(bool)property", asFUNCTION(set_sound_global_hrtf), asCALL_CDECL);
    engine->RegisterGlobalFunction("bool get_sound_global_hrtf()property", asFUNCTION(get_sound_global_hrtf), asCALL_CDECL);
    engine->RegisterGlobalProperty("mixer@ sound_default_mixer", (void*)&sound_default_mixer);
}