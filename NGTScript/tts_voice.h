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
	void speak(const std::string&);
	void speak_wait(const std::string&);
	void speak_interrupt(const std::string&);
	void speak_interrupt_wait(const std::string&);
	int get_rate() const;
	void set_rate(int);
private:
	SpeechEngine* speaker=nullptr;
};