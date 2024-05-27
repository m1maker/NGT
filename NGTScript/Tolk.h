#ifndef _TOLK_H_
#define _TOLK_H_
#include <string>
bool tolk_library_load(const std::string& name);
bool tolk_library_unload();


 void Tolk_Load();

 bool Tolk_IsLoaded();

 void Tolk_Unload();

 void Tolk_TrySAPI(bool trySAPI);

 void Tolk_PreferSAPI(bool preferSAPI);

 const wchar_t * Tolk_DetectScreenReader();

 bool Tolk_HasSpeech();

 bool Tolk_HasBraille();

 bool Tolk_Output(const wchar_t *str, bool interrupt = false);
 bool Tolk_Speak(const wchar_t *str, bool interrupt = false);
 bool Tolk_Braille(const wchar_t *str);

 bool Tolk_IsSpeaking();

 bool Tolk_Silence();

#endif