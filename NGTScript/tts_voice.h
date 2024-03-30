#pragma once
#include"sapi/SpeechEngine.h"
#include<string>
class tts_voice
{
public:
	tts_voice();
	~tts_voice();

	void construct();
	void destruct();
	void speak(const std::string& text);
	void speak_wait(const std::string& text);
	void speak_interrupt(const std::string& text);
	void speak_interrupt_wait(const std::string& text);
	int get_rate() const;
	void set_rate(int rate);
private:
	SpeechEngine* speaker=nullptr;
};