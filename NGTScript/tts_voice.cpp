#include "angelscript.h"
#include"ngt.h"
#include "SRAL.h"
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
	if (!SRAL_Initialized())SRAL_Initialize("", 0);
}

TTSVoice::~TTSVoice()
{
}

bool TTSVoice::speak(const std::string& text)
{
	return SRAL_SpeakExtended(ENGINE_SAPI, text.c_str(), false);
}

bool TTSVoice::speak_wait(const std::string& text)
{
	return SRAL_SpeakExtended(ENGINE_SAPI, text.c_str(), false);
}

bool TTSVoice::speak_interrupt(const std::string& text)
{
	return SRAL_SpeakExtended(ENGINE_SAPI, text.c_str(), true);
}

bool TTSVoice::speak_interrupt_wait(const std::string& text)
{
	return SRAL_SpeakExtended(ENGINE_SAPI, text.c_str(), true);
}

std::vector<std::string> TTSVoice::get_voice_names()
{
	return {};
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
}

int TTSVoice::get_rate() const
{
	return 0;
}

void TTSVoice::set_rate(int new_rate)
{
}

int TTSVoice::get_volume() const
{
	return 0;
}

void TTSVoice::set_volume(int new_volume)
{
}
