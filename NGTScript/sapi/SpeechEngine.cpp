#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 

#include "SpeechEngine.h"
#include <iostream>
#include <sphelper.h>

void SpeechEngine::speak(std::string const &text) {
    speak(converter.from_bytes(text));
}

void SpeechEngine::speak(std::wstring const &text) {
    spVoice->Speak(text.c_str(), SVSFlagsAsync, nullptr);
}

void SpeechEngine::speak_wait(std::string const& text) {
    speak_wait(converter.from_bytes(text));
}

void SpeechEngine::speak_wait(std::wstring const& text) {
    spVoice->Speak(text.c_str(),SVSFDefault,nullptr);
}

void SpeechEngine::speak_interrupt(std::string const &text) {
    speak_interrupt(converter.from_bytes(text));
}

void SpeechEngine::speak_interrupt(std::wstring const &text) {
    spVoice->Speak(text.c_str(), SVSFlagsAsync|SVSFPurgeBeforeSpeak, nullptr);
}

void SpeechEngine::speak_interrupt_wait(std::string const &text) {
    speak_interrupt_wait(converter.from_bytes(text));
}

void SpeechEngine::speak_interrupt_wait(std::wstring const &text) {
    spVoice->Speak(text.c_str(), SVSFPurgeBeforeSpeak, nullptr);
}

bool SpeechEngine::is_speaking() const {
    SPVOICESTATUS status;
    spVoice->GetStatus(&status, nullptr);
    return status.dwRunningState == SPRS_IS_SPEAKING;
}

void SpeechEngine::setRate(long rate) {
    spVoice->SetRate(rate);
}

long SpeechEngine::getRate() const {
    long *rate = nullptr;
    spVoice->GetRate(rate);
    return *rate;
}

void SpeechEngine::setVolume(unsigned short volume) {
    spVoice->SetVolume(volume);
}

unsigned short SpeechEngine::getVolume() const {
    unsigned short *volume = nullptr;
    spVoice->GetVolume(volume);
    return *volume;
}


SpeechEngine::SpeechEngine() {
    // TODO Exception handling when Initialize fails
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    CoCreateInstance(CLSID_SpVoice, nullptr, CLSCTX_ALL, IID_ISpVoice, (void **) &spVoice);
}

SpeechEngine::~SpeechEngine() {
    CoUninitialize();
}

std::vector<Voice> SpeechEngine::getVoices() const {
    std::vector<Voice> vector;
    ISpObjectTokenCategory *spCategory = nullptr;
    SpGetCategoryFromId(SPCAT_VOICES, &spCategory, FALSE);
    IEnumSpObjectTokens *spEnumTokens = nullptr;
    spCategory->EnumTokens(nullptr, nullptr, &spEnumTokens);
    ISpObjectToken *objectToken;
    while (S_OK == spEnumTokens->Next(1, &objectToken, NULL)) {
        ISpDataKey *regToken;
        objectToken->OpenKey(L"Attributes", &regToken);
        LPWSTR age;
        regToken->GetStringValue(L"Age", &age);
        LPWSTR gender;
        regToken->GetStringValue(L"Gender", &gender);
        LPWSTR languageCode;
        regToken->GetStringValue(L"Language", &languageCode);
        LPWSTR name;
        regToken->GetStringValue(L"Name", &name);
        LPWSTR sharedPronunciation;
        regToken->GetStringValue(L"SharedPronunciation", &sharedPronunciation);
        LPWSTR vendor;
        regToken->GetStringValue(L"Vendor", &vendor);
        LPWSTR version;
        regToken->GetStringValue(L"Version", &version);
        vector.emplace_back(Voice(age, gender, languageCode, name, sharedPronunciation, vendor, version, objectToken));
        objectToken->Release();
    }
    return vector;
}

void SpeechEngine::setVoice(Voice voice) {
    spVoice->SetVoice(voice.getVoiceToken());
}
