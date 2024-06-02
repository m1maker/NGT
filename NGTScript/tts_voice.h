#ifndef TTS_VOICE_H
#define TTS_VOICE_H
#pragma once
#include "as_class.h"
#include"sapi/SpeechEngine.h"
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
	SpeechEngine* speaker = nullptr;
};
#endif