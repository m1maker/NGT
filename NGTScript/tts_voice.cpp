#include "angelscript.h"
#include"ngt.h"
#include "tts_voice.h"
#include<atlbase.h>
#include<atlcom.h>
#include <sapi.h>
#include <sphelper.h>
#include <stdexcept>
void TTSVoice::add_ref() {
	asAtomicInc(ref);
}
void TTSVoice::release() {
	if (asAtomicDec(ref) < 1)
		delete this;
}
TTSVoice::TTSVoice() : rate(0), volume(100)
{
	ref = 1;
	if (g_COMInitialized == true)return;
	if (FAILED(::CoInitialize(nullptr)))
	{
		alert("COM Init Error", "Could not initialize component object model");
	}
	initialize();
}

TTSVoice::~TTSVoice()
{
	::CoUninitialize();
	g_COMInitialized = false;
}

void TTSVoice::initialize()
{
	if (FAILED(pVoice.CoCreateInstance(CLSID_SpVoice)))
	{
		alert("Speech API Error", "Could not create SAPI Voice");
	}

	CComPtr<IEnumSpObjectTokens> cpEnum;
	if (SUCCEEDED(SpEnumTokens(SPCAT_VOICES, nullptr, nullptr, &cpEnum)))
	{
		CComPtr<ISpObjectToken> pToken;
		while (cpEnum->Next(1, &pToken, nullptr) == S_OK)
		{
			voices.push_back(pToken);
			pToken.Release();
		}
	}
	g_COMInitialized = true;
}

bool TTSVoice::speak(const std::string& text)
{
	return SUCCEEDED(pVoice->Speak(CComBSTR(text.c_str()), SPF_ASYNC, nullptr));
}

bool TTSVoice::speak_wait(const std::string& text)
{
	return SUCCEEDED(pVoice->Speak(CComBSTR(text.c_str()), SPF_DEFAULT, nullptr));
}

bool TTSVoice::speak_interrupt(const std::string& text)
{
	return SUCCEEDED(pVoice->Speak(CComBSTR(text.c_str()), SPF_ASYNC | SPF_PURGEBEFORESPEAK, nullptr));
}

bool TTSVoice::speak_interrupt_wait(const std::string& text)
{
	return SUCCEEDED(pVoice->Speak(CComBSTR(text.c_str()), SPF_PURGEBEFORESPEAK, nullptr));
}

std::vector<std::string> TTSVoice::get_voice_names()
{
	std::vector<std::string> voice_names;
	for (auto& voice : voices)
	{
		CSpDynamicString dstrDesc;
		if (SUCCEEDED(SpGetDescription(voice, &dstrDesc)))
		{
			voice_names.push_back(std::string(CW2A(dstrDesc)));
		}
	}
	return voice_names;
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
	if (voice_index < voices.size())
	{
		pVoice->SetVoice(voices[voice_index]);
	}
}

int TTSVoice::get_rate() const
{
	return rate;
}

void TTSVoice::set_rate(int new_rate)
{
	rate = new_rate;
	pVoice->SetRate(rate);
}

int TTSVoice::get_volume() const
{
	return volume;
}

void TTSVoice::set_volume(int new_volume)
{
	volume = new_volume;
	pVoice->SetVolume(volume);
}
