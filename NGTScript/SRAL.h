#ifndef SRAL_H_
#define SRAL_H_
#pragma once
#ifdef _WIN32
#if defined SRAL_EXPORT
#define SRAL_API __declspec(dllexport)
#elif defined (SRAL_STATIC)
#define SRAL_API
#else
#define SRAL_API __declspec(dllimport)
#endif
#else
#define SRAL_API
#endif
#ifdef __cplusplus
extern "C" {
#include <stdbool.h>
#endif
#include <stdint.h>
#include <stdlib.h>
	enum SRAL_Engines {
		ENGINE_NONE = 0,
		ENGINE_NVDA = 2,
		ENGINE_SAPI = 4,
		ENGINE_JAWS = 8,
		ENGINE_SPEECH_DISPATCHER = 16,
		ENGINE_UIA = 32
	};
	enum SRAL_SupportedFeatures {
		SUPPORTS_SPEECH = 128,
		SUPPORTS_BRAILLE = 256,
		SUPPORTS_SPEECH_RATE = 512,
		SUPPORTS_SPEECH_VOLUME = 1024
	};


	SRAL_API bool SRAL_Speak(const char* text, bool interrupt);
	SRAL_API bool SRAL_Braille(const char* text);
	SRAL_API bool SRAL_Output(const char* text, bool interrupt);
	SRAL_API bool SRAL_StopSpeech(void);
	SRAL_API int SRAL_GetCurrentEngine(void);
	SRAL_API int SRAL_GetEngineFeatures(int engine = 0);
	SRAL_API bool SRAL_Initialize(const char* library_path, int engines_exclude = 0);
	SRAL_API void SRAL_Uninitialize(void);
	SRAL_API bool SRAL_SetVolume(uint64_t value);
	SRAL_API uint64_t SRAL_GetVolume(void);
	SRAL_API bool SRAL_SetRate(uint64_t value);
	SRAL_API uint64_t SRAL_GetRate(void);


	SRAL_API bool SRAL_SpeakExtended(int engine, const char* text, bool interrupt);
	SRAL_API bool SRAL_BrailleExtended(int engine, const char* text);
	SRAL_API bool SRAL_OutputExtended(int engine, const char* text, bool interrupt);
	SRAL_API bool SRAL_StopSpeechExtended(int engine);



	SRAL_API bool SRAL_Initialized(void);


#ifdef __cplusplus
}// extern "C"
#endif


#endif // SRAL_H_