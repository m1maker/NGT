// NGTAUDIO
#include "scriptmath/scriptmath3d.h"
#include <unordered_set>
#define NOMINMAX
#include "MemoryStream.h"
#include "ngt.h"
#include "obfuscate.h"
#include "Poco/Exception.h"
#include "scriptarray/scriptarray.h"
#include "sound.h"
#include <numeric>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <atomic>
#include <algorithm> // For std::find, std::min
#include <stdexcept>
#include <cstring>
#include <cmath>     // For sqrt, cos, sin, fabs (though fabs is in cstdlib/cstdlib.h)
#include <chrono>    // For std::this_thread::sleep_for

using namespace std;

#include "stb_vorbis.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdint.h> 
#include "phonon.h" /* Steam Audio */
#include "pack.h"   // For pack file integration

#define DEFAULT_FORMAT ma_format_f32 
static int DEFAULT_SAMPLE_RATE = 44100;
static int DEFAULT_CHANNELS = 2;

#include "fx/freeverb.h"
#define VERBLIB_IMPLEMENTATION
#include "fx/verblib.h"

// Forward declarations
class mixer;
class sound;
class AudioEngine;
class pcm_ring_buffer;

static AudioEngine* g_audio_engine = nullptr;
static bool g_SoundInitialized = false;

#ifdef __cplusplus
extern "C"
{
#endif

	typedef struct
	{
		ma_node_config nodeConfig;
		ma_uint32 channels;
		ma_uint32 sampleRate;
		float roomSize;
		float damping;
		float width;
		float wetVolume;
		float dryVolume;
		float mode;
	} ma_reverb_node_config;

	MA_API ma_reverb_node_config ma_reverb_node_config_init(ma_uint32 channels, ma_uint32 sampleRate, float dry = verblib_initialdry, float wet = verblib_initialwet, float room_size = verblib_initialroom);

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
	config.nodeConfig = ma_node_config_init();
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
	ma_reverb_node_process_pcm_frames, NULL, 1, 1, MA_NODE_FLAG_CONTINUOUS_PROCESSING
};

MA_API ma_result ma_reverb_node_init(ma_node_graph* pNodeGraph, const ma_reverb_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_reverb_node* pReverbNode)
{
	ma_result result;
	ma_node_config baseConfig;
	if (pReverbNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pReverbNode);
	if (pConfig == NULL) return MA_INVALID_ARGS;
	if (verblib_initialize(&pReverbNode->reverb, (unsigned long)pConfig->sampleRate, (unsigned int)pConfig->channels) == 0) {
		return MA_ERROR;
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
	if (pReverbNode == NULL) return;
	ma_node_uninit(&pReverbNode->baseNode, pAllocationCallbacks);
}

#define VOCLIB_IMPLEMENTATION
#include "fx/voclib.h" // Ensure path is correct

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		ma_node_config nodeConfig;
		ma_uint32 channels;
		ma_uint32 sampleRate;
		ma_uint32 bands;
		ma_uint32 filtersPerBand;
	} ma_vocoder_node_config;
	MA_API ma_vocoder_node_config ma_vocoder_node_config_init(ma_uint32 channels, ma_uint32 sampleRate);
	typedef struct {
		ma_node_base baseNode;
		voclib_instance voclib;
	} ma_vocoder_node;
	MA_API ma_result ma_vocoder_node_init(ma_node_graph* pNodeGraph, const ma_vocoder_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_vocoder_node* pVocoderNode);
	MA_API void ma_vocoder_node_uninit(ma_vocoder_node* pVocoderNode, const ma_allocation_callbacks* pAllocationCallbacks);
#ifdef __cplusplus
}
#endif

MA_API ma_vocoder_node_config ma_vocoder_node_config_init(ma_uint32 channels, ma_uint32 sampleRate) {
	ma_vocoder_node_config config;
	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channels = channels;
	config.sampleRate = sampleRate;
	config.bands = 16; // Default
	config.filtersPerBand = 6; // Default
	return config;
}
static void ma_vocoder_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
	ma_vocoder_node* pVocoderNode = (ma_vocoder_node*)pNode;
	(void)pFrameCountIn;
	voclib_process(&pVocoderNode->voclib, ppFramesIn[0], ppFramesIn[1], ppFramesOut[0], *pFrameCountOut);
}
static ma_node_vtable g_ma_vocoder_node_vtable = {
	ma_vocoder_node_process_pcm_frames, NULL, 2, 1, 0
};
MA_API ma_result ma_vocoder_node_init(ma_node_graph* pNodeGraph, const ma_vocoder_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_vocoder_node* pVocoderNode) {
	ma_result result;
	ma_node_config baseConfig;
	ma_uint32 inputChannels[2];
	ma_uint32 outputChannels[1];
	if (pVocoderNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pVocoderNode);
	if (pConfig == NULL) return MA_INVALID_ARGS;
	if (voclib_initialize(&pVocoderNode->voclib, (unsigned char)pConfig->bands, (unsigned char)pConfig->filtersPerBand, (unsigned int)pConfig->sampleRate, (unsigned char)pConfig->channels) == 0) {
		return MA_ERROR;
	}
	inputChannels[0] = pConfig->channels;
	inputChannels[1] = 1;
	outputChannels[0] = pConfig->channels;
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
MA_API void ma_vocoder_node_uninit(ma_vocoder_node* pVocoderNode, const ma_allocation_callbacks* pAllocationCallbacks) {
	if (pVocoderNode == NULL) return;
	ma_node_uninit(&pVocoderNode->baseNode, pAllocationCallbacks);
}
#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		ma_node_config nodeConfig;
		ma_uint32 channels;
		float threshold;
	} ma_ltrim_node_config;
	MA_API ma_ltrim_node_config ma_ltrim_node_config_init(ma_uint32 channels, float threshold);
	typedef struct {
		ma_node_base baseNode;
		float threshold;
		ma_bool32 foundStart;
	} ma_ltrim_node;
	MA_API ma_result ma_ltrim_node_init(ma_node_graph* pNodeGraph, const ma_ltrim_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_ltrim_node* pTrimNode);
	MA_API void ma_ltrim_node_uninit(ma_ltrim_node* pTrimNode, const ma_allocation_callbacks* pAllocationCallbacks);
#ifdef __cplusplus
}
#endif
MA_API ma_ltrim_node_config ma_ltrim_node_config_init(ma_uint32 channels, float threshold) {
	ma_ltrim_node_config config;
	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channels = channels;
	config.threshold = threshold;
	return config;
}
static void ma_ltrim_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
	ma_ltrim_node* pTrimNode = (ma_ltrim_node*)pNode;
	ma_uint32 framesProcessedIn = 0;
	ma_uint32 framesProcessedOut = 0;
	ma_uint32 channelCount = ma_node_get_input_channels(pNode, 0);
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
			if (pTrimNode->foundStart) break;
			else framesProcessedIn += 1;
		}
	}
	ma_uint32 framesToCopy = ma_min(*pFrameCountOut, *pFrameCountIn - framesProcessedIn);
	if (framesToCopy > 0) { // Check if there's anything to copy
		ma_copy_pcm_frames(ppFramesOut[0], &ppFramesIn[0][framesProcessedIn * channelCount], framesToCopy, DEFAULT_FORMAT, channelCount);
	}
	framesProcessedOut = framesToCopy;
	framesProcessedIn += framesProcessedOut; // Only advance input by what was outputted if different rates
	*pFrameCountIn = *pFrameCountIn; // Input frames are all "consumed" in terms of decision making
	*pFrameCountOut = framesProcessedOut;
}
static ma_node_vtable g_ma_ltrim_node_vtable = {
	ma_ltrim_node_process_pcm_frames, NULL, 1, 1, MA_NODE_FLAG_DIFFERENT_PROCESSING_RATES
};
MA_API ma_result ma_ltrim_node_init(ma_node_graph* pNodeGraph, const ma_ltrim_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_ltrim_node* pTrimNode) {
	ma_result result;
	ma_node_config baseConfig;
	if (pTrimNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pTrimNode);
	if (pConfig == NULL) return MA_INVALID_ARGS;
	pTrimNode->threshold = pConfig->threshold;
	pTrimNode->foundStart = MA_FALSE;
	baseConfig = pConfig->nodeConfig;
	baseConfig.vtable = &g_ma_ltrim_node_vtable;
	baseConfig.pInputChannels = &pConfig->channels;
	baseConfig.pOutputChannels = &pConfig->channels;
	result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pTrimNode->baseNode);
	return result;
}
MA_API void ma_ltrim_node_uninit(ma_ltrim_node* pTrimNode, const ma_allocation_callbacks* pAllocationCallbacks) {
	if (pTrimNode == NULL) return;
	ma_node_uninit(&pTrimNode->baseNode, pAllocationCallbacks);
}

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		ma_node_config nodeConfig;
		ma_uint32 channels;
	} ma_channel_combiner_node_config;
	MA_API ma_channel_combiner_node_config ma_channel_combiner_node_config_init(ma_uint32 channels);
	typedef struct {
		ma_node_base baseNode;
	} ma_channel_combiner_node;
	MA_API ma_result ma_channel_combiner_node_init(ma_node_graph* pNodeGraph, const ma_channel_combiner_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_combiner_node* pCombinerNode);
	MA_API void ma_channel_combiner_node_uninit(ma_channel_combiner_node* pCombinerNode, const ma_allocation_callbacks* pAllocationCallbacks);
#ifdef __cplusplus
}
#endif
MA_API ma_channel_combiner_node_config ma_channel_combiner_node_config_init(ma_uint32 channels) {
	ma_channel_combiner_node_config config;
	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channels = channels;
	return config;
}
static void ma_channel_combiner_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
	(void)pFrameCountIn; // All input buses should have same frame count
	ma_interleave_pcm_frames(DEFAULT_FORMAT, ma_node_get_output_channels(pNode, 0), *pFrameCountOut, (const void**)ppFramesIn, (void*)ppFramesOut[0]);
}
static ma_node_vtable g_ma_channel_combiner_node_vtable = {
	ma_channel_combiner_node_process_pcm_frames, NULL, MA_NODE_BUS_COUNT_UNKNOWN, 1, 0
};
MA_API ma_result ma_channel_combiner_node_init(ma_node_graph* pNodeGraph, const ma_channel_combiner_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_combiner_node* pCombinerNode) {
	ma_result result;
	ma_node_config baseConfig;
	ma_uint32 inputChannels[MA_MAX_NODE_BUS_COUNT];
	ma_uint32 outputChannels[1];
	ma_uint32 iChannel;
	if (pCombinerNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pCombinerNode);
	if (pConfig == NULL || pConfig->channels == 0 || pConfig->channels > MA_MAX_NODE_BUS_COUNT) return MA_INVALID_ARGS;
	for (iChannel = 0; iChannel < pConfig->channels; iChannel += 1) {
		inputChannels[iChannel] = 1; // Each input bus is mono
	}
	outputChannels[0] = pConfig->channels; // Output bus has combined channels
	baseConfig = pConfig->nodeConfig;
	baseConfig.vtable = &g_ma_channel_combiner_node_vtable;
	baseConfig.inputBusCount = pConfig->channels; // Set the number of input buses
	baseConfig.pInputChannels = inputChannels;
	baseConfig.pOutputChannels = outputChannels;
	result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pCombinerNode->baseNode);
	return result;
}
MA_API void ma_channel_combiner_node_uninit(ma_channel_combiner_node* pCombinerNode, const ma_allocation_callbacks* pAllocationCallbacks) {
	if (pCombinerNode == NULL) return;
	ma_node_uninit(&pCombinerNode->baseNode, pAllocationCallbacks);
}

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		ma_node_config nodeConfig;
		ma_uint32 channels;
	} ma_channel_separator_node_config;
	MA_API ma_channel_separator_node_config ma_channel_separator_node_config_init(ma_uint32 channels);
	typedef struct {
		ma_node_base baseNode;
	} ma_channel_separator_node;
	MA_API ma_result ma_channel_separator_node_init(ma_node_graph* pNodeGraph, const ma_channel_separator_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_separator_node* pSeparatorNode);
	MA_API void ma_channel_separator_node_uninit(ma_channel_separator_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks);
#ifdef __cplusplus
}
#endif
MA_API ma_channel_separator_node_config ma_channel_separator_node_config_init(ma_uint32 channels) {
	ma_channel_separator_node_config config;
	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channels = channels;
	return config;
}
static void ma_channel_separator_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
	(void)pFrameCountIn; // Input bus frame count
	ma_deinterleave_pcm_frames(DEFAULT_FORMAT, ma_node_get_input_channels(pNode, 0), *pFrameCountOut, (const void*)ppFramesIn[0], (void**)ppFramesOut);
}
static ma_node_vtable g_ma_channel_separator_node_vtable = {
	ma_channel_separator_node_process_pcm_frames, NULL, 1, MA_NODE_BUS_COUNT_UNKNOWN, 0
};
MA_API ma_result ma_channel_separator_node_init(ma_node_graph* pNodeGraph, const ma_channel_separator_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_separator_node* pSeparatorNode) {
	ma_result result;
	ma_node_config baseConfig;
	ma_uint32 inputChannels[1];
	ma_uint32 outputChannels[MA_MAX_NODE_BUS_COUNT];
	ma_uint32 iChannel;
	if (pSeparatorNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pSeparatorNode);
	if (pConfig == NULL || pConfig->channels == 0 || pConfig->channels > MA_MAX_NODE_BUS_COUNT) return MA_INVALID_ARGS;
	inputChannels[0] = pConfig->channels; // Input bus has specified channels
	for (iChannel = 0; iChannel < pConfig->channels; iChannel += 1) {
		outputChannels[iChannel] = 1; // Each output bus is mono
	}
	baseConfig = pConfig->nodeConfig;
	baseConfig.vtable = &g_ma_channel_separator_node_vtable;
	baseConfig.outputBusCount = pConfig->channels; // Set the number of output buses
	baseConfig.pInputChannels = inputChannels;
	baseConfig.pOutputChannels = outputChannels;
	result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pSeparatorNode->baseNode);
	return result;
}
MA_API void ma_channel_separator_node_uninit(ma_channel_separator_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks) {
	if (pSeparatorNode == NULL) return;
	ma_node_uninit(&pSeparatorNode->baseNode, pAllocationCallbacks);
}

// Steam Audio Binaural Node 
typedef struct {
	ma_node_config nodeConfig;
	ma_uint32 channelsIn;
	IPLAudioSettings iplAudioSettings;
	IPLContext iplContext;
	IPLHRTF iplHRTF;
} ma_steamaudio_binaural_node_config;

MA_API ma_steamaudio_binaural_node_config ma_steamaudio_binaural_node_config_init(ma_uint32 channelsIn, const IPLAudioSettings* iplAudioSettings, IPLContext iplContext, IPLHRTF iplHRTF);

typedef struct {
	ma_node_base baseNode;
	IPLAudioSettings iplAudioSettingsInternal;
	IPLContext iplContextInternal;
	IPLHRTF iplHRTFInternal;
	IPLBinauralEffect iplEffect;
	IPLVector3 ipl_direction_to_source_;
	float* ppBuffersIn[MA_MAX_CHANNELS];
	float* ppBuffersOut[2];
	void* _pHeap;
	ma_sound* ma_sound_handle_ref_;
} ma_steamaudio_binaural_node;

MA_API ma_result ma_steamaudio_binaural_node_init(ma_node_graph* pNodeGraph, const ma_steamaudio_binaural_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_steamaudio_binaural_node* pBinauralNode);
MA_API void ma_steamaudio_binaural_node_uninit(ma_steamaudio_binaural_node* pBinauralNode, const ma_allocation_callbacks* pAllocationCallbacks);
MA_API void ma_steamaudio_binaural_node_set_sound_handle(ma_steamaudio_binaural_node* pBinauralNode, ma_sound* sound_handle);

static asUINT g_period_size_frames = 256;
static std::vector<float> g_OutputDataBuffer;
static bool g_RecordOutput = false;


class AudioEngine {
public:
	ma_device device_;
	ma_engine engine_;
	ma_context context_;

	IPLAudioSettings ipl_audio_settings_;
	IPLContextSettings ipl_context_settings_;
	IPLContext ipl_context_;
	IPLHRTFSettings ipl_hrtf_settings_;
	IPLHRTF ipl_hrtf_;
	bool initialized_ = false;

	AudioEngine() {
		MA_ZERO_OBJECT(&device_);
		MA_ZERO_OBJECT(&engine_);
		MA_ZERO_OBJECT(&context_);
		MA_ZERO_OBJECT(&ipl_audio_settings_);
		MA_ZERO_OBJECT(&ipl_context_settings_);
		MA_ZERO_OBJECT(&ipl_context_);
		MA_ZERO_OBJECT(&ipl_hrtf_settings_);
		MA_ZERO_OBJECT(&ipl_hrtf_);
	}
	~AudioEngine() {
		if (initialized_) uninit();
	}

	static void device_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
		AudioEngine* audioEngine = (AudioEngine*)pDevice->pUserData;
		if (audioEngine && audioEngine->initialized_ && audioEngine->get_engine()) {
			ma_engine_read_pcm_frames(audioEngine->get_engine(), pOutput, frameCount, nullptr);
			if (g_RecordOutput) {
				const float* out_samples = (const float*)pOutput;
				// Assuming DEFAULT_CHANNELS reflects the device's output channels
				for (ma_uint32 i = 0; i < frameCount * pDevice->playback.channels; ++i) {
					g_OutputDataBuffer.push_back(out_samples[i]);
				}
			}
		}
		(void)pInput;
	}

	bool init() {
		if (initialized_) return true;
		ma_result result;
		result = ma_context_init(NULL, 0, NULL, &context_);
		if (result != MA_SUCCESS) {
			return false;
		}
		ma_device_config deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = DEFAULT_FORMAT;
		deviceConfig.playback.channels = DEFAULT_CHANNELS;
		deviceConfig.sampleRate = DEFAULT_SAMPLE_RATE;
		deviceConfig.dataCallback = AudioEngine::device_data_callback;
		deviceConfig.pUserData = this;
		deviceConfig.periodSizeInFrames = g_period_size_frames;
		deviceConfig.noClip = MA_TRUE;
		result = ma_device_init(&context_, &deviceConfig, &device_);
		if (result != MA_SUCCESS) {
			ma_context_uninit(&context_);
			return false;
		}
		DEFAULT_SAMPLE_RATE = device_.sampleRate; // Update global with actual device rate
		DEFAULT_CHANNELS = device_.playback.channels; // Update global with actual device channels
		g_period_size_frames = deviceConfig.periodSizeInFrames;

		ma_engine_config engineConfig = ma_engine_config_init();
		engineConfig.pDevice = &device_;
		engineConfig.pContext = &context_;
		engineConfig.channels = DEFAULT_CHANNELS; // Use actual device channels
		engineConfig.sampleRate = DEFAULT_SAMPLE_RATE; // Use actual device sample rate
		engineConfig.periodSizeInFrames = g_period_size_frames; // Use actual device period size

		result = ma_engine_init(&engineConfig, &engine_);
		if (result != MA_SUCCESS) {
			ma_device_uninit(&device_);
			ma_context_uninit(&context_);
			return false;
		}

		MA_ZERO_OBJECT(&ipl_audio_settings_);
		ipl_audio_settings_.samplingRate = DEFAULT_SAMPLE_RATE;
		ipl_audio_settings_.frameSize = g_period_size_frames;
		MA_ZERO_OBJECT(&ipl_context_settings_);
		ipl_context_settings_.version = STEAMAUDIO_VERSION;
		if (iplContextCreate(&ipl_context_settings_, &ipl_context_) != IPL_STATUS_SUCCESS) {
			ma_engine_uninit(&engine_); ma_device_uninit(&device_); ma_context_uninit(&context_);
			return false;
		}
		MA_ZERO_OBJECT(&ipl_hrtf_settings_);
		ipl_hrtf_settings_.type = IPL_HRTFTYPE_DEFAULT;
		ipl_hrtf_settings_.volume = 1.0f;
		if (iplHRTFCreate(ipl_context_, &ipl_audio_settings_, &ipl_hrtf_settings_, &ipl_hrtf_) != IPL_STATUS_SUCCESS) {
			iplContextRelease(&ipl_context_);
			ma_engine_uninit(&engine_); ma_device_uninit(&device_); ma_context_uninit(&context_);
			return false;
		}
		if (ma_device_start(&device_) != MA_SUCCESS) {
			iplHRTFRelease(&ipl_hrtf_); iplContextRelease(&ipl_context_);
			ma_engine_uninit(&engine_); ma_device_uninit(&device_); ma_context_uninit(&context_);
			return false;
		}
		initialized_ = true;
		return true;
	}

	void uninit() {
		if (!initialized_) return;
		if (ma_device_is_started(&device_)) ma_device_stop(&device_);
		if (ipl_hrtf_) iplHRTFRelease(&ipl_hrtf_); MA_ZERO_OBJECT(&ipl_hrtf_);
		if (ipl_context_) iplContextRelease(&ipl_context_); MA_ZERO_OBJECT(&ipl_context_);
		ma_engine_uninit(&engine_); MA_ZERO_OBJECT(&engine_);
		ma_device_uninit(&device_); MA_ZERO_OBJECT(&device_);
		ma_context_uninit(&context_); MA_ZERO_OBJECT(&context_);
		initialized_ = false;
	}

	ma_engine* get_engine() { return initialized_ ? &engine_ : nullptr; }
	ma_device* get_device() { return initialized_ ? &device_ : nullptr; }
	IPLContext get_ipl_context() { return initialized_ ? ipl_context_ : nullptr; }
	IPLHRTF get_ipl_hrtf() { return initialized_ ? ipl_hrtf_ : nullptr; }
	const IPLAudioSettings* get_ipl_audio_settings() { return initialized_ ? &ipl_audio_settings_ : nullptr; }
	ma_context* get_ma_context() { return initialized_ ? &context_ : nullptr; }
};


MA_API ma_steamaudio_binaural_node_config ma_steamaudio_binaural_node_config_init(ma_uint32 channelsIn, const IPLAudioSettings* iplAudioSettings, IPLContext iplContext, IPLHRTF iplHRTF) {
	ma_steamaudio_binaural_node_config config;
	MA_ZERO_OBJECT(&config);
	config.nodeConfig = ma_node_config_init();
	config.channelsIn = channelsIn;
	if (iplAudioSettings) config.iplAudioSettings = *iplAudioSettings;
	config.iplContext = iplContext;
	config.iplHRTF = iplHRTF;
	return config;
}

float spatial_blend_max_distance = 2.0f;
void set_spatial_blend_max_distance(float distance) { spatial_blend_max_distance = distance; }
float get_spatial_blend_max_distance() { return spatial_blend_max_distance; }


static void ma_steamaudio_binaural_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut) {
	ma_steamaudio_binaural_node* pBinauralNode = (ma_steamaudio_binaural_node*)pNode;
	if (!pBinauralNode || !pBinauralNode->iplEffect || !pBinauralNode->ma_sound_handle_ref_ || !g_audio_engine || !g_audio_engine->get_engine()) {
		if (*pFrameCountOut > 0 && ppFramesOut[0]) { // Silence output if not ready
			ma_silence_pcm_frames(ppFramesOut[0], *pFrameCountOut, DEFAULT_FORMAT, ma_node_get_output_channels(pNode, 0));
		}
		return;
	}

	IPLBinauralEffectParams binauralParams;
	IPLAudioBuffer inputBufferDesc;
	IPLAudioBuffer outputBufferDesc;
	ma_uint32 totalFramesToProcess = *pFrameCountOut;
	ma_uint32 totalFramesProcessed = 0;

	ma_vec3f ma_rel_dir = ma_sound_get_direction_to_listener(pBinauralNode->ma_sound_handle_ref_);

	binauralParams.direction.x = ma_rel_dir.x;
	binauralParams.direction.y = ma_rel_dir.y;
	binauralParams.direction.z = ma_rel_dir.z;

	float distance = 0.0f;
	ma_uint32 listenerIndex = ma_sound_get_listener_index(pBinauralNode->ma_sound_handle_ref_);
	ma_vec3f soundWorldPos = ma_sound_get_position(pBinauralNode->ma_sound_handle_ref_);
	ma_vec3f listenerWorldPos = ma_engine_listener_get_position(g_audio_engine->get_engine(), listenerIndex);
	distance = ma_vec3f_len(ma_vec3f_sub(soundWorldPos, listenerWorldPos));

	binauralParams.interpolation = (distance < 0.01f) ? IPL_HRTFINTERPOLATION_NEAREST : IPL_HRTFINTERPOLATION_BILINEAR;
	float normalizedDistance = (spatial_blend_max_distance > 0.01f) ? (distance / spatial_blend_max_distance) : 0.0f;
	binauralParams.spatialBlend = ma_min(1.0f, normalizedDistance);
	binauralParams.hrtf = pBinauralNode->iplHRTFInternal;
	binauralParams.peakDelays = NULL;

	inputBufferDesc.numChannels = (IPLint32)ma_node_get_input_channels(pNode, 0);
	outputBufferDesc.numSamples = pBinauralNode->iplAudioSettingsInternal.frameSize;
	outputBufferDesc.numChannels = 2;
	outputBufferDesc.data = pBinauralNode->ppBuffersOut;

	while (totalFramesProcessed < totalFramesToProcess) {
		ma_uint32 framesThisIteration = ma_min(totalFramesToProcess - totalFramesProcessed, (ma_uint32)pBinauralNode->iplAudioSettingsInternal.frameSize);
		if (inputBufferDesc.numChannels == 1) {
			pBinauralNode->ppBuffersIn[0] = (float*)ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, 1);
		}
		else {
			ma_deinterleave_pcm_frames(DEFAULT_FORMAT, inputBufferDesc.numChannels, framesThisIteration,
				ma_offset_pcm_frames_const_ptr_f32(ppFramesIn[0], totalFramesProcessed, inputBufferDesc.numChannels),
				(void**)pBinauralNode->ppBuffersIn);
		}
		inputBufferDesc.data = pBinauralNode->ppBuffersIn;
		inputBufferDesc.numSamples = (IPLint32)framesThisIteration;
		iplBinauralEffectApply(pBinauralNode->iplEffect, &binauralParams, &inputBufferDesc, &outputBufferDesc);
		ma_interleave_pcm_frames(DEFAULT_FORMAT, 2, framesThisIteration,
			(const void**)pBinauralNode->ppBuffersOut,
			ma_offset_pcm_frames_ptr_f32(ppFramesOut[0], totalFramesProcessed, 2));
		totalFramesProcessed += framesThisIteration;
	}
	(void)pFrameCountIn;
}

static ma_node_vtable g_ma_steamaudio_binaural_node_vtable = {
	ma_steamaudio_binaural_node_process_pcm_frames, NULL, 1, 1, 0
};

MA_API ma_result ma_steamaudio_binaural_node_init(ma_node_graph* pNodeGraph, const ma_steamaudio_binaural_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_steamaudio_binaural_node* pBinauralNode) {
	ma_result result;
	ma_node_config baseConfig;
	ma_uint32 channelsIn;
	ma_uint32 channelsOut = 2;
	IPLBinauralEffectSettings iplEffectSettings; // Renamed from iplBinauralEffectSettings
	size_t heapSizeInBytes;

	if (pBinauralNode == NULL) return MA_INVALID_ARGS;
	MA_ZERO_OBJECT(pBinauralNode);
	if (pConfig == NULL || pConfig->iplAudioSettings.frameSize == 0 || pConfig->iplContext == NULL || pConfig->iplHRTF == NULL) return MA_INVALID_ARGS;
	if (pConfig->channelsIn < 1 || pConfig->channelsIn > MA_MAX_CHANNELS) return MA_INVALID_ARGS; // Allow up to MA_MAX_CHANNELS for input flexibility
	channelsIn = pConfig->channelsIn;

	baseConfig = pConfig->nodeConfig; // Use passed-in config as base
	baseConfig.vtable = &g_ma_steamaudio_binaural_node_vtable;
	baseConfig.pInputChannels = &channelsIn;
	baseConfig.pOutputChannels = &channelsOut;
	result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pBinauralNode->baseNode);
	if (result != MA_SUCCESS) return result;

	pBinauralNode->iplAudioSettingsInternal = pConfig->iplAudioSettings;
	pBinauralNode->iplContextInternal = pConfig->iplContext;
	pBinauralNode->iplHRTFInternal = pConfig->iplHRTF;
	pBinauralNode->ma_sound_handle_ref_ = nullptr;

	MA_ZERO_OBJECT(&iplEffectSettings);
	iplEffectSettings.hrtf = pBinauralNode->iplHRTFInternal;
	if (iplBinauralEffectCreate(pBinauralNode->iplContextInternal, &pBinauralNode->iplAudioSettingsInternal, &iplEffectSettings, &pBinauralNode->iplEffect) != IPL_STATUS_SUCCESS) {
		ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
		return MA_ERROR;
	}

	heapSizeInBytes = 0;
	heapSizeInBytes += sizeof(float) * channelsOut * pBinauralNode->iplAudioSettingsInternal.frameSize;
	heapSizeInBytes += sizeof(float) * channelsIn * pBinauralNode->iplAudioSettingsInternal.frameSize;
	pBinauralNode->_pHeap = ma_malloc(heapSizeInBytes, pAllocationCallbacks);
	if (pBinauralNode->_pHeap == NULL) {
		iplBinauralEffectRelease(&pBinauralNode->iplEffect);
		ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks);
		return MA_OUT_OF_MEMORY;
	}
	pBinauralNode->ppBuffersOut[0] = (float*)pBinauralNode->_pHeap;
	pBinauralNode->ppBuffersOut[1] = (float*)ma_offset_ptr(pBinauralNode->ppBuffersOut[0], sizeof(float) * pBinauralNode->iplAudioSettingsInternal.frameSize);
	float* input_buffer_start = (float*)ma_offset_ptr(pBinauralNode->ppBuffersOut[1], sizeof(float) * pBinauralNode->iplAudioSettingsInternal.frameSize);
	for (ma_uint32 i = 0; i < channelsIn; ++i) {
		pBinauralNode->ppBuffersIn[i] = (float*)ma_offset_ptr(input_buffer_start, sizeof(float) * pBinauralNode->iplAudioSettingsInternal.frameSize * i);
	}
	return MA_SUCCESS;
}

MA_API void ma_steamaudio_binaural_node_uninit(ma_steamaudio_binaural_node* pBinauralNode, const ma_allocation_callbacks* pAllocationCallbacks) {
	if (pBinauralNode == NULL) return;
	ma_node_uninit(&pBinauralNode->baseNode, pAllocationCallbacks); // Uninit base first
	if (pBinauralNode->iplEffect) iplBinauralEffectRelease(&pBinauralNode->iplEffect);
	ma_free(pBinauralNode->_pHeap, pAllocationCallbacks);
	MA_ZERO_OBJECT(pBinauralNode);
}

MA_API void ma_steamaudio_binaural_node_set_sound_handle(ma_steamaudio_binaural_node* pBinauralNode, ma_sound* sound_handle) {
	if (pBinauralNode) pBinauralNode->ma_sound_handle_ref_ = sound_handle;
}


static mixer* g_master_mixer = nullptr;
static mixer* g_default_sound_mixer = nullptr;

struct AudioDevice { std::string name; ma_device_id id; };
static std::vector<AudioDevice> g_output_audio_devices_cache;
static std::vector<AudioDevice> g_input_audio_devices_cache;
static ma_device_id* g_current_input_device_id = nullptr;

bool soundsystem_init() {
	if (g_SoundInitialized) return true;
	if (!g_audio_engine) g_audio_engine = new AudioEngine();
	if (!g_audio_engine->init()) {
		delete g_audio_engine; g_audio_engine = nullptr; return false;
	}
	try {
		g_master_mixer = new mixer(g_audio_engine, nullptr, true);
		g_default_sound_mixer = new mixer(g_audio_engine, g_master_mixer);
	}
	catch (const std::runtime_error& e) {
		if (g_master_mixer) delete g_master_mixer;
		g_audio_engine->uninit(); delete g_audio_engine; g_audio_engine = nullptr;
		return false;
	}
	g_SoundInitialized = true;
	return true;
}

void soundsystem_free() {
	if (!g_SoundInitialized) return;
	if (g_default_sound_mixer) { delete g_default_sound_mixer; g_default_sound_mixer = nullptr; }
	if (g_master_mixer) { delete g_master_mixer; g_master_mixer = nullptr; }
	if (g_audio_engine) { g_audio_engine->uninit(); delete g_audio_engine; g_audio_engine = nullptr; }
	g_output_audio_devices_cache.clear();
	g_input_audio_devices_cache.clear();
	g_current_input_device_id = nullptr;
	g_SoundInitialized = false;
}

string sound_path_global;
pack* sound_pack_global = nullptr;

void set_sound_storage(const string& path) {
	sound_path_global = path; sound_pack_global = nullptr;
}
string get_sound_storage() { return sound_path_global; }
void set_sound_pack(pack* p) {
	if (p == nullptr && sound_pack_global != nullptr) {
		sound_pack_global = nullptr; sound_path_global = "";  return;
	}
	if (p != nullptr) { sound_pack_global = p; sound_path_global = ""; }
}
pack* get_sound_pack() { return sound_pack_global; }

void set_master_volume(float volume_db) {
	if (!g_SoundInitialized || !g_audio_engine || !g_audio_engine->get_engine()) return;
	if (volume_db > 0.0f) volume_db = 0.0f; // Clamp max
	if (volume_db < -100.0f) volume_db = -100.0f; // Reasonable min
	ma_engine_set_volume(g_audio_engine->get_engine(), ma_volume_db_to_linear(volume_db));
}
float get_master_volume() {
	if (!g_SoundInitialized || !g_audio_engine || !g_audio_engine->get_engine()) return ma_volume_linear_to_db(0.0f); // Return silence dB
	return ma_volume_linear_to_db(ma_engine_get_volume(g_audio_engine->get_engine()));
}

bool sound_global_hrtf_enabled = false;


class mixer {
public:
	ma_sound m_group_sound;
	bool m_is_group_initialized = false;
	mutable std::atomic<int> ref;
	std::unordered_set<mixer*> child_mixers;
	std::unordered_set<sound*> sounds;
	mixer* parent_mixer_ptr;
	AudioEngine* audio_engine_ref_;
	bool is_root_mixer_ = false;

	mixer(AudioEngine* engine_ref, mixer* parent = nullptr, bool root = false)
		: parent_mixer_ptr(nullptr), audio_engine_ref_(engine_ref), is_root_mixer_(root), ref(1) {
		MA_ZERO_OBJECT(&m_group_sound);
		if (!audio_engine_ref_ || !audio_engine_ref_->get_engine()) {
			return;
		}
		if (!is_root_mixer_) {
			ma_result res = ma_sound_group_init(audio_engine_ref_->get_engine(), 0, nullptr, &m_group_sound); // Init as a group
			if (res != MA_SUCCESS) {
				return;
			}
			m_is_group_initialized = true;
		} // Root mixer uses engine endpoint, no separate m_group_sound needed for its primary role.
		set_parent_mixer(parent); // Handles attachment
	}
	~mixer() {
		set_parent_mixer(nullptr); // Detach from parent
		// Child mixers & sounds are not owned by this mixer, their lifetime is managed elsewhere.
		// They should detach themselves if their parent mixer is destroyed.
		if (m_is_group_initialized && !is_root_mixer_) {
			ma_sound_uninit(&m_group_sound);
		}
	}
	void AddRef() const { ref++; }
	void Release() const { if (--ref == 0) delete this; }

	void set_parent_mixer(mixer* new_parent) {
		if (parent_mixer_ptr == new_parent || is_root_mixer_) return;
		ma_node* this_mixer_node = get_input_attachment_node(); // This is what others connect TO. We need our OUTPUT node.
		ma_node* this_mixer_output_node = is_root_mixer_ ? ma_engine_get_endpoint(audio_engine_ref_->get_engine()) : (ma_node*)&m_group_sound;


		if (parent_mixer_ptr) {
			if (this_mixer_output_node) ma_node_detach_all_output_buses(this_mixer_output_node); // Detach our output
			parent_mixer_ptr->child_mixers.erase(this);
		}
		parent_mixer_ptr = new_parent;
		if (parent_mixer_ptr) {
			ma_node* parent_input_node = parent_mixer_ptr->get_input_attachment_node();
			if (this_mixer_output_node && parent_input_node) {
				ma_node_attach_output_bus(this_mixer_output_node, 0, parent_input_node, 0);
			}
			parent_mixer_ptr->child_mixers.insert(this);
		}
		else if (!is_root_mixer_ && g_master_mixer && this != g_master_mixer) { // Default to master if not root
			ma_node* master_input_node = g_master_mixer->get_input_attachment_node();
			if (this_mixer_output_node && master_input_node) {
				ma_node_attach_output_bus(this_mixer_output_node, 0, master_input_node, 0);
			}
			g_master_mixer->child_mixers.insert(this);
			parent_mixer_ptr = g_master_mixer;
		}
	}
	ma_engine* get_engine_ref() { return audio_engine_ref_ ? audio_engine_ref_->get_engine() : nullptr; }
	ma_node* get_input_attachment_node() { // Node for sounds/children to connect their output to
		if (is_root_mixer_) return ma_engine_get_endpoint(audio_engine_ref_->get_engine());
		return m_is_group_initialized ? (ma_node*)&m_group_sound : nullptr;
	}
};

static ma_mutex g_device_enum_mutex; // Mutex for device enumeration cache

static void init_device_enum_mutex() {
	ma_mutex_init(&g_device_enum_mutex);
}
static void uninit_device_enum_mutex() {
	ma_mutex_uninit(&g_device_enum_mutex);
}

// Call init_device_enum_mutex() in soundsystem_init() after engine is up, before first enum
// Call uninit_device_enum_mutex() in soundsystem_free()

static std::vector<AudioDevice> GetOutputAudioDevicesInternal() {
	std::vector<AudioDevice> audioDevices;
	if (!g_audio_engine || !g_audio_engine->get_ma_context()) return audioDevices;

	ma_mutex_lock(&g_device_enum_mutex); // Lock before accessing/modifying cache or enumerating
	if (!g_output_audio_devices_cache.empty()) { // Use cache if available
		audioDevices = g_output_audio_devices_cache;
		ma_mutex_unlock(&g_device_enum_mutex);
		return audioDevices;
	}
	ma_mutex_unlock(&g_device_enum_mutex); // Unlock for enumeration if cache was empty

	struct UserData { std::vector<AudioDevice>* devices_list; };
	UserData user_data = { &audioDevices }; // Local vector to fill
	ma_context_enumerate_devices(g_audio_engine->get_ma_context(),
		[](ma_context*, ma_device_type deviceType, const ma_device_info* pInfo, void* pUserData) -> ma_bool32 {
			if (deviceType == ma_device_type_playback) {
				((UserData*)pUserData)->devices_list->push_back({ pInfo->name, pInfo->id });
			}
			return MA_TRUE;
		}, &user_data);

	ma_mutex_lock(&g_device_enum_mutex);
	g_output_audio_devices_cache = audioDevices; // Update cache
	ma_mutex_unlock(&g_device_enum_mutex);
	return audioDevices;
}

static std::vector<AudioDevice> GetInputAudioDevicesInternal() {
	std::vector<AudioDevice> audioDevices;
	if (!g_audio_engine || !g_audio_engine->get_ma_context()) return audioDevices;

	ma_mutex_lock(&g_device_enum_mutex);
	if (!g_input_audio_devices_cache.empty()) {
		audioDevices = g_input_audio_devices_cache;
		ma_mutex_unlock(&g_device_enum_mutex);
		return audioDevices;
	}
	ma_mutex_unlock(&g_device_enum_mutex);

	struct UserData { std::vector<AudioDevice>* devices_list; };
	UserData user_data = { &audioDevices };
	ma_context_enumerate_devices(g_audio_engine->get_ma_context(),
		[](ma_context*, ma_device_type deviceType, const ma_device_info* pInfo, void* pUserData) -> ma_bool32 {
			if (deviceType == ma_device_type_capture) {
				((UserData*)pUserData)->devices_list->push_back({ pInfo->name, pInfo->id });
			}
			return MA_TRUE;
		}, &user_data);

	ma_mutex_lock(&g_device_enum_mutex);
	g_input_audio_devices_cache = audioDevices;
	ma_mutex_unlock(&g_device_enum_mutex);
	return audioDevices;
}

CScriptArray* get_output_audio_devices() {
	if (!g_SoundInitialized && !soundsystem_init()) { // Init if not already, handle failure
		return CScriptArray::Create(asGetActiveContext()->GetEngine()->GetTypeInfoByDecl("array<string>"));
	}
	if (!g_audio_engine) return CScriptArray::Create(asGetActiveContext()->GetEngine()->GetTypeInfoByDecl("array<string>"));
	std::vector<AudioDevice> devices = GetOutputAudioDevicesInternal(); // Use internal helper
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)devices.size());
	for (asUINT i = 0; i < devices.size(); ++i) {
		array->SetValue(i, &devices[i].name);
	}
	return array;
}

CScriptArray* get_input_audio_devices() {
	if (!g_SoundInitialized && !soundsystem_init()) {
		return CScriptArray::Create(asGetActiveContext()->GetEngine()->GetTypeInfoByDecl("array<string>"));
	}
	if (!g_audio_engine) return CScriptArray::Create(asGetActiveContext()->GetEngine()->GetTypeInfoByDecl("array<string>"));
	std::vector<AudioDevice> devices = GetInputAudioDevicesInternal();
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)devices.size());
	for (asUINT i = 0; i < devices.size(); ++i) {
		array->SetValue(i, &devices[i].name);
	}
	return array;
}

bool set_output_audio_device(asUINT id_index) {
	if (!g_SoundInitialized && !soundsystem_init()) return false;
	if (!g_audio_engine || !g_audio_engine->get_device() || !g_audio_engine->get_ma_context()) return false;

	ma_mutex_lock(&g_device_enum_mutex); // Lock for cache access
	std::vector<AudioDevice> current_devices = g_output_audio_devices_cache; // Copy cache
	ma_mutex_unlock(&g_device_enum_mutex);

	if (current_devices.empty()) {
		current_devices = GetOutputAudioDevicesInternal(); // Populate if empty
	}
	if (id_index >= current_devices.size()) return false;

	ma_device_stop(&g_audio_engine->device_); // Stop current device
	ma_device_uninit(&g_audio_engine->device_);

	ma_device_config devConfig = ma_device_config_init(ma_device_type_playback);
	devConfig.playback.pDeviceID = &current_devices[id_index].id; // Use ID from (potentially refreshed) list
	devConfig.playback.format = DEFAULT_FORMAT;
	devConfig.playback.channels = DEFAULT_CHANNELS; // Engine will use this
	devConfig.sampleRate = DEFAULT_SAMPLE_RATE; // Engine will use this
	devConfig.dataCallback = AudioEngine::device_data_callback;
	devConfig.pUserData = g_audio_engine;
	devConfig.periodSizeInFrames = g_period_size_frames;
	devConfig.noClip = MA_TRUE;

	ma_result result = ma_device_init(g_audio_engine->get_ma_context(), &devConfig, &g_audio_engine->device_);
	if (result != MA_SUCCESS) {
		return false;
	}
	DEFAULT_SAMPLE_RATE = g_audio_engine->device_.sampleRate; // Update globals from new device
	DEFAULT_CHANNELS = g_audio_engine->device_.playback.channels;
	g_period_size_frames = g_audio_engine->device_.playback.internalPeriodSizeInFrames;


	if (ma_device_start(&g_audio_engine->device_) != MA_SUCCESS) {
		ma_device_uninit(&g_audio_engine->device_);
		return false;
	}
	ma_mutex_lock(&g_device_enum_mutex);
	g_output_audio_devices_cache = current_devices; // Update cache with list that was used for ID
	ma_mutex_unlock(&g_device_enum_mutex);
	return true;
}

bool set_input_audio_device(asUINT id_index) {
	if (!g_SoundInitialized && !soundsystem_init()) return false;
	std::vector<AudioDevice> current_input_devs = GetInputAudioDevicesInternal(); // Ensure cache is populated
	if (id_index >= current_input_devs.size()) return false;

	static ma_device_id selected_input_id; // Static to persist
	selected_input_id = current_input_devs[id_index].id;
	g_current_input_device_id = &selected_input_id;
	return true;
}


class pcm_ring_buffer {
public:
	ma_pcm_rb rb;
	ma_uint32 channels_;
	ma_uint32 sample_rate_;
	mutable std::atomic<int> ref_count;

	pcm_ring_buffer(ma_uint32 channels = DEFAULT_CHANNELS, ma_uint32 sample_rate = DEFAULT_SAMPLE_RATE, ma_uint32 bufferSizeInFrames = 1024)
		: channels_(channels), sample_rate_(sample_rate), ref_count(1) {
		MA_ZERO_OBJECT(&rb);
		if (ma_pcm_rb_init(DEFAULT_FORMAT, channels_, bufferSizeInFrames, nullptr, nullptr, &rb) != MA_SUCCESS) {
			throw std::runtime_error("Failed to initialize PCM ring buffer");
		}
		ma_pcm_rb_set_sample_rate(&rb, sample_rate_);
	}
	~pcm_ring_buffer() { ma_pcm_rb_uninit(&rb); }
	void write(const std::string& data) {
		if (data.empty()) return;
		ma_uint32 frame_size_bytes = ma_get_bytes_per_frame(DEFAULT_FORMAT, channels_);
		if (frame_size_bytes == 0) return;
		ma_uint32 sizeInFrames = static_cast<ma_uint32>(data.size() / frame_size_bytes);
		if (sizeInFrames == 0) return;
		void* bufferOut = nullptr;
		ma_uint32 framesToWrite = sizeInFrames;
		if (ma_pcm_rb_acquire_write(&rb, &framesToWrite, &bufferOut) != MA_SUCCESS || framesToWrite == 0) {
			return;
		}
		// Only copy framesToWrite worth of data
		std::memcpy(bufferOut, data.data(), framesToWrite * frame_size_bytes);
		ma_pcm_rb_commit_write(&rb, framesToWrite);
	}
	std::string read(size_t size_bytes) {
		void* bufferIn = nullptr;
		ma_uint32 frame_size_bytes = ma_get_bytes_per_frame(DEFAULT_FORMAT, channels_);
		if (frame_size_bytes == 0) return "";
		ma_uint32 framesToReadRequest = static_cast<ma_uint32>(size_bytes / frame_size_bytes);
		ma_uint32 framesAvailable = framesToReadRequest;
		if (ma_pcm_rb_acquire_read(&rb, &framesAvailable, &bufferIn) != MA_SUCCESS || framesAvailable == 0) {
			return "";
		}
		std::string result(static_cast<const char*>(bufferIn), framesAvailable * frame_size_bytes); // Use const char*
		ma_pcm_rb_commit_read(&rb, framesAvailable);
		return result;
	}
	void reset() { ma_pcm_rb_reset(&rb); }
	void add_ref() const { ref_count++; } // Mark const
	void release() const { if (--ref_count == 0) { delete this; } } // Mark const
};


class MINIAUDIO_IMPLEMENTATION sound {
public:
	ma_sound handle_;
	bool handle_initialized_ = false;
	ma_decoder decoder_;
	bool decoderInitialized_ = false;
	ma_steamaudio_binaural_node m_binauralNode;
	bool m_binauralNodeInitialized = false;
	ma_reverb_node m_reverbNode; bool m_reverbNodeInitialized = false;
	ma_vocoder_node m_vocoderNode; bool m_vocoderNodeInitialized = false;
	ma_delay_node m_delayNode; bool m_delayNodeInitialized = false; // Assuming ma_delay_node is defined
	ma_ltrim_node m_ltrimNode; bool m_ltrimNodeInitialized = false;
	ma_channel_separator_node m_separatorNode; bool m_separatorNodeInitialized = false;
	ma_channel_combiner_node m_combinerNode; bool m_combinerNodeInitialized = false;
	ma_hpf_node highpass_node_; bool highpass_node_initialized_ = false;
	ma_lpf_node lowpass_node_; bool lowpass_node_initialized_ = false;
	ma_notch_node notch_node_; bool notch_node_initialized_ = false;
	ma_reverb_node_config reverbNodeConfig_;
	ma_vocoder_node_config vocoderNodeConfig_;
	ma_delay_node_config delayNodeConfig_;
	ma_ltrim_node_config trimNodeConfig_;
	ma_channel_separator_node_config separatorNodeConfig_;
	ma_channel_combiner_node_config combinerNodeConfig_;
	ma_hpf_node_config highpass_config_;
	ma_lpf_node_config lowpass_config_;
	ma_notch_node_config notch_config_;
	ma_steamaudio_binaural_node_config binauralNodeConfig_;
	std::vector<ma_node*> active_effect_nodes_;
	ma_audio_buffer m_buffer;
	bool buffer_initialized_ = false;
	string file_path_;
	mixer* current_mixer_ptr_;
	mutable std::atomic<int> ref;

	sound(const string& filename = "") : current_mixer_ptr_(nullptr), ref(1) {
		MA_ZERO_OBJECT(&handle_); MA_ZERO_OBJECT(&decoder_);
		MA_ZERO_OBJECT(&m_binauralNode); MA_ZERO_OBJECT(&m_reverbNode); /* ... zero others ... */
		MA_ZERO_OBJECT(&m_buffer);
		if (!g_SoundInitialized && !soundsystem_init()) {
			return;
		}
		current_mixer_ptr_ = g_default_sound_mixer;
		if (current_mixer_ptr_) current_mixer_ptr_->sounds.insert(this);
		if (!filename.empty()) this->load(filename);
	}
	~sound() {
		if (handle_initialized_) this->close();
		if (current_mixer_ptr_) current_mixer_ptr_->sounds.erase(this);
	}
	void AddRef() const { ref++; }
	void Release() const { if (--ref == 0) delete this; }

	ma_node* get_mixer_input_node() const {
		if (current_mixer_ptr_) return current_mixer_ptr_->get_input_attachment_node();
		if (g_audio_engine && g_audio_engine->get_engine()) return ma_engine_get_endpoint(g_audio_engine->get_engine()); // Fallback
		return nullptr;
	}
	void rebuild_node_attachments() {
		if (!handle_initialized_ || !g_audio_engine || !g_audio_engine->get_engine()) return;
		ma_node* current_source_node = (ma_node*)&handle_;
		ma_node* final_destination_node = get_mixer_input_node();
		if (!final_destination_node) return;
		ma_node_detach_all_output_buses(current_source_node); // Detach sound from previous chain
		for (ma_node* effect_node : active_effect_nodes_) { // Detach all effects too
			if (effect_node) ma_node_detach_all_output_buses(effect_node);
		}
		// Rebuild chain: sound -> effect1 -> effect2 -> ... -> mixer_input
		for (ma_node* effect_node : active_effect_nodes_) {
			if (effect_node) {
				ma_node_attach_output_bus(current_source_node, 0, effect_node, 0);
				current_source_node = effect_node;
			}
		}
		ma_node_attach_output_bus(current_source_node, 0, final_destination_node, 0);
	}

	bool load(const string& filename) {
		if (!g_audio_engine || !g_audio_engine->get_engine()) return false;
		if (handle_initialized_) this->close();
		string full_path;
		if (sound_pack_global && sound_pack_global->active()) {
			string file_content = sound_pack_global->get_file(filename);
			if (!file_content.empty()) {
				return this->load_from_memory(file_content, file_content.size());
			}
			return false;
		}
		else if (!sound_path_global.empty()) {
			full_path = sound_path_global + "/" + filename;
		}
		else {
			full_path = filename;
		}
		ma_sound_config sound_config = ma_sound_config_init();
		sound_config.pFilePath = full_path.c_str();
		sound_config.flags = MA_SOUND_FLAG_NO_SPATIALIZATION; // Start non-spatialized
		// pInitialAttachment will be handled by rebuild_node_attachments after init
		ma_result loading_result = ma_sound_init_ex(g_audio_engine->get_engine(), &sound_config, &handle_);
		if (loading_result != MA_SUCCESS) {
			return false;
		}
		handle_initialized_ = true; file_path_ = full_path;
		ma_sound_set_attenuation_model(&handle_, ma_attenuation_model_linear);
		ma_sound_set_rolloff(&handle_, 0.75f);
		ma_sound_set_directional_attenuation_factor(&handle_, 1.0f);
		rebuild_node_attachments();
		if (sound_global_hrtf_enabled) this->set_hrtf(true);
		return true;
	}
	bool load_from_memory(const string& data, size_t stream_size) {
		if (!g_audio_engine || !g_audio_engine->get_engine() || data.empty()) return false;
		if (handle_initialized_) this->close();
		ma_result r = ma_decoder_init_memory(data.data(), stream_size, NULL, &decoder_);
		if (r != MA_SUCCESS) { return false; }
		decoderInitialized_ = true;
		ma_sound_config sound_config = ma_sound_config_init();
		sound_config.pDataSource = &decoder_;
		sound_config.flags = MA_SOUND_FLAG_NO_SPATIALIZATION;
		ma_result loading_result = ma_sound_init_ex(g_audio_engine->get_engine(), &sound_config, &handle_);
		if (loading_result != MA_SUCCESS) {
			if (decoderInitialized_) { ma_decoder_uninit(&decoder_); decoderInitialized_ = false; }
			return false;
		}
		handle_initialized_ = true; file_path_.clear();
		ma_sound_set_attenuation_model(&handle_, ma_attenuation_model_linear);
		ma_sound_set_rolloff(&handle_, 0.75f);
		ma_sound_set_directional_attenuation_factor(&handle_, 1.0f);
		rebuild_node_attachments();
		if (sound_global_hrtf_enabled) this->set_hrtf(true);
		return true;
	}
	bool load_pcm(const string& data_str, size_t size_bytes, int channels, int sample_rate, int bits_per_sample) {
		if (!g_audio_engine || !g_audio_engine->get_engine() || data_str.empty()) return false;
		if (handle_initialized_) this->close();
		if (buffer_initialized_) { ma_audio_buffer_uninit(&m_buffer); buffer_initialized_ = false; }
		ma_format pcm_format;
		switch (bits_per_sample) {
		case 8: pcm_format = ma_format_u8; break; case 16: pcm_format = ma_format_s16; break;
		case 24: pcm_format = ma_format_s24; break; case 32: pcm_format = ma_format_f32; break;
		default: return false;
		}
		ma_uint32 frame_size_bytes = ma_get_bytes_per_frame(pcm_format, channels);
		if (frame_size_bytes == 0) { return false; }
		ma_uint64 frame_count = size_bytes / frame_size_bytes;
		if (frame_count == 0) { return false; }
		ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(pcm_format, channels, frame_count, data_str.data(), nullptr);
		bufferConfig.sampleRate = sample_rate;
		ma_result result = ma_audio_buffer_init(&bufferConfig, &m_buffer);
		if (result != MA_SUCCESS) { return false; }
		buffer_initialized_ = true;
		ma_sound_config sound_config = ma_sound_config_init();
		sound_config.pDataSource = &m_buffer;
		sound_config.flags = MA_SOUND_FLAG_NO_SPATIALIZATION;
		ma_result loading_result = ma_sound_init_ex(g_audio_engine->get_engine(), &sound_config, &handle_);
		if (loading_result != MA_SUCCESS) {
			if (buffer_initialized_) { ma_audio_buffer_uninit(&m_buffer); buffer_initialized_ = false; }
			return false;
		}
		handle_initialized_ = true; file_path_.clear();
		ma_sound_set_attenuation_model(&handle_, ma_attenuation_model_linear);
		ma_sound_set_rolloff(&handle_, 0.75f);
		ma_sound_set_directional_attenuation_factor(&handle_, 1.0f);
		rebuild_node_attachments();
		if (sound_global_hrtf_enabled) this->set_hrtf(true);
		return true;
	}
	bool load_pcm_buffer(pcm_ring_buffer* buffer_obj) {
		if (!g_audio_engine || !g_audio_engine->get_engine() || !buffer_obj) return false;
		if (handle_initialized_) this->close();
		ma_sound_config sound_config = ma_sound_config_init();
		sound_config.pDataSource = &buffer_obj->rb;
		sound_config.flags = MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM; // Assume stream for ring buffer
		ma_result loading_result = ma_sound_init_ex(g_audio_engine->get_engine(), &sound_config, &handle_);
		if (loading_result != MA_SUCCESS) { return false; }
		handle_initialized_ = true; file_path_.clear();
		ma_sound_set_attenuation_model(&handle_, ma_attenuation_model_linear);
		ma_sound_set_rolloff(&handle_, 0.75f);
		ma_sound_set_directional_attenuation_factor(&handle_, 1.0f);
		rebuild_node_attachments();
		if (sound_global_hrtf_enabled) this->set_hrtf(true);
		return true;
	}

	void set_mixer(mixer* new_mixer_ptr) {
		if (current_mixer_ptr_ == new_mixer_ptr) return;
		if (current_mixer_ptr_) current_mixer_ptr_->sounds.erase(this);
		current_mixer_ptr_ = new_mixer_ptr ? new_mixer_ptr : g_default_sound_mixer;
		if (current_mixer_ptr_) current_mixer_ptr_->sounds.insert(this);
		rebuild_node_attachments();
	}
	string get_file_path() const { return file_path_; }
	void set_fade_time(float volume_beg_db, float volume_end_db, float time_ms) {
		if (!handle_initialized_) return;
		float lin_start = (volume_beg_db <= -100.0f) ? ma_sound_get_volume(&handle_) : ma_volume_db_to_linear(volume_beg_db);
		float lin_end = ma_volume_db_to_linear(volume_end_db);
		ma_sound_set_fade_in_milliseconds(&handle_, lin_start, lin_end, static_cast<ma_uint64>(time_ms));
	}
	bool play() {
		if (!handle_initialized_) return false;
		ma_sound_set_looping(&handle_, false);
		return ma_sound_start(&handle_) == MA_SUCCESS;
	}
	bool play_looped() {
		if (!handle_initialized_) return false;
		ma_sound_set_looping(&handle_, true);
		return ma_sound_start(&handle_) == MA_SUCCESS;
	}
	bool pause() {
		if (!handle_initialized_ || !ma_sound_is_playing(&handle_)) return true;
		return ma_sound_stop(&handle_) == MA_SUCCESS;
	}
	bool play_wait() {
		if (!this->play()) return false;
		while (is_playing()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		return true;
	}
	bool stop() {
		if (!handle_initialized_) return false;
		ma_sound_stop(&handle_);
		ma_sound_seek_to_pcm_frame(&handle_, 0);
		return true;
	}
	bool close() {
		if (!handle_initialized_) return false;
		if (ma_sound_is_playing(&handle_)) ma_sound_stop(&handle_); // Stop before uninit
		if (m_binauralNodeInitialized) { ma_steamaudio_binaural_node_uninit(&m_binauralNode, nullptr); m_binauralNodeInitialized = false; }
		if (m_reverbNodeInitialized) { ma_reverb_node_uninit(&m_reverbNode, nullptr); m_reverbNodeInitialized = false; }
		active_effect_nodes_.clear();
		if (decoderInitialized_) { ma_decoder_uninit(&decoder_); decoderInitialized_ = false; }
		if (buffer_initialized_) { ma_audio_buffer_uninit(&m_buffer); buffer_initialized_ = false; }
		ma_sound_uninit(&handle_);
		handle_initialized_ = false;
		MA_ZERO_OBJECT(&handle_);
		file_path_.clear();
		return true;
	}

	void add_effect_node(ma_node* effect_node_ptr) {
		if (!effect_node_ptr || !handle_initialized_) return;
		auto it = std::find(active_effect_nodes_.begin(), active_effect_nodes_.end(), effect_node_ptr);
		if (it == active_effect_nodes_.end()) {
			active_effect_nodes_.push_back(effect_node_ptr);
			rebuild_node_attachments();
		}
	}
	void remove_effect_node(ma_node* effect_node_to_remove) {
		if (!effect_node_to_remove || !handle_initialized_) return;
		auto it = std::find(active_effect_nodes_.begin(), active_effect_nodes_.end(), effect_node_to_remove);
		if (it != active_effect_nodes_.end()) {
			active_effect_nodes_.erase(it);
			rebuild_node_attachments();
			// The caller is responsible for uninitializing the effect_node_to_remove itself
		}
	}

	void set_fx(const string& fx_name) {
		if (!handle_initialized_ || !g_audio_engine || !g_audio_engine->get_engine()) return;
		if (fx_name == "reverb" && !m_reverbNodeInitialized) {
			reverbNodeConfig_ = ma_reverb_node_config_init(DEFAULT_CHANNELS, DEFAULT_SAMPLE_RATE);
			if (ma_reverb_node_init(ma_engine_get_node_graph(g_audio_engine->get_engine()), &reverbNodeConfig_, NULL, &m_reverbNode) == MA_SUCCESS) {
				m_reverbNodeInitialized = true; add_effect_node((ma_node*)&m_reverbNode);
			}
		}
		else if (fx_name == "hrtf" && !m_binauralNodeInitialized) {
			set_hrtf(true);
		}
	}
	void delete_fx(const string& fx_name) {
		if (!handle_initialized_) return;
		if (fx_name == "reverb" && m_reverbNodeInitialized) {
			remove_effect_node((ma_node*)&m_reverbNode);
			ma_reverb_node_uninit(&m_reverbNode, NULL); m_reverbNodeInitialized = false;
		}
		else if (fx_name == "hrtf" && m_binauralNodeInitialized) {
			set_hrtf(false);
		}
	}
	void set_reverb_parameters(float dry, float wet, float room_size, float damping, float mode) {
		if (!m_reverbNodeInitialized) return;
		verblib_set_dry(&m_reverbNode.reverb, dry); verblib_set_wet(&m_reverbNode.reverb, wet);
		verblib_set_room_size(&m_reverbNode.reverb, room_size); verblib_set_damping(&m_reverbNode.reverb, damping);
		verblib_set_mode(&m_reverbNode.reverb, mode);
	}
	void set_delay_parameters(float dry, float wet, float decay_ms) {
		if (!m_delayNodeInitialized) return;
	}
	void set_position_3d(float x, float y, float z) {
		if (!handle_initialized_) return;
		ma_sound_set_spatialization_enabled(&handle_, MA_TRUE);
		ma_sound_set_position(&handle_, x, y, z);
	}
	void set_velocity_3d(float vx, float vy, float vz) {
		if (!handle_initialized_) return;
		if (!ma_sound_is_spatialization_enabled(&handle_)) ma_sound_set_spatialization_enabled(&handle_, MA_TRUE);
		ma_sound_set_velocity(&handle_, vx, vy, vz);
	}
	void set_direction_3d(float dx, float dy, float dz) {
		if (!handle_initialized_) return;
		if (!ma_sound_is_spatialization_enabled(&handle_)) ma_sound_set_spatialization_enabled(&handle_, MA_TRUE);
		ma_sound_set_direction(&handle_, dx, dy, dz);
	}
	void set_hrtf(bool enable_hrtf) {
		if (!handle_initialized_ || !g_audio_engine || !g_audio_engine->get_engine() || !g_audio_engine->get_ipl_context()) return;
		if (enable_hrtf) {
			if (m_binauralNodeInitialized) return;
			ma_uint32 sound_channels = DEFAULT_CHANNELS;
			ma_sound_get_data_format(&handle_, nullptr, &sound_channels, nullptr, nullptr, 0);

			binauralNodeConfig_ = ma_steamaudio_binaural_node_config_init(
				sound_channels,
				g_audio_engine->get_ipl_audio_settings(),
				g_audio_engine->get_ipl_context(),
				g_audio_engine->get_ipl_hrtf()
			);
			if (ma_steamaudio_binaural_node_init(ma_engine_get_node_graph(g_audio_engine->get_engine()), &binauralNodeConfig_, NULL, &m_binauralNode) == MA_SUCCESS) {
				m_binauralNodeInitialized = true;
				ma_steamaudio_binaural_node_set_sound_handle(&m_binauralNode, &handle_);
				add_effect_node((ma_node*)&m_binauralNode);
				ma_sound_set_spatialization_enabled(&handle_, MA_TRUE);
				ma_sound_set_directional_attenuation_factor(&handle_, 0.0f);
			}
		}
		else {
			if (!m_binauralNodeInitialized) return;
			remove_effect_node((ma_node*)&m_binauralNode);
			ma_steamaudio_binaural_node_uninit(&m_binauralNode, NULL);
			m_binauralNodeInitialized = false;
			ma_sound_set_directional_attenuation_factor(&handle_, 1.0f);
		}
	}
	bool get_hrtf() const { return m_binauralNodeInitialized; }
	void set_rolloff_factor(float factor) {
		if (!handle_initialized_) return; ma_sound_set_rolloff(&handle_, factor);
	}
	void set_directional_attenuation(float factor) {
		if (!handle_initialized_) return; ma_sound_set_directional_attenuation_factor(&handle_, factor);
	}
	void set_doppler_factor(float factor) {
		if (!handle_initialized_) return; ma_sound_set_doppler_factor(&handle_, factor);
	}
	bool seek(float new_position_ms) {
		if (!handle_initialized_ || new_position_ms < 0.0f) return false;
		ma_uint32 sr = (g_audio_engine && g_audio_engine->get_engine()) ? g_audio_engine->get_engine()->sampleRate : DEFAULT_SAMPLE_RATE;
		ma_uint64 pcm_frame = static_cast<ma_uint64>((new_position_ms / 1000.0f) * sr);
		return ma_sound_seek_to_pcm_frame(&handle_, pcm_frame) == MA_SUCCESS;
	}
	void set_looping(bool looping) {
		if (!handle_initialized_) return; ma_sound_set_looping(&handle_, looping);
	}
	bool get_looping() const {
		if (!handle_initialized_) return false; return ma_sound_is_looping(&handle_);
	}
	float get_pan() const {
		if (!handle_initialized_) return 0.0f; return ma_sound_get_pan(&handle_);
	}
	void set_pan(float pan_linear) {
		if (!handle_initialized_) return; ma_sound_set_pan(&handle_, ma_clamp(pan_linear, -1.0f, 1.0f));
	}
	float get_volume() const {
		if (!handle_initialized_) return ma_volume_linear_to_db(0.0f);
		return ma_volume_linear_to_db(ma_sound_get_volume(&handle_));
	}
	void set_volume(float volume_db) {
		if (!handle_initialized_) return;
		ma_sound_set_volume(&handle_, ma_volume_db_to_linear(ma_clamp(volume_db, -100.0f, 0.0f)));
	}
	float get_pitch() const {
		if (!handle_initialized_) return 1.0f; return ma_sound_get_pitch(&handle_);
	}
	void set_pitch(float pitch_multiplier) {
		if (!handle_initialized_ || pitch_multiplier <= 0.0f) return; // Pitch must be > 0
		ma_sound_set_pitch(&handle_, pitch_multiplier);
	}
	bool is_active() const { return handle_initialized_; }
	bool is_playing() const {
		if (!handle_initialized_) return false; return ma_sound_is_playing(&handle_);
	}
	bool is_paused() const {
		if (!handle_initialized_) return false;
		return !ma_sound_is_playing(&handle_) && (get_position() > 0.001f) && !at_end();
	}
	bool at_end() const {
		if (!handle_initialized_) return true; return ma_sound_at_end(&handle_);
	}
	float get_position() {
		if (!handle_initialized_) return 0.0f;
		ma_uint64 pcm_frame_pos = 0;
		ma_sound_get_cursor_in_pcm_frames(&handle_, &pcm_frame_pos);
		ma_uint32 sr = (g_audio_engine && g_audio_engine->get_engine()) ? g_audio_engine->get_engine()->sampleRate : DEFAULT_SAMPLE_RATE;
		if (sr == 0) return 0.0f; // Avoid division by zero if engine not ready
		return static_cast<float>(pcm_frame_pos) * 1000.0f / sr;
	}
	float get_length() {
		if (!handle_initialized_) return 0.0f;
		ma_uint64 pcm_frame_len = 0;
		ma_sound_get_length_in_pcm_frames(&handle_, &pcm_frame_len);
		ma_uint32 sr = (g_audio_engine && g_audio_engine->get_engine()) ? g_audio_engine->get_engine()->sampleRate : DEFAULT_SAMPLE_RATE;
		if (sr == 0) return 0.0f;
		return static_cast<float>(pcm_frame_len) * 1000.0f / sr;
	}
	void set_length(float length_ms = 0.0f) {
		if (!handle_initialized_) return;
		if (length_ms <= 0.0f) {
			ma_sound_set_stop_time_in_pcm_frames(&handle_, 0);
			return;
		}
		ma_uint32 sr = (g_audio_engine && g_audio_engine->get_engine()) ? g_audio_engine->get_engine()->sampleRate : DEFAULT_SAMPLE_RATE;
		if (sr == 0) return;
		ma_uint64 pcm_frames = static_cast<ma_uint64>((length_ms / 1000.0f) * sr);
		ma_sound_set_stop_time_in_pcm_frames(&handle_, pcm_frames);
	}
	float get_sample_rate() {
		if (!handle_initialized_) return static_cast<float>(DEFAULT_SAMPLE_RATE);
		ma_format format_ignored; ma_uint32 channels_ignored, ds_sample_rate;
		if (ma_sound_get_data_format(&handle_, &format_ignored, &channels_ignored, &ds_sample_rate, NULL, 0) == MA_SUCCESS) {
			return static_cast<float>(ds_sample_rate);
		}
		return static_cast<float>(DEFAULT_SAMPLE_RATE);
	}
};

void set_sound_global_hrtf(bool hrtf) { sound_global_hrtf_enabled = hrtf; }
bool get_sound_global_hrtf() { return sound_global_hrtf_enabled; }

static void audio_recorder_data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
	std::vector<float>* data_buffer = static_cast<std::vector<float>*>(pDevice->pUserData);
	if (!data_buffer || !pInput) return;
	const float* input_samples = static_cast<const float*>(pInput);
	size_t samples_to_copy = static_cast<size_t>(frameCount) * pDevice->capture.channels;
	data_buffer->insert(data_buffer->end(), input_samples, input_samples + samples_to_copy);
	(void)pOutput;
}

class MINIAUDIO_IMPLEMENTATION audio_recorder {
public:
	std::vector<float> recording_data_buffer_;
	ma_device_config deviceConfig_;
	ma_device recording_device_;
	bool started_ = false;
	mutable std::atomic<int> ref;
	bool record_engine_output_ = false;

	audio_recorder() : started_(false), ref(1), record_engine_output_(false) {
		MA_ZERO_OBJECT(&recording_device_); MA_ZERO_OBJECT(&deviceConfig_);
	}
	~audio_recorder() { if (started_) stop(); }
	void AddRef() const { ref++; }
	void Release() const { if (--ref == 0) delete this; }
	void start() {
		if (started_) stop();
		recording_data_buffer_.clear();
		if (record_engine_output_) {
			g_OutputDataBuffer.clear();
			g_RecordOutput = true;
			started_ = true; return;
		}
		if (!g_audio_engine || !g_audio_engine->get_ma_context()) {
			return;
		}
		deviceConfig_ = ma_device_config_init(ma_device_type_capture);
		if (g_current_input_device_id) deviceConfig_.capture.pDeviceID = g_current_input_device_id;
		deviceConfig_.capture.format = DEFAULT_FORMAT;
		deviceConfig_.capture.channels = DEFAULT_CHANNELS;
		deviceConfig_.sampleRate = DEFAULT_SAMPLE_RATE;
		deviceConfig_.dataCallback = audio_recorder_data_callback;
		deviceConfig_.pUserData = &recording_data_buffer_;
		if (ma_device_init(g_audio_engine->get_ma_context(), &deviceConfig_, &recording_device_) != MA_SUCCESS) {
			started_ = false; return;
		}
		if (ma_device_start(&recording_device_) != MA_SUCCESS) {
			ma_device_uninit(&recording_device_); started_ = false; return;
		}
		started_ = true;
	}
	void stop() {
		if (!started_) return;
		if (record_engine_output_) {
			g_RecordOutput = false;
			recording_data_buffer_ = g_OutputDataBuffer;
			started_ = false; return;
		}
		if (ma_device_is_started(&recording_device_)) ma_device_stop(&recording_device_);
		ma_device_uninit(&recording_device_);
		MA_ZERO_OBJECT(&recording_device_);
		started_ = false;
	}
	std::string get_data(size_t& out_size_bytes) {
		const std::vector<float>* source_buf = record_engine_output_ ? &g_OutputDataBuffer : &recording_data_buffer_;
		if (!source_buf || source_buf->empty()) { out_size_bytes = 0; return ""; }
		out_size_bytes = source_buf->size() * sizeof(float);
		std::string result_str(out_size_bytes, '\0');
		std::memcpy(&result_str[0], source_buf->data(), out_size_bytes);
		return result_str;
	}
	void clear() {
		if (record_engine_output_) g_OutputDataBuffer.clear();
		else recording_data_buffer_.clear();
	}
};

audio_recorder* faudio_recorder() { return new audio_recorder(); }
audio_recorder* get_output_audio_recorder() {
	audio_recorder* recorder = new audio_recorder();
	recorder->record_engine_output_ = true;
	return recorder;
}
sound* fsound(const string& filename) { return new sound(filename); }
pcm_ring_buffer* fbuffer(ma_uint32 channels, ma_uint32 sample_rate, ma_uint32 buffer_size) {
	return new pcm_ring_buffer(channels, sample_rate, buffer_size);
}

void register_sound(asIScriptEngine* engine) {
	engine->RegisterGlobalFunction("void set_sound_storage(const string &in folder_name)property", asFUNCTION(set_sound_storage), asCALL_CDECL);
	engine->RegisterGlobalFunction("string get_sound_storage()property", asFUNCTION(get_sound_storage), asCALL_CDECL);

	// pcm_ring_buffer
	engine->RegisterObjectType("pcm_ring_buffer", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("pcm_ring_buffer", asBEHAVE_FACTORY, "pcm_ring_buffer@ f(uint32 channels = 0, uint32 sample_rate = 0, uint32 size = 1024)", asFUNCTION(fbuffer), asCALL_CDECL); // Default args might need helper
	engine->RegisterObjectBehaviour("pcm_ring_buffer", asBEHAVE_ADDREF, "void f() const", asMETHOD(pcm_ring_buffer, add_ref), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("pcm_ring_buffer", asBEHAVE_RELEASE, "void f() const", asMETHOD(pcm_ring_buffer, release), asCALL_THISCALL);
	engine->RegisterObjectMethod("pcm_ring_buffer", "void write(const string &in data)", asMETHOD(pcm_ring_buffer, write), asCALL_THISCALL);
	engine->RegisterObjectMethod("pcm_ring_buffer", "string read(uint64 size_bytes)", asMETHODPR(pcm_ring_buffer, read, (size_t), std::string), asCALL_THISCALL);
	engine->RegisterObjectMethod("pcm_ring_buffer", "void reset()", asMETHOD(pcm_ring_buffer, reset), asCALL_THISCALL);

	engine->RegisterObjectType("sound", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("sound", asBEHAVE_FACTORY, "sound@ f(const string &in filename = \"\")", asFUNCTION(fsound), asCALL_CDECL);
	engine->RegisterObjectBehaviour("sound", asBEHAVE_ADDREF, "void f() const", asMETHOD(sound, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("sound", asBEHAVE_RELEASE, "void f() const", asMETHOD(sound, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool load(const string &in filename)", asMETHOD(sound, load), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool load_from_memory(const string&in memory, uint64 memory_size)", asMETHODPR(sound, load_from_memory, (const string&, size_t), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool load_pcm(const string&in memory, uint64 memory_size, int channels, int sample_rate, int bits_per_sample)", asMETHODPR(sound, load_pcm, (const string&, size_t, int, int, int), bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool load_pcm_buffer(pcm_ring_buffer@ buffer)", asMETHOD(sound, load_pcm_buffer), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "string get_file_path() const property", asMETHOD(sound, get_file_path), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_fade_time(float volume_beg_db, float volume_end_db, float time_ms)", asMETHOD(sound, set_fade_time), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool play()", asMETHOD(sound, play), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool play_looped()", asMETHOD(sound, play_looped), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool pause()", asMETHOD(sound, pause), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool play_wait()", asMETHOD(sound, play_wait), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool stop()", asMETHOD(sound, stop), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool close()", asMETHOD(sound, close), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_fx(const string &in effect_name)", asMETHOD(sound, set_fx), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void delete_fx(const string &in effect_name)", asMETHOD(sound, delete_fx), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_reverb_parameters(float dry, float wet, float room_size, float damping, float mode)", asMETHOD(sound, set_reverb_parameters), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_position_3d(float x, float y, float z)", asMETHOD(sound, set_position_3d), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_hrtf(bool enable_hrtf = true) property", asMETHOD(sound, set_hrtf), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_hrtf() const property", asMETHOD(sound, get_hrtf), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool seek(float pos_ms)", asMETHOD(sound, seek), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_looping() const property", asMETHOD(sound, get_looping), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_looping(bool looping) property", asMETHOD(sound, set_looping), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_pan() const property", asMETHOD(sound, get_pan), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_pan(float pan_linear) property", asMETHOD(sound, set_pan), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_volume() const property", asMETHOD(sound, get_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_volume(float volume_db) property", asMETHOD(sound, set_volume), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_pitch() const property", asMETHOD(sound, get_pitch), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_pitch(float pitch_multiplier) property", asMETHOD(sound, set_pitch), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_active() const property", asMETHOD(sound, is_active), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_playing() const property", asMETHOD(sound, is_playing), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_paused() const property", asMETHOD(sound, is_paused), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "bool get_at_end() const property", asMETHOD(sound, at_end), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_position() const property", asMETHOD(sound, get_position), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_length() const property", asMETHOD(sound, get_length), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "void set_length(float length_ms = 0.0) property", asMETHOD(sound, set_length), asCALL_THISCALL);
	engine->RegisterObjectMethod("sound", "float get_sample_rate() const property", asMETHOD(sound, get_sample_rate), asCALL_THISCALL);

	engine->RegisterObjectType("audio_recorder", 0, asOBJ_REF);
	engine->RegisterObjectBehaviour("audio_recorder", asBEHAVE_FACTORY, "audio_recorder@ f()", asFUNCTION(faudio_recorder), asCALL_CDECL);
	engine->RegisterObjectBehaviour("audio_recorder", asBEHAVE_ADDREF, "void f() const", asMETHOD(audio_recorder, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("audio_recorder", asBEHAVE_RELEASE, "void f() const", asMETHOD(audio_recorder, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod("audio_recorder", "void start()", asMETHOD(audio_recorder, start), asCALL_THISCALL);
	engine->RegisterObjectMethod("audio_recorder", "void stop()", asMETHOD(audio_recorder, stop), asCALL_THISCALL);
	engine->RegisterObjectMethod("audio_recorder", "string get_data(uint64 &out size_bytes)", asMETHODPR(audio_recorder, get_data, (size_t&), std::string), asCALL_THISCALL);
	engine->RegisterObjectMethod("audio_recorder", "void clear()", asMETHOD(audio_recorder, clear), asCALL_THISCALL);
	engine->RegisterGlobalFunction("audio_recorder@ get_output_audio_recorder() property", asFUNCTION(get_output_audio_recorder), asCALL_CDECL);

	engine->RegisterGlobalFunction("void set_sound_global_hrtf(bool enable) property", asFUNCTION(set_sound_global_hrtf), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool get_sound_global_hrtf() property", asFUNCTION(get_sound_global_hrtf), asCALL_CDECL);
	engine->RegisterGlobalFunction("void set_spatial_blend_max_distance(float distance) property", asFUNCTION(set_spatial_blend_max_distance), asCALL_CDECL);
	engine->RegisterGlobalFunction("float get_spatial_blend_max_distance() property", asFUNCTION(get_spatial_blend_max_distance), asCALL_CDECL);

	engine->RegisterGlobalProperty("const uint default_channels", (void*)&DEFAULT_CHANNELS);
	engine->RegisterGlobalProperty("const uint default_samplerate", (void*)&DEFAULT_SAMPLE_RATE);
}
