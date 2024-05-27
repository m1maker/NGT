#include "Tolk.h"
#include "dlibrary.h"
dlibrary tolk;
bool tolk_library_load(const std::string& name) {
    return tolk.load(name);
}
bool tolk_library_unload() {
    return tolk.unload();
}

void Tolk_Load() {
    if (!tolk.get_active()) return;
    typedef void(*Tolk_Load_Proc)();
    Tolk_Load_Proc Tolk_Load_Function = static_cast<Tolk_Load_Proc>(tolk.get("Tolk_Load"));
    Tolk_Load_Function();
}

bool Tolk_IsLoaded() {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_IsLoaded_Proc)();
    Tolk_IsLoaded_Proc Tolk_IsLoaded_Function = static_cast<Tolk_IsLoaded_Proc>(tolk.get("Tolk_IsLoaded"));
    return Tolk_IsLoaded_Function();
}

void Tolk_Unload() {
    if (!tolk.get_active()) return;
    typedef void(*Tolk_Unload_Proc)();
    Tolk_Unload_Proc Tolk_Unload_Function = static_cast<Tolk_Unload_Proc>(tolk.get("Tolk_Unload"));
    Tolk_Unload_Function();
}

void Tolk_TrySAPI(bool trySAPI) {
    if (!tolk.get_active()) return;
    typedef void(*Tolk_TrySAPI_Proc)(bool);
    Tolk_TrySAPI_Proc Tolk_TrySAPI_Function = static_cast<Tolk_TrySAPI_Proc>(tolk.get("Tolk_TrySAPI"));
    Tolk_TrySAPI_Function(trySAPI);
}

void Tolk_PreferSAPI(bool preferSAPI) {
    if (!tolk.get_active()) return;
    typedef void(*Tolk_PreferSAPI_Proc)(bool);
    Tolk_PreferSAPI_Proc Tolk_PreferSAPI_Function = static_cast<Tolk_PreferSAPI_Proc>(tolk.get("Tolk_PreferSAPI"));
    Tolk_PreferSAPI_Function(preferSAPI);
}

const wchar_t* Tolk_DetectScreenReader() {
    if (!tolk.get_active()) return L"";
    typedef const wchar_t* (*Tolk_DetectScreenReader_Proc)();
    Tolk_DetectScreenReader_Proc Tolk_DetectScreenReader_Function = static_cast<Tolk_DetectScreenReader_Proc>(tolk.get("Tolk_DetectScreenReader"));
    return Tolk_DetectScreenReader_Function();
}

bool Tolk_HasSpeech() {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_HasSpeech_Proc)();
    Tolk_HasSpeech_Proc Tolk_HasSpeech_Function = static_cast<Tolk_HasSpeech_Proc>(tolk.get("Tolk_HasSpeech"));
    return Tolk_HasSpeech_Function();
}

bool Tolk_HasBraille() {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_HasBraille_Proc)();
    Tolk_HasBraille_Proc Tolk_HasBraille_Function = static_cast<Tolk_HasBraille_Proc>(tolk.get("Tolk_HasBraille"));
    return Tolk_HasBraille_Function();
}

bool Tolk_Output(const wchar_t* str, bool interrupt) {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_Output_Proc)(const wchar_t*, bool);
    Tolk_Output_Proc Tolk_Output_Function = static_cast<Tolk_Output_Proc>(tolk.get("Tolk_Output"));
    return Tolk_Output_Function(str, interrupt);
}
bool Tolk_Speak(const wchar_t* str, bool interrupt) {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_Speak_Proc)(const wchar_t*, bool);
    Tolk_Speak_Proc Tolk_Speak_Function = static_cast<Tolk_Speak_Proc>(tolk.get("Tolk_Speak"));
    return Tolk_Speak_Function(str, interrupt);
}
bool Tolk_Braille(const wchar_t* str) {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_Braille_Proc)(const wchar_t*);
    Tolk_Braille_Proc Tolk_Braille_Function = static_cast<Tolk_Braille_Proc>(tolk.get("Tolk_Braille"));
    return Tolk_Braille_Function(str);
}

bool Tolk_IsSpeaking() {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_IsSpeaking_Proc)();
    Tolk_IsSpeaking_Proc Tolk_IsSpeaking_Function = static_cast<Tolk_IsSpeaking_Proc>(tolk.get("Tolk_IsSpeaking"));
    return Tolk_IsSpeaking_Function();
}

bool Tolk_Silence() {
    if (!tolk.get_active()) return false;
    typedef bool(*Tolk_Silence_Proc)();
    Tolk_Silence_Proc Tolk_Silence_Function = static_cast<Tolk_Silence_Proc>(tolk.get("Tolk_Silence"));
    return Tolk_Silence_Function();
}

