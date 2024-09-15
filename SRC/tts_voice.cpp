#include "angelscript.h"
#include"ngt.h"
#define SRAL_STATIC
#include "../SRAL/Include/SRAL.h"
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
	if (!SRAL_IsInitialized())SRAL_Initialize(0);
}

TTSVoice::~TTSVoice()
{
}

bool TTSVoice::speak(const std::string& text)
{
	return SRAL_SpeakEx(ENGINE_SAPI, text.c_str(), false);
}

bool TTSVoice::speak_wait(const std::string& text)
{
	return SRAL_SpeakEx(ENGINE_SAPI, text.c_str(), false);
}

bool TTSVoice::speak_interrupt(const std::string& text)
{
	return SRAL_SpeakEx(ENGINE_SAPI, text.c_str(), true);
}

bool TTSVoice::speak_interrupt_wait(const std::string& text)
{
	return SRAL_SpeakEx(ENGINE_SAPI, text.c_str(), true);
}

std::vector<std::string> TTSVoice::get_voice_names()
{
	std::vector<std::string> names;
	uint64_t count = SRAL_GetVoiceCountEx(ENGINE_SAPI);
	if (count == 0)return{};
	for (uint64_t i = 0; i < count; ++i) {
		const char* name = SRAL_GetVoiceNameEx(ENGINE_SAPI, i);
		names.push_back(std::string(name));
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
	SRAL_SetVoiceEx(ENGINE_SAPI, voice_index);
}

int TTSVoice::get_rate() const
{
	return SRAL_GetRateEx(ENGINE_SAPI);
}

void TTSVoice::set_rate(int new_rate)
{
	SRAL_SetRateEx(ENGINE_SAPI, new_rate);
}

int TTSVoice::get_volume() const
{
	return SRAL_GetVolumeEx(ENGINE_SAPI);;
}

void TTSVoice::set_volume(int new_volume)
{
	SRAL_SetVolumeEx(ENGINE_SAPI, new_volume);
}
