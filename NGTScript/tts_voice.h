#ifndef TTS_VOICE_H
#define TTS_VOICE_H
#pragma once
#include "as_class.h"
#ifdef _WIN32
#include"sapi/SpeechEngine.h"
#endif
#include<string>
class tts_voice : public as_class
{
public:
	tts_voice();
	~tts_voice();

	void speak(const std::string&);
	void speak_wait(const std::string&);
	void speak_interrupt(const std::string&);
	void speak_interrupt_wait(const std::string&);
	int get_rate() const;
	void set_rate(int);
private:
#ifdef _WIN32
	SpeechEngine* speaker = nullptr;
#endif
};
#endif