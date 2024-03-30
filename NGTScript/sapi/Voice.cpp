#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 

#include "Voice.h"

Voice::Voice(wstring const age, wstring const gender, wstring const languageCode, wstring const name, wstring const sharedPronunciation,
             wstring  const vendor, wstring const version, ISpObjectToken *voiceToken) {
        this->age = age;
        this->gender = gender;
        this->languageCode = languageCode;
        this->name = name;
        this->sharedPronunciation = sharedPronunciation;
        this->vendor = vendor;
        this->version = version;
        this->voiceToken = voiceToken;
}

wstring Voice::getAge() const {
    return age;
}

wstring Voice::getGender() const {
    return gender;
}

wstring Voice::getLanguageCode() const{
    return languageCode;
}

wstring Voice::getName() const {
    return name;
}

wstring Voice::getSharedPronunciation() const {
    return sharedPronunciation;
}

wstring Voice::getVendor() const {
    return vendor;
}

wstring Voice::getVersion() const {
    return version;
}

ISpObjectToken *Voice::getVoiceToken() const {
    return voiceToken;
}