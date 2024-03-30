#ifndef SAPI_WRAPPER_VOICE_H
#define SAPI_WRAPPER_VOICE_H

#include <sapi.h>
#include <string>

using std::wstring;

class Voice {
private:
        wstring age;
        wstring gender;
        wstring languageCode;
        wstring name;
        wstring sharedPronunciation;
        wstring vendor;
        wstring version;
        ISpObjectToken * voiceToken;
    public:
        Voice(wstring age, wstring gender, wstring languageCode, wstring name,
            wstring sharedPronunciation, wstring vendor, wstring version, ISpObjectToken * voiceToken);
    wstring getAge() const;
    wstring getGender() const;
    wstring getLanguageCode() const;
    wstring getName() const;
    wstring getSharedPronunciation() const;
    wstring getVendor() const;
    wstring getVersion() const;
    ISpObjectToken * getVoiceToken() const;
};


#endif //SAPI_WRAPPER_VOICE_H
