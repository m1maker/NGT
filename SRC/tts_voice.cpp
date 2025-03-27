#include"ngt.h"
#include <angelscript.h>
#define SRAL_STATIC
#include <SRAL.h>
#include "tts_voice.h"
void TTSVoice::add_ref() {
	asAtomicInc(ref);
}
void TTSVoice::release() {
	if (asAtomicDec(ref) < 1)
		delete this;
}
TTSVoice::TTSVoice()
{
	ref = 1;
	if (!SRAL_IsInitialized()) {
		SRAL_Initialize(0);
#ifdef _WIN32
		engine = ENGINE_SAPI;
		SRAL_RegisterKeyboardHooks(); // Linux is blocking all window events when keyboard hooks are installed
#else
		engine = ENGINE_SPEECH_DISPATCHER;
#endif
	}
}
TTSVoice::~TTSVoice()
{
}

bool TTSVoice::speak(const std::string& text)
{
	return SRAL_SpeakEx(engine, text.c_str(), false);
}

bool TTSVoice::speak_wait(const std::string& text)
{
	return SRAL_SpeakEx(engine, text.c_str(), false);
}

bool TTSVoice::speak_interrupt(const std::string& text)
{
	return SRAL_SpeakEx(engine, text.c_str(), true);
}

bool TTSVoice::speak_interrupt_wait(const std::string& text)
{
	return SRAL_SpeakEx(engine, text.c_str(), true);
}

std::string TTSVoice::speak_to_memory(const std::string& text, uint64_t& size, int& channels, int& sample_rate, int& bits_per_sample) {
	char* pcm = (char*)SRAL_SpeakToMemoryEx(engine, text.c_str(), &size, &channels, &sample_rate, &bits_per_sample);
	return std::string(pcm, size);
}

std::vector<std::string> TTSVoice::get_voice_names()
{
	std::vector<std::string> names;
	int voice_count;
	SRAL_GetEngineParameter(engine, VOICE_COUNT, &voice_count);
	if (voice_count == 0) return{};
	char* voices[256];
	for (unsigned int i = 0; i < voice_count; ++i) {
		voices[i] = (char*)malloc(64);
	}

	SRAL_GetEngineParameter(engine, VOICE_LIST, voices);
	names.resize(voice_count);
	for (unsigned int i = 0; i < voice_count; ++i) {
		names[i] = std::string(voices[i]);
		free(voices[i]);
	}
	return names;
}

CScriptArray* TTSVoice::get_voice_names_script() {
	asIScriptContext* ctx = asGetActiveContext();
	asIScriptEngine* engine = ctx->GetEngine();
	asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<string>"));
	CScriptArray* array = CScriptArray::Create(arrayType, (asUINT)0);
	std::vector<std::string> voice_names = this->get_voice_names();
	if (voice_names.size() == 0)return array;
	for (uint64_t i = 0; i < voice_names.size(); i++)
	{
		array->Resize(array->GetSize() + 1);
		((string*)array->At(i))->assign(voice_names[i]);
	}
	return array;

}
void TTSVoice::set_voice(uint64_t voice_index)
{
	SRAL_SetEngineParameter(engine, VOICE_INDEX, (int*)&voice_index);
}

int TTSVoice::get_rate() const
{
	int rate;
	SRAL_GetEngineParameter(engine, SPEECH_RATE, &rate);
	return rate;
}

void TTSVoice::set_rate(int new_rate)
{
	SRAL_SetEngineParameter(engine, SPEECH_RATE, (int*)&new_rate);
}

int TTSVoice::get_volume() const
{
	int volume;
	SRAL_GetEngineParameter(engine, SPEECH_VOLUME, &volume);
	return volume;
}

void TTSVoice::set_volume(int new_volume)
{
	SRAL_SetEngineParameter(engine, SPEECH_VOLUME, (int*)&new_volume);
}
