#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include <random>
#include "sound.h"
#include "ngtreg.h"
#include <filesystem>
#include<vector>
#include <thread>
#include "Tolk.h"
#include <chrono>
#include <string>
#include"sdl/SDL.h"
#define BL_NUMWORDS_IMPLEMENTATION
extern "C"
{
#include"bl_number_to_words.h"
}
#include<map>
#include "../uni_algo.h"
#include<algorithm>
#include <stdio.h>
#include "NGT.H"
#include "scriptfile/scriptfilesystem.h"
#include <locale>
#include <codecvt>
#include<fstream>
#include <cstdlib>
#include<algorithm>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
asIScriptFunction* exit_callback = nullptr;
tts_voice* voice_object = nullptr;
/**
 * Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
 * Fills in the encryption and decryption ctx objects and returns 0 on success
 **/
int aes_init(unsigned char* key_data, int key_data_len, unsigned char* salt, EVP_CIPHER_CTX* e_ctx,
    EVP_CIPHER_CTX* d_ctx)
{
    int i, nrounds = 5;
    unsigned char key[32], iv[32];

    /*
     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
     * nrounds is the number of times the we hash the material. More rounds are more secure but
     * slower.
     */
    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
    if (i != 32) {
        printf("Key size is %d bits - should be 256 bits\n", i);
        return -1;
    }

    EVP_CIPHER_CTX_init(e_ctx);
    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_CIPHER_CTX_init(d_ctx);
    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

    return 0;
}

/*
 * Encrypt *len bytes of data
 * All data going in & out is considered binary (unsigned char[])
 */
unsigned char* aes_encrypt(EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len)
{
    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
    int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
    unsigned char* ciphertext = (unsigned char*)malloc(c_len);

    /* allows reusing of 'e' for multiple encryption cycles */
    EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

    /* update ciphertext, c_len is filled with the length of ciphertext generated,
      *len is the size of plaintext in bytes */
    EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

    /* update ciphertext with the final remaining bytes */
    EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

    *len = c_len + f_len;
    return ciphertext;
}

/*
 * Decrypt *len bytes of ciphertext
 */
unsigned char* aes_decrypt(EVP_CIPHER_CTX* e, unsigned char* ciphertext, int* len)
{
    /* plaintext will always be equal to or lesser than length of ciphertext*/
    int p_len = *len, f_len = 0;
    unsigned char* plaintext = (unsigned char*)malloc(p_len);

    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
    EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

    *len = p_len + f_len;
    return plaintext;
}


using namespace std;
SDL_Window* win = NULL;

SDL_Event e;
bool window_is_focused;
wstring wstr(const string& utf8String)
{
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
}
wstring reader;
unordered_map<SDL_Scancode, bool> keys;
bool keyhook = false;
string inputtext;
void replace(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}


void as_printf(asIScriptGeneric* gen)
{
    void* ref = gen->GetArgAddress(0);
    int typeId = gen->GetArgTypeId(0);

    string format = *static_cast<string*>(ref);

    for (int i = 1; i < 16; i++)
    {
        ref = gen->GetArgAddress(i);
        typeId = gen->GetArgTypeId(i);

        switch (typeId)
        {
        case 67108876: //string?
        {
            string local = *static_cast<string*>(ref);
            replace(format, "%s", local);
            break;
        }
        case 2:
        {
            char local = *static_cast<char*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 3:
        {
            short local = *static_cast<short*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 4:
        {
            int local = *static_cast<int*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 5:
        {
            long long local = *static_cast<long long*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 6:
        {
            unsigned char local = *static_cast<unsigned char*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 7:
        {
            unsigned short local = *static_cast<unsigned short*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 8:
        {
            unsigned int local = *static_cast<unsigned int*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 9:
        {
            unsigned long long local = *static_cast<unsigned long long*>(ref);
            replace(format, "%d", to_string(local));
            break;
        }
        case 10:
        {
            float local = *static_cast<float*>(ref);
            replace(format, "%f", to_string(local));
            break;
        }
        case 11:
        {
            double local = *static_cast<double*>(ref);
            replace(format, "%f", to_string(local));
            break;
        }
        }
    }

    cout << format << endl;
    return;
}
int get_cpu_count() {
    return SDL_GetCPUCount();
}
int get_system_ram() {
    return SDL_GetSystemRAM();
}
string get_platform() {
    return string(SDL_GetPlatform());
}
void set_exit_callback(asIScriptFunction* callback) {
    exit_callback = callback;
}

void init_engine(){

    SDL_Init(SDL_INIT_EVERYTHING);
    if (!tolk_library_load("Tolk.dll")) {
        voice_object = new tts_voice;
    }
    Tolk_Load();

    enet_initialize();
    Tolk_TrySAPI(true);
    reader=Tolk_DetectScreenReader();
    if (!Tolk_HasSpeech()) {
        Tolk_PreferSAPI(true);
}
}   
    void set_library_path(const string& path) {
    if (Tolk_IsLoaded())Tolk_Unload();
    tolk_library_unload();
    if (voice_object != nullptr) {
        delete voice_object;
        voice_object = nullptr;
    }
    filesystem::path current_dir = filesystem::current_path();
    filesystem::path new_dir= filesystem::current_path()/path;

    filesystem::current_path(new_dir);
    if (!tolk_library_load("Tolk.dll")) {
        voice_object = new tts_voice;
    }
    Tolk_Load();
    soundsystem_free();
    soundsystem_init();
    filesystem::current_path(current_dir);
}

random_device rd;
mt19937 gen(rd());
long random(long min, long max) {
    static_assert(is_integral<long>::value, "Type must be integral");

    uniform_int_distribution<long> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}
double randomDouble(double min, double max) {
    static_assert(is_floating_point<double>::value, "Type must be floating point");

    uniform_real_distribution<double> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}

bool random_bool() {
    return random(0, 1) == random(0, 1);
}
int get_last_error() {
    return 0;
}

void speak(const string & text, bool stop) {
    wstring textstr = wstr(text);
        Tolk_Speak(textstr.c_str(), stop);
        if (voice_object != nullptr) {
            if (stop)
                voice_object->speak_interrupt(text);
            else
                voice_object->speak(text);
        }
}
void speak_wait(const string & text, bool stop) {
    speak(text, stop);
    while (Tolk_IsSpeaking()) {
        SDL_PollEvent(&e);
    }
    if (voice_object != nullptr) {
        if (stop)
            voice_object->speak_interrupt_wait(text);
        else
            voice_object->speak_wait(text);
    }

}

void stop_speech() {
        Tolk_Silence();
        if (voice_object != nullptr) {
                voice_object->speak_interrupt(nullptr);
        }

}
string screen_reader_detect() {
    reader = Tolk_DetectScreenReader();
    return string(reader.begin(), reader.end());
}
bool window_closable;
    void show_console() {
    if (AllocConsole())
    {
        FILE* file = nullptr;
        freopen_s(&file, "CONOUT$", "w", stdout);
    }
}

void hide_console() {
    FreeConsole();
}

bool show_window(const string & title,int width, int height, bool closable)
{
    if (win != NULL)
        return false;
    if (reader == L"JAWS") {
        SDL_SetHint(SDL_HINT_ALLOW_ALT_TAB_WHILE_GRABBED, "1");
        win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_KEYBOARD_GRABBED);
        SDL_RegisterApp("NGTWindow", 0x450, GetModuleHandle(NULL));

    }
    else
        SDL_SetHint(SDL_HINT_APP_NAME, "NGTGame");
    SDL_RegisterApp("NGTWindow", 0x450, GetModuleHandle(NULL));

        win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);

SDL_StartTextInput();
window_closable = closable;
if (win!=NULL)
{

    update_window();
    window_is_focused = true;
    return true;
}
return false;
}
bool focus_window() {
    SDL_SetWindowInputFocus(win);
    return true;
}
void hide_window() {
    SDL_StopTextInput();

    SDL_DestroyWindow(win);
    window_is_focused = false;
}
void set_window_title(const string & new_title) {
    SDL_SetWindowTitle(win, new_title.c_str());
}
void set_window_closable(bool set_closable) {
    window_closable = set_closable;
}
void garbage_collect() {
    asIScriptContext* ctx = asGetActiveContext();
    asIScriptEngine* engine = ctx->GetEngine();
    engine->GarbageCollect(1|2, 1);
}
string key_to_string(SDL_Scancode key) {
    return string(SDL_GetScancodeName(key));
}
SDL_Scancode string_to_key(const string&key) {
    return SDL_GetScancodeFromName(key.c_str());
}
void update_window(bool wait_event)
{
    if (win != nullptr) {
if(!wait_event)
        SDL_PollEvent(&e);
else
SDL_WaitEvent(&e);
if (e.type == SDL_QUIT and window_closable == true)
        {
            exit_engine();
        }
        if (e.type == SDL_TEXTINPUT)
            inputtext += e.text.text;

        if (e.type == SDL_KEYDOWN)
        {
            keys[e.key.keysym.scancode] = true;
        }
        if (e.type == SDL_KEYUP)
        {
            auto it = keys.find(e.key.keysym.scancode);
            if (it != keys.end())
            {
                it->second = false;
            }
        }
        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event== SDL_WINDOWEVENT_FOCUS_GAINED)
                window_is_focused = true;
            if (e.window.event== SDL_WINDOWEVENT_FOCUS_LOST)
                window_is_focused = false;

}
    }
}
    bool get_window_active() {
    return window_is_focused;
}
    bool mouse_down(int button) {
        return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
    }

    bool mouse_pressed(int button) {
        static bool prev_state = false;
        bool current_state = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
        bool result = current_state && !prev_state;
        prev_state = current_state;
        return result;
    }

    bool mouse_update() {
        SDL_PumpEvents();
        return true;
    }

    int get_MOUSE_X() {
        int x;
        SDL_GetMouseState(&x, NULL);
        return x;
    }

    int get_MOUSE_Y() {
        int y;
        SDL_GetMouseState(NULL, &y);
        return y;
    }

    int get_MOUSE_Z() {
        int x, y;
        SDL_GetMouseState(NULL, NULL);
        return 0; // SDL does not provide direct support for mouse wheel in this function
    }


    void exit_engine(int return_number)
{
        if (exit_callback != nullptr) {
            asIScriptContext* ctx = asGetActiveContext();
            asIScriptEngine* engine = ctx->GetEngine();
            asIScriptContext* e_ctx = engine->CreateContext();
            e_ctx->Prepare(exit_callback);
            e_ctx->Execute();
            int result = e_ctx->GetReturnDWord();
            if (result == 1) {
                e_ctx->Release();
                return;
}
            e_ctx->Release();
        }
        soundsystem_free();
    SDL_StopTextInput();
    SDL_DestroyWindow(win);
win=NULL;
SDL_UnregisterApp();
enet_deinitialize();
Tolk_Unload();
tolk_library_unload();
if (voice_object != nullptr) {
    delete voice_object;
    voice_object = nullptr;
}
SDL_Quit();
exit(return_number);
}
string number_to_words(uint64_t num, bool include_and)
{
    vector<char> buf(1024);
    int use_and = (include_and ? 1 : 0);
    size_t bufsize = bl_number_to_words(num, buf.data(), buf.size(), use_and);
    if (bufsize > buf.size())
    {
        return "";
    }
    return string(buf.data());
}
string read_environment_variable(const string& path) {
#ifdef _WIN32
    // Use _dupenv_s on Windows
    char* value;
    size_t size;
    if (_dupenv_s(&value, &size, path.c_str()) != 0 || value == nullptr) {
        // Environment variable not found or error occurred
        return "";
    }
    const string & result(value);
    free(value);  // Free the allocated memory
    return result;
#else
    // Use getenv for non-Windows platforms
    char* value = getenv(path.c_str());
    if (value == nullptr) {
        // Environment variable not found
        return "";
    }
    return const string &(value);
#endif
}
bool clipboard_copy_text(const string & text) {
    SDL_SetClipboardText(text.c_str());
    return true;
}
string clipboard_read_text() {
    return SDL_GetClipboardText();
}
string get_input() {
    string temp=inputtext;
    inputtext="";
    return temp;
}
bool key_pressed(SDL_Scancode key_code)
{
    if (e.key.state == SDL_PRESSED)
    {
        if (e.key.keysym.scancode == key_code and e.key.repeat==0){
            return true;
    }
    }
return false;
}
bool key_released(SDL_Scancode key_code)
{
    if (e.key.state== SDL_RELEASED)
    {
if(e.key.keysym.scancode==key_code){
    return true;
}
    }
    return false;
}
bool key_down(SDL_Scancode key_code)
{
    if (keys.find(key_code) != keys.begin()or keys.find(key_code) != keys.end()) {
        return keys[key_code];
    }
return false;
}
bool key_repeat(SDL_Scancode key_code)
{
    if (e.type == SDL_KEYDOWN)
    {
if(e.key.keysym.scancode==key_code){
    return true;
}
    }
    return false;
}
bool force_key_down(SDL_Scancode keycode) {
    e.type = SDL_KEYDOWN;
    e.key.keysym.scancode = keycode;
        keys[e.key.keysym.scancode] = true;
    return SDL_PushEvent(&e);
}
bool force_key_up(SDL_Scancode keycode) {
    e.type = SDL_KEYUP;
    e.key.keysym.scancode = keycode;
    keys[e.key.keysym.scancode] = false;
    return SDL_PushEvent(&e);
}
void reset_all_forced_keys() {
    for (int i = 0; i < keys.size(); i++) {
        e.type = SDL_KEYUP;

        keys[e.key.keysym.scancode] = false;
        SDL_PushEvent(&e);

}
}
string string_encrypt(string the_string, string encryption_key) {
    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
       status of enc/dec operations */
    EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX* de = EVP_CIPHER_CTX_new();

    /* 8 bytes to salt the key_data during key generation. This is an example of
       compiled in salt. We just read the bit pattern created by these two 4 byte
       integers on the stack as 64 bits of contigous salt material -
       ofcourse this only works if sizeof(int) >= 4 */
    unsigned int salt[] = { 12345, 54321 };
    unsigned char* key_data;
    int key_data_len, i;
    key_data = (unsigned char*)encryption_key.c_str();
    key_data_len = strlen((char*)key_data);
    const char* str = the_string.c_str();
    int len = strlen((char*)str);
    /* gen key and iv. init the cipher ctx object */
    if (aes_init(key_data, key_data_len, (unsigned char*)&salt, en, de)) {
        return "";
    }
    unsigned char* ciphertext = aes_encrypt(en, (unsigned char*)str, &len);
    return string((const char*)ciphertext);
}

string string_decrypt(string the_string, string encryption_key) {
    /* "opaque" encryption, decryption ctx structures that libcrypto uses to record
       status of enc/dec operations */
    EVP_CIPHER_CTX* en = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX* de = EVP_CIPHER_CTX_new();

    /* 8 bytes to salt the key_data during key generation. This is an example of
       compiled in salt. We just read the bit pattern created by these two 4 byte
       integers on the stack as 64 bits of contigous salt material -
       ofcourse this only works if sizeof(int) >= 4 */
    unsigned int salt[] = { 12345, 54321 };
    unsigned char* key_data;
    int key_data_len, i;
    key_data = (unsigned char*)encryption_key.c_str();
    key_data_len = strlen((char*)key_data);
    const char* str = the_string.c_str();

    int len = strlen((char*)str);
    /* gen key and iv. init the cipher ctx object */
    if (aes_init(key_data, key_data_len, (unsigned char*)&salt, en, de)) {
        return "";
    }
    char* decrypted_text = (char*)aes_decrypt(de, (unsigned char*)str, &len);

    // Trim any extra characters added during encryption
    string decrypted_string(decrypted_text, len);

    return decrypted_string;
}


string url_decode(const string& url) {
    URI uri(url);
    return uri.getPathEtc();
}

string url_encode(const string& url) {
    URI uri;
    uri.setPathEtc(url);
    return uri.toString();
}

string url_get(const string& url) {
    try {
        HTTPClientSession session(url);
        HTTPRequest request(HTTPRequest::HTTP_GET, "/");
        session.sendRequest(request);
        HTTPResponse response;
        istream& rs = session.receiveResponse(response);
        string result;
        StreamCopier::copyToString(rs, result);
        return result;
    }
    catch (const Poco::Exception& e) {
        asIScriptContext* ctx= asGetActiveContext();
        ctx->SetException(e.displayText().c_str());
    }
}



string url_post(const string& url, const string& parameters) {
    try {
        HTTPClientSession session(url);
        HTTPRequest request(HTTPRequest::HTTP_POST, "/");
        request.setContentType("application/x-www-form-urlencoded");
        request.setContentLength(parameters.length());
        ostream& os = session.sendRequest(request);
        os << parameters;
        HTTPResponse response;
        istream& rs = session.receiveResponse(response);
        string result;
        StreamCopier::copyToString(rs, result);
        return result;
    }
    catch (const Poco::Exception& e) {
        asIScriptContext* ctx = asGetActiveContext();
        ctx->SetException(e.displayText().c_str());
    }

}

string ascii_to_character(int the_ascii_code) {
    return string(1, static_cast<char>(the_ascii_code));
}

int character_to_ascii(string the_character) {
    if (the_character.length() == 1) {
        return static_cast<int>(the_character[0]);
    }
    else {
        return -1; // Error: input string must be of length 1
    }
}

string hex_to_string(string the_hexadecimal_sequence) {
    string decoded;
    istringstream iss(the_hexadecimal_sequence);
    ostringstream oss;
    Poco::HexBinaryDecoder decoder(iss);
    decoder >> decoded;
    return decoded;
}


string number_to_hex_string(double the_number) {
    return Poco::NumberFormatter::formatHex(static_cast<unsigned int>(the_number));
}

string string_base64_decode(string base64_string) {
    istringstream iss(base64_string);
    ostringstream oss;
    Poco::Base64Decoder decoder(iss);
    decoder >> oss.rdbuf();
    return oss.str();
}


string string_base64_encode(string the_string) {
    istringstream iss(the_string);
    ostringstream oss;
    Poco::Base64Encoder encoder(oss);
    encoder << iss.rdbuf();
    encoder.close();
    return oss.str();
}
string string_base32_decode(string base32_string) {
    istringstream iss(base32_string);
    ostringstream oss;
    Poco::Base32Decoder decoder(iss);
    decoder >> oss.rdbuf();
    return oss.str();
}


string string_base32_encode(string the_string) {
    istringstream iss(the_string);
    ostringstream oss;
    Poco::Base32Encoder encoder(oss);
    encoder << iss.rdbuf();
    encoder.close();
    return oss.str();
}

string string_to_hex(string the_string) {
    ostringstream oss;
    Poco::HexBinaryEncoder encoder(oss);
    encoder << the_string;
    encoder.close();
    return oss.str();
}
bool alert(const string & title, const string & text, const string &button_name)
{
    SDL_MessageBoxButtonData buttons[] = {
        { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 1, button_name.c_str()},
    };

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        win,
        title.c_str(),
        text.c_str(),
        SDL_arraysize(buttons),
        buttons,
        NULL
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("Error displaying message box");
        return false;
    }



return true;
}
int question(const string& title, const string& text) {
    SDL_MessageBoxButtonData buttons[] = {
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Yes" },
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 2, "No" },
    };

    SDL_MessageBoxData messageboxdata = {
        SDL_MESSAGEBOX_INFORMATION,
        win,
        title.c_str(),
        text.c_str(),
        SDL_arraysize(buttons),
        buttons,
        NULL
    };

    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        SDL_Log("Error displaying message box");
        return -1;
    }

    return buttonid;
}


void wait(uint64_t time) {
    timer waittimer;
    int el = 0;
    while (el < time) {
        SDL_PollEvent(&e);
        el = waittimer.elapsed_millis();
    }
}
void delay(int ms)
{
SDL_Delay(ms);
}
void serialize(asIScriptGeneric* gen) {
    void* dict = gen->GetArgAddress(0);
    char* dict_str = static_cast<char*>(dict);
    string result(dict_str);
    gen->SetReturnObject(&result);
}
void deserialize(asIScriptGeneric* gen) {
    CScriptDictionary*dict = (CScriptDictionary*)gen->GetArgAddress(0);

    gen->SetReturnObject(dict);
}
bool urlopen(const string& url) {
    int result=SDL_OpenURL(url.c_str());
    if (result == 0)return true;
    return false;
}
void timer::construct() {
}

void timer::destruct() {
}
    uint64_t timer::elapsed_seconds() {
        return pausedNanos != 0 ? chrono::duration_cast<chrono::seconds>(chrono::nanoseconds(pausedNanos)).count() 
                                : chrono::duration_cast<chrono::seconds>(
                                  chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_minutes() {
        return pausedNanos != 0 ? chrono::duration_cast<chrono::minutes>(chrono::nanoseconds(pausedNanos)).count() 
                                : chrono::duration_cast<chrono::minutes>(
                                  chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_hours() {
        return pausedNanos != 0 ? chrono::duration_cast<chrono::hours>(chrono::nanoseconds(pausedNanos)).count() 
                                : chrono::duration_cast<chrono::hours>(
                                  chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_millis() {
        return pausedNanos != 0 ? chrono::duration_cast<chrono::milliseconds>(chrono::nanoseconds(pausedNanos)).count() 
                                : chrono::duration_cast<chrono::milliseconds>(
                                  chrono::steady_clock::now() - inittime).count();
    }

    uint64_t timer::elapsed_micros() {
        return pausedNanos != 0 ? chrono::duration_cast<chrono::microseconds>(chrono::nanoseconds(pausedNanos)).count() 
                                : chrono::duration_cast<chrono::microseconds>(
                                  chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_nanos() {
        return pausedNanos != 0 ? pausedNanos 
                                : chrono::duration_cast<chrono::nanoseconds>(
                                  chrono::steady_clock::now() - inittime).count();
    }
    void timer::force_seconds(uint64_t seconds) {
        inittime = chrono::steady_clock::now() - chrono::seconds(seconds);
        pausedNanos = 0;
    }
    void timer::force_minutes(uint64_t minutes) {
        inittime = chrono::steady_clock::now() - chrono::minutes(minutes);
        pausedNanos = 0;
    }
    void timer::force_hours(uint64_t hours) {
        inittime = chrono::steady_clock::now() - chrono::hours(hours);
        pausedNanos = 0;
    }
    void timer::force_millis(uint64_t millis) {
        inittime = chrono::steady_clock::now() - chrono::milliseconds(millis);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in microseconds
    void timer::force_micros(uint64_t micros) {
        inittime = chrono::steady_clock::now() - chrono::microseconds(micros);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in nanoseconds
    void timer::force_nanos(uint64_t nanos) {
        inittime = chrono::steady_clock::now() - chrono::nanoseconds(nanos);
        pausedNanos = 0;
    }

    void timer::restart() {
        inittime = chrono::steady_clock::now();
        pausedNanos = 0;
    }
    void timer::pause() {
        if (pausedNanos == 0) {
            pausedNanos = chrono::duration_cast<chrono::nanoseconds>(
                          chrono::steady_clock::now() - inittime).count();
        }
}
bool timer::is_running() {
    return pausedNanos == 0;
}

    void timer::resume() {
        if (pausedNanos != 0) {
            inittime = chrono::steady_clock::now() - chrono::nanoseconds(pausedNanos);
            pausedNanos = 0;
        }
    }

        void network_event::construct() {}
        void network_event::destruct() {}

        void network::construct() {}
        void network::destruct() { }
        unsigned int network::connect(const string& hostAddress, int port) {
            ENetAddress enetAddress;
            enet_address_set_host(&enetAddress, hostAddress.c_str());
            enetAddress.port = port;

            int initial_timeout = 5000;
            int timeout_multiplier = 2;
            int max_retries = 100;
            int retry_count = 0;

            while (retry_count < max_retries) {
                ENetPeer* peer = enet_host_connect(host, &enetAddress, 1, 0);
                if (peer != NULL) {
                    current_peer_id = peer->incomingPeerID;
                    return peer->incomingPeerID;
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(initial_timeout));
                    initial_timeout *= timeout_multiplier;
                    retry_count++;
                }
            }

            return 0;
        }

        bool network::destroy() {
        if (host) {
            enet_host_destroy(host);
            host = NULL;
            return true;
        }
        return false;
    }
    bool network::disconnect_peer(unsigned int peer_id) {
        if (host) {
            enet_peer_disconnect(&host->peers[peer_id], 0);
            return true;
        }
        return false;
    }
    bool network::disconnect_peer_forcefully(unsigned int peer_id) {
        if (host) {
            enet_peer_reset(&host->peers[peer_id]);
            return true;
        }
        return false;

}
    string address_to_string(const ENetAddress* address)
    {
        uint8_t a = (uint8_t)(address->host);
        uint8_t b = (uint8_t)(address->host >> 8);
        uint8_t c = (uint8_t)(address->host >> 16);
        uint8_t d = (uint8_t)(address->host >> 24);
        return to_string(a) + "." + to_string(b) + "." + to_string(c) + "." + to_string(d) + ":" + to_string(address->port);
    }


    string network::get_peer_address(unsigned int peer_id) {
        return address_to_string(&host->peers[peer_id].address);
    }
    double network::get_peer_average_round_trip_time(unsigned int peerId) {
        return host->peers[peerId].roundTripTime;
    }
    CScriptArray* network::get_peer_list() {
        asIScriptContext* ctx = asGetActiveContext();
        asIScriptEngine* engine = ctx->GetEngine();
        asITypeInfo* arrayType = engine->GetTypeInfoById(engine->GetTypeIdByDecl("array<uint>"));
        CScriptArray* peers = CScriptArray::Create(arrayType, (asUINT)0);
        for (UINT i = 0; i < host->connectedPeers; i++) {
            peers->InsertLast(&host->peers[i]);
        }
        return peers;
    }
    network_event* network::request(int initial_timeout, int* out_host_result) {
        network_event* handle_ = new network_event;
        int timeout = initial_timeout;
        int retry_count = 0;
        const int max_retries = 100;
        const int timeout_multiplier = 2;

        while (retry_count < max_retries) {
            ENetEvent event;
            int result = enet_host_service(host, &event, timeout);
            *out_host_result = result;

            if (result > 0) {
                handle_->m_type = event.type;
                handle_->m_channel = event.channelID;

                if (event.type == ENET_EVENT_TYPE_RECEIVE && event.packet != nullptr) {
                    handle_->m_message = std::string(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
                    enet_packet_destroy(event.packet);
                }

                if (event.peer != nullptr) {
                    handle_->m_peerId = event.peer->incomingPeerID;
                }

                return handle_;
            }
            else if (result == 0) {
                timeout *= timeout_multiplier;
                retry_count++;
            }
            else {
                return handle_;
            }
        }
        return handle_;
    }

    bool network::send_reliable(unsigned int peer_id, const string& packet, int channel) {
        ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str())+1, ENET_PACKET_FLAG_RELIABLE);
        int result=enet_peer_send(&host->peers[peer_id], channel, p);
        if (result == 0)
            return true;
        return false;
    }
    bool network::send_unreliable(unsigned int peer_id, const string& packet, int channel) {
        ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str())+1, 0);
        int result = enet_peer_send(&host->peers[peer_id], channel, p);
        if (result == 0)
            return true;
        return false;
    }
    bool network::set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth) {
        enet_host_bandwidth_limit(host, incomingBandwidth, outgoingBandwidth);
        return true;
    }

    bool network::setup_client(int channels, int maxPeers) {
        address.host = ENET_HOST_ANY;
        address.port = 0; // Let the system choose a port

        host = enet_host_create(NULL, 1, channels, 0, 0);

        if (host == NULL) {
            return false; // Failed to create host
        }

        m_active = true;

        return true;
    }

    bool network::setup_server(int listeningPort, int channels, int maxPeers) {
        address.host = ENET_HOST_ANY;
        address.port = listeningPort;

        host = enet_host_create(&address, maxPeers, channels, 0, 0);

        if (host == NULL) {
            return false; // Failed to create host
        }

        m_active = true;

        return true;
    }

    int network::get_connected_peers() const {
        return host ? host->connectedPeers : 0;
    }

    double network::get_bytes_sent() const {
        return m_bytesSent;
    }

    double network::get_bytes_received() const {
        return m_bytesReceived;
    }

    bool network::is_active() const {
        return m_active;
    }
    void library::construct() {}
    void library::destruct() {}
    bool library::load(const string& libname) {
        lib = SDL_LoadObject(libname.c_str());
                                    return lib != NULL;
                                }
    void library_call(asIScriptGeneric* gen) {
#undef GetObject
        asIScriptContext* ctx = asGetActiveContext();
        library* lib_obj = (library*)gen->GetObject();

        void* ref = gen->GetArgAddress(0);
        string func_name = *static_cast<string*>(ref);
        //Function signature parser:
        vector<string> tokens;
        string token;
        for (char c : func_name) {
            if (c == ' ' || c == '(' || c == ')' || c == ',' || c == ';') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else {
                token += c;
            }
        }

        // First array: Return type and function name
        vector<string> first;
        first.push_back(tokens[0]);  // Return type
        first.push_back(tokens[1]);  // Function name

        // Second array: Parameters
        vector<string> last;
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                last.push_back(tokens[i]);
            }
        }

        void*address = SDL_LoadFunction(lib_obj->lib, first[1].c_str());
        if (address == NULL) {
            const char* name = first[1].c_str();
            string message = "Signature parse error: GetProcAddress failed for '" + string(name) + "'";
            ctx->SetException(message.c_str());
            return;
        }
        asIScriptEngine* engine = ctx->GetEngine();
        int func_id = engine->RegisterGlobalFunction(func_name.c_str(), asFUNCTION(address), asCALL_CDECL);
        asIScriptFunction* script_func = engine->GetFunctionById(func_id);
        asIScriptContext* call_ctx = engine->CreateContext();
        call_ctx->Prepare(script_func);
        for (int i = 1; i <= 10; i++) {
            int arg_count = i - 1;
            if (arg_count < last.size()) {
                if (last[arg_count] == "void") {
                    call_ctx->Release();
                    call_ctx = NULL;
                    ctx->SetException("A type ID can not be void");
                    return;
                }
                else if (last[arg_count] == "int" or last[arg_count]=="int8" or last[arg_count]=="int16" or last[arg_count]=="int32" or last[arg_count]=="int64") {
                    asINT64 value = *static_cast<asINT64*>(gen->GetArgAddress(i));
                    call_ctx->SetArgDWord(arg_count, value);
                }
                else if (last[arg_count] == "uint" or last[arg_count] == "uint8" or last[arg_count] == "uint16" or last[arg_count] == "uint32" or last[arg_count] == "uint64") {
                    unsigned int value = *static_cast<unsigned int*>(gen->GetArgAddress(i));
                    call_ctx->SetArgDWord(arg_count, value);
                }
                else if (last[arg_count] == "short") {
                    short value = *static_cast<short*>(gen->GetArgAddress(i));
                    call_ctx->SetArgWord(arg_count, value);
                }
                else if (last[arg_count] == "long") {
                    long value = *static_cast<long*>(gen->GetArgAddress(i));
                    call_ctx->SetArgQWord(arg_count, value);
                }
                else if (last[arg_count] == "double") {
                    double value = *static_cast<double*>(gen->GetArgAddress(i));
                    call_ctx->SetArgDouble(arg_count, value);
                }
                else if (last[arg_count] == "float") {
                    float value = *static_cast<float*>(gen->GetArgAddress(i));
                    call_ctx->SetArgFloat(arg_count, value);
                }
                else if (last[arg_count] == "string") {
                    void* ref = gen->GetArgAddress(i);
                    string value = *static_cast<string*>(ref);
                    call_ctx->SetArgObject(arg_count, &value);
                }
                else if (last[arg_count] == "c_str") {
                    void* ref = gen->GetArgAddress(i);
                    string value = *static_cast<string*>(ref);
                    const char* str = value.c_str();
                    call_ctx->SetArgObject(arg_count, &str);
                }
                if (last[arg_count] == "?&in" or last[arg_count] == "?&out") {
                    void* ref = gen->GetArgAddress(i);
                    call_ctx->SetArgObject(arg_count, ref);
}
            }
        }
            call_ctx->Execute();
        CScriptDictionary* dict = CScriptDictionary::Create(engine);
        if (first[0] == "int" or first[0]=="int8" or first[0]=="int16" or first[0]=="int32" or first[0]=="int64" or first[0] == "uint" or first[0] == "uint8" or first[0] == "uint16" or first[0] == "uint32" or first[0] == "uint64" or first[0]=="short" or first[0]=="long") {
            asINT64 value = call_ctx->GetReturnDWord();
            dict->Set("0", value);
}
        if (first[0] == "double" or first[0] == "float") {
            double value = call_ctx->GetReturnDouble();
            dict->Set("0", value);
}
        for (int i = 1; i <= 10; i++) {
            int arg_count = i - 1;
            if (arg_count < last.size()) {
            if (last[arg_count] == "int" or last[arg_count] == "int8" or last[arg_count] == "int16" or last[arg_count] == "int32" or last[arg_count] == "int64" or last[arg_count] == "uint" or last[arg_count] == "uint8" or last[arg_count] == "uint16" or last[arg_count] == "uint32" or last[arg_count] == "uint64" or last[arg_count] == "short" or last[arg_count] == "long") {
                asINT64 value = *static_cast<asINT64*>(gen->GetArgAddress(i));
                dict->Set(to_string(i), value);
            }
            else if (last[arg_count] == "double" or last[arg_count] == "float") {
                double value = *static_cast<double*>(gen->GetArgAddress(i));
                dict->Set(to_string(i), value);

            }
            else if (last[arg_count] == "string") {
                void* ref = gen->GetArgAddress(i);

                string value = *static_cast<string*>(ref);
                dict->Set(to_string(i), &value, 67108876);
            }
            if (last[arg_count] == "?&in" or last[arg_count] == "?&out") {
                void* ref = gen->GetArgAddress(i);
                dict->Set(to_string(i), ref, 67108876);

            }

            }
        }
            gen->SetReturnObject(dict);
    }
        void library::unload() {
                                    SDL_UnloadObject(lib);
                                }
        script_thread::script_thread(asIScriptFunction* func) {
            function = func;
            asIScriptContext* current_context = asGetActiveContext();
            if (function == 0) {
                current_context->SetException("Function is null");
            }
            asIScriptEngine* current_engine = current_context->GetEngine();
            context = current_engine->CreateContext();
            context->Prepare(function);
        }
        void script_thread::detach() {
            thread t([this]() {
                this->context->Execute();
                });
            t.detach();
        }
        void script_thread::join() {
            thread t([this]() {
                this->context->Execute();
                });
            t.join();
        }

        void instance::construct() {}
                void instance::destruct() {  }

                bool instance::is_running() {
                            DWORD result = WaitForSingleObject(mutex, 0);
                            if (result == WAIT_OBJECT_0) {
                                ReleaseMutex(mutex);
                                return false;
                            }
                            return true;
                        }

user_idle::user_idle() {}
uint64_t user_idle::elapsed_millis() {
    return get_idle_time();
}
uint64_t user_idle::elapsed_seconds() {
    return get_idle_time() / 1000;
}
uint64_t user_idle::elapsed_minutes() {
    return elapsed_seconds() / 60;
}
uint64_t user_idle::elapsed_hours() {
    return elapsed_minutes() / 60;
}
uint64_t user_idle::elapsed_days() {
    return elapsed_hours() / 24;
}
uint64_t user_idle::elapsed_weeks() {
    return elapsed_days() / 7;
}
uint64_t user_idle::get_idle_time() {
    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
    GetLastInputInfo(&lii);
    DWORD currentTick = GetTickCount();
    return (currentTick - lii.dwTime);
}

uint64_t get_time_stamp_millis() {
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    return millis;
}

uint64_t get_time_stamp_seconds() {
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = chrono::duration_cast<chrono::seconds>(duration).count();
    return seconds;
}
void ngtvector::construct() {}
void ngtvector::destruct() {}
float ngtvector::get_length()const {
    return sqrt(x * x + y * y + z * z);
}
ngtvector& ngtvector::operator=(const ngtvector new_vector) {
    this->x = new_vector.x;
    this->y = new_vector.y;
    this->z = new_vector.z;

    return *this;
}

void sqlite3statement::construct() {}
void sqlite3statement::destruct() {}
void ngtsqlite3::construct() {}
void ngtsqlite3::destruct() {}

int sqlite3statement::step() { return sqlite3_step(stmt); }
    int sqlite3statement::reset() { return sqlite3_reset(stmt); }
    string sqlite3statement::get_expanded_sql_statement() const { return sqlite3_expanded_sql(stmt); }
    string sqlite3statement::get_sql_statement() const { return sqlite3_sql(stmt); }
    int sqlite3statement::get_bind_param_count() const { return sqlite3_bind_parameter_count(stmt); }
    int sqlite3statement::get_column_count() const { return sqlite3_column_count(stmt); }
    int sqlite3statement::bind_blob(int index, const string& value, bool copy) { return sqlite3_bind_blob(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::bind_double(int index, double value) { return sqlite3_bind_double(stmt, index, value); }
    int sqlite3statement::bind_int(int index, int value) { return sqlite3_bind_int(stmt, index, value); }
    int sqlite3statement::bind_int64(int index, int64_t value) { return sqlite3_bind_int64(stmt, index, value); }
    int sqlite3statement::bind_null(int index) { return sqlite3_bind_null(stmt, index); }
    int sqlite3statement::bind_param_index(const string& name) { return sqlite3_bind_parameter_index(stmt, name.c_str()); }
    string sqlite3statement::bind_param_name(int index) { return sqlite3_bind_parameter_name(stmt, index); }
    int sqlite3statement::bind_text(int index, const string& value, bool copy) { return sqlite3_bind_text(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::clear_bindings() { return sqlite3_clear_bindings(stmt); }
    string sqlite3statement::column_blob(int index) { return string(reinterpret_cast<const char*>(sqlite3_column_blob(stmt, index)), sqlite3_column_bytes(stmt, index)); }
    int sqlite3statement::column_bytes(int index) { return sqlite3_column_bytes(stmt, index); }
    string sqlite3statement::column_decltype(int index) { return sqlite3_column_decltype(stmt, index); }
    double sqlite3statement::column_double(int index) { return sqlite3_column_double(stmt, index); }
    int sqlite3statement::column_int(int index) { return sqlite3_column_int(stmt, index); }
    int64_t sqlite3statement::column_int64(int index) { return sqlite3_column_int64(stmt, index); }
    string sqlite3statement::column_name(int index) { return sqlite3_column_name(stmt, index); }
    int sqlite3statement::column_type(int index) { return sqlite3_column_type(stmt, index); }
    string sqlite3statement::column_text(int index) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, index)); }


    int ngtsqlite3::close() { return sqlite3_close(db); }

    int ngtsqlite3::open(const string& filename, int flags) { return sqlite3_open_v2(filename.c_str(), &db, flags, nullptr); }
    sqlite3statement* ngtsqlite3::prepare(const string& name, int& out) {
        sqlite3statement* statement = new sqlite3statement();
        if (sqlite3_prepare_v2(db, name.c_str(), -1, &statement->stmt, 0) != SQLITE_OK)
        {
            out = sqlite3_errcode(db);
            return nullptr;
        }
        return statement;
    }

    int ngtsqlite3::execute(const string& sql)
    {
        return sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    }

    int64_t ngtsqlite3::get_rows_changed() const { return sqlite3_changes(db); }

    int64_t ngtsqlite3::get_total_rows_changed() const { return sqlite3_total_changes(db); }

    int ngtsqlite3::limit(int id, int val) { return sqlite3_limit(db, id, val); }

    int ngtsqlite3::set_authorizer(sqlite3_authorizer* callback, const string& arg) { return 0; }

    int64_t ngtsqlite3::get_last_insert_rowid() const { return sqlite3_last_insert_rowid(db); }

    void ngtsqlite3::set_last_insert_rowid(int64_t id) { sqlite3_set_last_insert_rowid(db, id); }

    int ngtsqlite3::get_last_error() { return sqlite3_errcode(db); }

    string ngtsqlite3::get_last_error_text() { return sqlite3_errmsg(db); }

    bool ngtsqlite3::get_active() const { return db != nullptr; }

