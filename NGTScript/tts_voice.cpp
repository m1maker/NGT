#include"sapi/SpeechEngine.h"
#include<string>
#include"tts_voice.h"
tts_voice::tts_voice()
{
	speaker = new SpeechEngine();
}
tts_voice::~tts_voice()
{
	delete speaker;
	speaker = nullptr;
}
void tts_voice::construct()
{

}
void tts_voice::destruct()
{
}
void tts_voice::speak(const std::string& text)
{
	speaker->speak(text);
}
void tts_voice::speak_wait(const std::string& text)
{
	speaker->speak_wait(text);
}
void tts_voice::speak_interrupt(const std::string& text)
{
	speaker->speak_interrupt(text);
}
void tts_voice::speak_interrupt_wait(const std::string& text)
{
	speaker->speak_interrupt_wait(text);
}
int tts_voice::get_rate() const
{
return speaker->getRate();
}
void tts_voice::set_rate(int rate)
{
speaker->setRate(rate);
}