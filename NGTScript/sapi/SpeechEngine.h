#ifndef SAPI_WRAPPER_SPEECHENGINE_H
#define SAPI_WRAPPER_SPEECHENGINE_H

#include <sapi.h>
#include <string>
#include <codecvt>
#include "Voice.h"
#include <vector>

class SpeechEngine {
private:
        ISpVoice * spVoice;
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
public:
    SpeechEngine();
    ~SpeechEngine();
    void speak(std::string const &text);
    void speak(std::wstring const &text);
    void speak_wait(std::string const& text);
    void speak_wait(std::wstring const& text);
    void speak_interrupt(std::string const &text);
    void speak_interrupt(std::wstring const &text);
    void speak_interrupt_wait(std::string const &text);
    void speak_interrupt_wait(std::wstring const &text);
    bool is_speaking() const;
    void setRate(long rate);
    long getRate() const ;
    void setVolume(unsigned short volume);
    unsigned short getVolume() const;
    std::vector<Voice> getVoices() const;
    void setVoice(Voice voice);
};

#endif //SAPI_WRAPPER_SPEECHENGINE_H
