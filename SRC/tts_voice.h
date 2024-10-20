#pragma once
#include "scriptarray/scriptarray.h"
#include <string>
#include <vector>
class TTSVoice
{
public:
	void add_ref();
	void release();
	TTSVoice();
	~TTSVoice();
	bool speak(const std::string& text);
	bool speak_wait(const std::string& text);
	bool speak_interrupt(const std::string& text);
	bool speak_interrupt_wait(const std::string& text);
	std::string speak_to_memory(const std::string& text, uint64_t& size);
	std::vector<std::string> get_voice_names();
	CScriptArray* get_voice_names_script();
	void set_voice(uint64_t voice_index);
	int get_rate() const;
	void set_rate(int rate);
	int get_volume() const;
	void set_volume(int volume);

private:
	mutable int ref;
	int engine;
};
