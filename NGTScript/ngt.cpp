#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#include "openssl/aes.h"
#include "openssl/rand.h"
#include "openssl/evp.h"
#include "openssl/err.h"
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
#include <stdio.h>
#include "NGT.H"
#include "scriptfile/scriptfilesystem.h"
#include <locale>
#include <codecvt>
#include<fstream>
#include <cstdlib>
#include<algorithm>
#include <cstring>
using namespace std;
bool engine_is_active= false;
SDL_Window* win = NULL;

SDL_Event e;
bool window_is_focused;
std::wstring wstr(const std::string& utf8String)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(utf8String);
}
std::wstring reader;
std::unordered_map<SDL_Scancode, bool> keys;
bool keyhook = false;
std::string inputtext;
void replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
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

void init_engine(){
    Tolk_Load();

    SDL_Init(SDL_INIT_EVERYTHING);
    enet_initialize();
    Tolk_TrySAPI(true);
    reader=Tolk_DetectScreenReader();
    if (!Tolk_HasSpeech()) {
        Tolk_PreferSAPI(true);
}
}   
    void set_library_path(const std::string& path) {
    if (Tolk_IsLoaded())Tolk_Unload();
    std::filesystem::path current_dir = std::filesystem::current_path();
    std::filesystem::path new_dir= std::filesystem::current_path()/path;

    std::filesystem::current_path(new_dir);

    Tolk_Load();
    soundsystem_free();
    soundsystem_init();
    std::filesystem::current_path(current_dir);
}

std::random_device rd;
std::mt19937 gen(rd());
long random(long min, long max) {
    static_assert(std::is_integral<long>::value, "Type must be integral");

    std::uniform_int_distribution<long> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}
double randomDouble(double min, double max) {
    static_assert(std::is_floating_point<double>::value, "Type must be floating point");

    std::uniform_real_distribution<double> dis(min, max);

    return dis(gen); // Ensure 'gen' is a valid random number generator
}

bool random_bool() {
    return rand() % 2 == 0;
}
int get_last_error() {
    return 0;
}
void speak(const std::string & text, bool stop) {
    std::wstring textstr = wstr(text);
    std::thread speakingThread([textstr, stop]() {
        Tolk_Speak(textstr.c_str(), stop);
        });
    speakingThread.detach();
}
void speak_wait(const std::string & text, bool stop) {
    speak(text, stop);
    while (Tolk_IsSpeaking()) {
        SDL_PollEvent(&e);
    }
}

void stop_speech() {
        Tolk_Silence();
    }
std::string screen_reader_detect() {
    reader = Tolk_DetectScreenReader();
    return std::string(reader.begin(), reader.end());
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

bool show_game_window(const std::string & title,int width, int height, bool closable)
{
    if (win != NULL)
        return false;
    if (reader == L"JAWS") {
        win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_KEYBOARD_GRABBED);
        SDL_SetHint(SDL_HINT_ALLOW_ALT_TAB_WHILE_GRABBED, "1");
    }
    else
win = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
SDL_RegisterApp("NGTWindow", 0, win);

SDL_StartTextInput();
window_closable = closable;
if (win!=NULL)
{
    update_game_window();
    return true;
}
return false;
}
bool focus_game_window() {
    SDL_SetWindowInputFocus(win);
    return true;
}
void hide_game_window() {
    SDL_StopTextInput();

    SDL_DestroyWindow(win);
}
void set_game_window_title(const std::string & new_title) {
    SDL_SetWindowTitle(win, new_title.c_str());
}
void set_game_window_closable(bool set_closable) {
    window_closable = set_closable;
}
void update_game_window()
{
    if (win != nullptr) {
        SDL_PollEvent(&e);
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

        if (e.type == SDL_WINDOWEVENT_FOCUS_GAINED)
            window_is_focused == true;
        if (e.type == SDL_WINDOWEVENT_FOCUS_LOST)
            window_is_focused == false;
    }
}
    bool is_game_window_active() {
    return window_is_focused;
}
void exit_engine(int return_number)
{
    soundsystem_free();
    SDL_StopTextInput();
    SDL_DestroyWindow(win);
win=NULL;
SDL_UnregisterApp();
enet_deinitialize();
Tolk_Unload();
SDL_Quit();
exit(return_number);
}
std::string number_to_words(uint64_t num, bool include_and)
{
    std::vector<char> buf(1024);
    int use_and = (include_and ? 1 : 0);
    size_t bufsize = bl_number_to_words(num, buf.data(), buf.size(), use_and);
    if (bufsize > buf.size())
    {
        return "";
    }
    return std::string(buf.data());
}
std::string read_environment_variable(const std::string& path) {
#ifdef _WIN32
    // Use _dupenv_s on Windows
    char* value;
    size_t size;
    if (_dupenv_s(&value, &size, path.c_str()) != 0 || value == nullptr) {
        // Environment variable not found or error occurred
        return "";
    }
    const std::string & result(value);
    free(value);  // Free the allocated memory
    return result;
#else
    // Use getenv for non-Windows platforms
    char* value = std::getenv(path.c_str());
    if (value == nullptr) {
        // Environment variable not found
        return "";
    }
    return const std::string &(value);
#endif
}
bool clipboard_copy_text(const std::string & text) {
    SDL_SetClipboardText(text.c_str());
    return true;
}
std::string clipboard_read_text() {
    return SDL_GetClipboardText();
}
std::string get_input() {
    std::string temp=inputtext;
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
std::string string_encrypt(std::string the_string, std::string encryption_key) {
    AES_KEY aesKey;
    if (AES_set_encrypt_key((const unsigned char*)encryption_key.c_str(), 256, &aesKey) != 0) {
        return "";
    }

    std::string ciphertext;
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0, AES_BLOCK_SIZE); // Initialize IV to zeros or use a secure random IV

    // Create a buffer large enough to hold the encrypted data
    ciphertext.resize(the_string.length() + AES_BLOCK_SIZE);

    AES_cbc_encrypt((const unsigned char*)the_string.c_str(), (unsigned char*)&ciphertext[0], the_string.length(), &aesKey, iv, AES_ENCRYPT);
    return ciphertext;
}

std::string string_decrypt(std::string the_string, std::string encryption_key) {
    AES_KEY aesKey;
    if (AES_set_decrypt_key((const unsigned char*)encryption_key.c_str(), 256, &aesKey) != 0) {
        return "";
    }

    std::string decryptedtext;
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0, AES_BLOCK_SIZE); // Assuming IV is all zeros

    // Create a buffer large enough to hold the decrypted data
    decryptedtext.resize(the_string.length());

    AES_cbc_encrypt((const unsigned char*)the_string.c_str(), (unsigned char*)&decryptedtext[0], the_string.length(), &aesKey, iv, AES_DECRYPT);
    return decryptedtext;
}

std::string url_decode(const std::string& url) {
    URI uri(url);
    return uri.getPathEtc();
}

std::string url_encode(const std::string& url) {
    URI uri;
    uri.setPathEtc(url);
    return uri.toString();
}

std::string url_get(const std::string& url) {
    HTTPClientSession session(url);
    HTTPRequest request(HTTPRequest::HTTP_GET, "/");
    session.sendRequest(request);
    HTTPResponse response;
    std::istream& rs = session.receiveResponse(response);
    std::string result;
    StreamCopier::copyToString(rs, result);
    return result;
}

std::string url_post(const std::string& url, const std::string& parameters) {
    HTTPClientSession session(url);
    HTTPRequest request(HTTPRequest::HTTP_POST, "/");
    request.setContentType("application/x-www-form-urlencoded");
    request.setContentLength(parameters.length());
    std::ostream& os = session.sendRequest(request);
    os << parameters;
    HTTPResponse response;
    std::istream& rs = session.receiveResponse(response);
    std::string result;
    StreamCopier::copyToString(rs, result);
    return result;
}

std::string ascii_to_character(int the_ascii_code) {
    return std::string(1, static_cast<char>(the_ascii_code));
}

int character_to_ascii(std::string the_character) {
    if (the_character.length() == 1) {
        return static_cast<int>(the_character[0]);
    }
    else {
        return -1; // Error: input string must be of length 1
    }
}

std::string hex_to_string(std::string the_hexadecimal_sequence) {
    std::string decoded;
    std::istringstream iss(the_hexadecimal_sequence);
    std::ostringstream oss;
    Poco::HexBinaryDecoder decoder(iss);
    decoder >> decoded;
    return decoded;
}


std::string number_to_hex_string(double the_number) {
    return Poco::NumberFormatter::formatHex(static_cast<unsigned int>(the_number));
}

std::string string_base64_decode(std::string base64_string) {
    std::istringstream iss(base64_string);
    std::ostringstream oss;
    Poco::Base64Decoder decoder(iss);
    decoder >> oss.rdbuf();
    return oss.str();
}


std::string string_base64_encode(std::string the_string) {
    std::istringstream iss(the_string);
    std::ostringstream oss;
    Poco::Base64Encoder encoder(oss);
    encoder << iss.rdbuf();
    encoder.close();
    return oss.str();
}

std::string string_to_hex(std::string the_string) {
    std::ostringstream oss;
    Poco::HexBinaryEncoder encoder(oss);
    encoder << the_string;
    encoder.close();
    return oss.str();
}


bool alert(const std::string & title, const std::string & text, const std::string &button_name)
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
int question(const std::string& title, const std::string& text) {
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


void wait(int time) {
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
std::string serialize(CScriptDictionary* the_data) {
    std::string result;
    for (auto it : *the_data)
    {
        std::string keyName = it.GetKey();
        int typeId = it.GetTypeId();
        const void* addressOfValue = it.GetAddressOfValue();
        // Serialize keyName
        result += keyName + "|";

        // Serialize typeId
        result += std::to_string(typeId) + "|";

        // Serialize value based on typeId
        switch (typeId)
        {
        case asTYPEID_DOUBLE:
            result += std::to_string(*(double*)addressOfValue) + "|";
            break;
        case asTYPEID_FLOAT:
            result += std::to_string(*(float*)addressOfValue) + "|";
            break;
        case asTYPEID_INT64:
            result += std::to_string(*(int64_t*)addressOfValue) + "|";
            break;
        case asTYPEID_INT32:
            result += std::to_string(*(int32_t*)addressOfValue) + "|";
            break;
        case asTYPEID_INT16:
            result += std::to_string(*(int16_t*)addressOfValue) + "|";
            break;
        case asTYPEID_INT8:
            result += std::to_string(*(int8_t*)addressOfValue) + "|";
            break;
        case asTYPEID_UINT64:
            result += std::to_string(*(uint64_t*)addressOfValue) + "|";
            break;
        case asTYPEID_UINT32:
            result += std::to_string(*(uint32_t*)addressOfValue) + "|";
            break;
        case asTYPEID_UINT16:
            result += std::to_string(*(uint16_t*)addressOfValue) + "|";
            break;
        case asTYPEID_UINT8:
            result += std::to_string(*(uint8_t*)addressOfValue) + "|";
            break;
        case asTYPEID_BOOL:
            result += *(bool*)addressOfValue + "|";
            break;

            // if it is not a const value, use cached type_id (type_id defined at AS engine runtime)
        default:break;
        }
        }
        return result;
}
CScriptDictionary* deserialize(const std::string& data) {
    asIScriptContext* ctx = asGetActiveContext();
    asIScriptEngine* engine = ctx->GetEngine();

    CScriptDictionary* result = CScriptDictionary::Create(engine);

    return result;
    }
void timer::construct() {
}

void timer::destruct() {
}
    uint64_t timer::elapsed_seconds() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::seconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::seconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_minutes() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::minutes>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::minutes>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_hours() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::hours>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::hours>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_millis() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::milliseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }

    uint64_t timer::elapsed_micros() {
        return pausedNanos != 0 ? std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::nanoseconds(pausedNanos)).count() 
                                : std::chrono::duration_cast<std::chrono::microseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    uint64_t timer::elapsed_nanos() {
        return pausedNanos != 0 ? pausedNanos 
                                : std::chrono::duration_cast<std::chrono::nanoseconds>(
                                  std::chrono::steady_clock::now() - inittime).count();
    }
    void timer::force_seconds(uint64_t seconds) {
        inittime = std::chrono::steady_clock::now() - std::chrono::seconds(seconds);
        pausedNanos = 0;
    }
    void timer::force_minutes(uint64_t minutes) {
        inittime = std::chrono::steady_clock::now() - std::chrono::minutes(minutes);
        pausedNanos = 0;
    }
    void timer::force_hours(uint64_t hours) {
        inittime = std::chrono::steady_clock::now() - std::chrono::hours(hours);
        pausedNanos = 0;
    }
    void timer::force_millis(uint64_t millis) {
        inittime = std::chrono::steady_clock::now() - std::chrono::milliseconds(millis);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in microseconds
    void timer::force_micros(uint64_t micros) {
        inittime = std::chrono::steady_clock::now() - std::chrono::microseconds(micros);
        pausedNanos = 0;
    }

    // Force the timer to a specific time in nanoseconds
    void timer::force_nanos(uint64_t nanos) {
        inittime = std::chrono::steady_clock::now() - std::chrono::nanoseconds(nanos);
        pausedNanos = 0;
    }

    void timer::restart() {
        inittime = std::chrono::steady_clock::now();
        pausedNanos = 0;
    }
    void timer::pause() {
        if (pausedNanos == 0) {
            pausedNanos = std::chrono::duration_cast<std::chrono::nanoseconds>(
                          std::chrono::steady_clock::now() - inittime).count();
        }
}
bool timer::is_running() {
    return pausedNanos == 0;
}

    void timer::resume() {
        if (pausedNanos != 0) {
            inittime = std::chrono::steady_clock::now() - std::chrono::nanoseconds(pausedNanos);
            pausedNanos = 0;
        }
    }

        void network_event::construct() {}
        void network_event::destruct() {}

        void network::construct() {}
        void network::destruct() { }
    unsigned int network::connect(const std::string& hostAddress, int port) {
        ENetAddress enetAddress;
        enet_address_set_host(&enetAddress, hostAddress.c_str());
        enetAddress.port = port;

        ENetPeer* peer = enet_host_connect(host, &enetAddress, 1, 0);
        if (peer == NULL) {
            return 0; // Connection failed
        }
        current_peer_id = peer->incomingPeerID;
        return peer->incomingPeerID;
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
    std::string network::get_peer_address(unsigned int peer_id) {
        char addressStr[64];
        enet_address_get_host_ip(&host->peers[peer_id].address, addressStr, sizeof(addressStr));
        return std::string(addressStr);
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
    network_event* network::request() {
        network_event* handle_ = new network_event;

        std::thread t([this, handle_]() {
            ENetEvent event;
            enet_host_service(host, &event, 0);
if(event.type!=NULL)
            handle_->m_type = event.type;
if(event.channelID!=0)
handle_->m_channel = event.channelID;

            if (event.packet != nullptr) {
                handle_->m_message = std::string(reinterpret_cast<char*>(event.packet->data));
            }

            if (event.peer != nullptr) {
                handle_->m_peerId = event.peer->incomingPeerID;
            }
            });

        t.detach();
        return handle_;

    }

    bool network::send_reliable(unsigned int peer_id, const std::string& packet, int channel) {
        ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str()), ENET_PACKET_FLAG_RELIABLE);
        int result=enet_peer_send(&host->peers[peer_id], channel, p);
        enet_packet_destroy(p);
        if (result == 0)
            return true;
        return false;
    }
    bool network::send_unreliable(unsigned int peer_id, const std::string& packet, int channel) {
        ENetPacket* p = enet_packet_create(packet.c_str(), strlen(packet.c_str()), 0);
        int result = enet_peer_send(&host->peers[peer_id], channel, p);
        enet_packet_destroy(p);
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
    bool library::load(const std::string& libname) {
                                    lib = LoadLibraryW(std::wstring(libname.begin(), libname.end()).c_str());
                                    return lib == NULL;
                                }
    typedef void* (*func)(void**, void**, void**, void**, void**, void**, void**, void**, void**, void**);
    void library_call(asIScriptGeneric* gen) {
#undef GetObject
        asIScriptContext* ctx = asGetActiveContext();
        library* lib_obj = (library*)gen->GetObject();

        void* ref = gen->GetArgAddress(0);
        std::string func_name = *static_cast<string*>(ref);
        //Function signature parser:
        std::vector<std::string> tokens;
        std::string token;
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
        std::vector<std::string> first;
        first.push_back(tokens[0]);  // Return type
        first.push_back(tokens[1]);  // Function name

        // Second array: Parameters
        std::vector<std::string> last;
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                last.push_back(tokens[i]);
            }
        }

        FARPROC address = GetProcAddress(lib_obj->lib, first[1].c_str());
        if (address == NULL) {
            const char* name = first[1].c_str();
            std::string message = "Signature parse error: GetProcAddress failed for '" + std::string(name) + "'";
            ctx->SetException(message.c_str());
            return;
        }
        func function = reinterpret_cast<func>(address);
        asIScriptEngine* engine = ctx->GetEngine();
        int func_id = engine->RegisterGlobalFunction(func_name.c_str(), asFUNCTION(function), asCALL_CDECL);
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
                    std::string value = *static_cast<std::string*>(ref);
                    call_ctx->SetArgObject(arg_count, &value);
                }
                else if (last[arg_count] == "c_str") {
                    void* ref = gen->GetArgAddress(i);
                    std::string value = *static_cast<std::string*>(ref);
                    const char* str = value.c_str();
                    call_ctx->SetArgObject(arg_count, &str);
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
                dict->Set(std::to_string(i), value);
            }
            else if (last[arg_count] == "double" or last[arg_count] == "float") {
                double value = *static_cast<double*>(gen->GetArgAddress(i));
                dict->Set(std::to_string(i), value);

            }
            else if (last[arg_count] == "string") {
                void* ref = gen->GetArgAddress(i);

                std::string value = *static_cast<std::string*>(ref);
                dict->Set(std::to_string(i), &value, 67108876);
            }

            }
        }
            gen->SetReturnObject(dict);
    }
        void library::unload() {
                                    FreeLibrary(lib);
                                }
        script_thread::script_thread(asIScriptFunction* func, CScriptDictionary* args) {
            function = func;
            asIScriptContext* current_context = asGetActiveContext();
            if (function == 0) {
                current_context->SetException("Function is null");
}
            asIScriptEngine* current_engine = current_context->GetEngine();
            context = current_engine->CreateContext();
            alert("W", std::string(function->GetDeclaration()));
            context->Prepare(function);
            for (int i = 0; i < args->GetSize(); i++) {
                void* arg=NULL;
                args->Get(std::to_string(i), arg, args->GetTypeId(std::to_string(i)));
                context->SetArgAddress(i, arg);
            }
        }
        void script_thread::detach() {
            std::thread t([this]() {
                this->context->Execute();
                });
            t.detach();
        }
        void script_thread::join() {
            std::thread t([this]() {
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
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return millis;
}

uint64_t get_time_stamp_seconds() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
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
    std::string sqlite3statement::get_expanded_sql_statement() const { return sqlite3_expanded_sql(stmt); }
    std::string sqlite3statement::get_sql_statement() const { return sqlite3_sql(stmt); }
    int sqlite3statement::get_bind_param_count() const { return sqlite3_bind_parameter_count(stmt); }
    int sqlite3statement::get_column_count() const { return sqlite3_column_count(stmt); }
    int sqlite3statement::bind_blob(int index, const std::string& value, bool copy) { return sqlite3_bind_blob(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::bind_double(int index, double value) { return sqlite3_bind_double(stmt, index, value); }
    int sqlite3statement::bind_int(int index, int value) { return sqlite3_bind_int(stmt, index, value); }
    int sqlite3statement::bind_int64(int index, int64_t value) { return sqlite3_bind_int64(stmt, index, value); }
    int sqlite3statement::bind_null(int index) { return sqlite3_bind_null(stmt, index); }
    int sqlite3statement::bind_param_index(const std::string& name) { return sqlite3_bind_parameter_index(stmt, name.c_str()); }
    std::string sqlite3statement::bind_param_name(int index) { return sqlite3_bind_parameter_name(stmt, index); }
    int sqlite3statement::bind_text(int index, const std::string& value, bool copy) { return sqlite3_bind_text(stmt, index, value.c_str(), value.size(), copy ? SQLITE_TRANSIENT : SQLITE_STATIC); }
    int sqlite3statement::clear_bindings() { return sqlite3_clear_bindings(stmt); }
    std::string sqlite3statement::column_blob(int index) { return std::string(reinterpret_cast<const char*>(sqlite3_column_blob(stmt, index)), sqlite3_column_bytes(stmt, index)); }
    int sqlite3statement::column_bytes(int index) { return sqlite3_column_bytes(stmt, index); }
    std::string sqlite3statement::column_decltype(int index) { return sqlite3_column_decltype(stmt, index); }
    double sqlite3statement::column_double(int index) { return sqlite3_column_double(stmt, index); }
    int sqlite3statement::column_int(int index) { return sqlite3_column_int(stmt, index); }
    int64_t sqlite3statement::column_int64(int index) { return sqlite3_column_int64(stmt, index); }
    std::string sqlite3statement::column_name(int index) { return sqlite3_column_name(stmt, index); }
    int sqlite3statement::column_type(int index) { return sqlite3_column_type(stmt, index); }
    std::string sqlite3statement::column_text(int index) { return reinterpret_cast<const char*>(sqlite3_column_text(stmt, index)); }


    int ngtsqlite3::close() { return sqlite3_close(db); }

    int ngtsqlite3::open(const std::string& filename, int flags) { return sqlite3_open_v2(filename.c_str(), &db, flags, nullptr); }
    sqlite3statement* ngtsqlite3::prepare(const std::string& name, int& out) {
        sqlite3statement* statement = new sqlite3statement();
        if (sqlite3_prepare_v2(db, name.c_str(), -1, &statement->stmt, 0) != SQLITE_OK)
        {
            out = sqlite3_errcode(db);
            return nullptr;
        }
        return statement;
    }

    int ngtsqlite3::execute(const std::string& sql)
    {
        return sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    }

    int64_t ngtsqlite3::get_rows_changed() const { return sqlite3_changes(db); }

    int64_t ngtsqlite3::get_total_rows_changed() const { return sqlite3_total_changes(db); }

    int ngtsqlite3::limit(int id, int val) { return sqlite3_limit(db, id, val); }

    int ngtsqlite3::set_authorizer(sqlite3_authorizer* callback, const std::string& arg) { return 0; }

    int64_t ngtsqlite3::get_last_insert_rowid() const { return sqlite3_last_insert_rowid(db); }

    void ngtsqlite3::set_last_insert_rowid(int64_t id) { sqlite3_set_last_insert_rowid(db, id); }

    int ngtsqlite3::get_last_error() { return sqlite3_errcode(db); }

    std::string ngtsqlite3::get_last_error_text() { return sqlite3_errmsg(db); }

    bool ngtsqlite3::get_active() const { return db != nullptr; }

