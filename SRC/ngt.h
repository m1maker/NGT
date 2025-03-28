#ifndef NGT_H
#define NGT_H
#pragma once
#include"as_class.h"
#include "contextmgr/contextmgr.h"
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "../GUI/GUI.h"
#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#else
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <time.h>
#endif
#define FFI_BUILDING
#include "ffi.h"
#include "ffi_cfi.h"
#include "ffi_common.h"
#include"tts_voice.h"

#include <Poco/Base32Decoder.h>
#include <Poco/Base32Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Exception.h>
#include <Poco/HexBinaryDecoder.h>
#include <Poco/HexBinaryEncoder.h>
#include <Poco/Net/AcceptCertificateHandler.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/FTPStreamFactory.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTTPSClientSession.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Net/MailMessage.h>
#include <Poco/Net/MailRecipient.h>
#include <Poco/Net/InvalidCertificateHandler.h>
#include <Poco/Net/SMTPClientSession.h>
#include <Poco/Net/SSLManager.h>
#include <Poco/Net/StringPartSource.h>
#include <Poco/NumberFormatter.h>
#include <Poco/Path.h>
#include <Poco/SharedPtr.h>
#include <Poco/StreamCopier.h>
#include <Poco/String.h>
#include <Poco/URI.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Mutex.h>
#include<Poco/NamedMutex.h>
#include <memory>
using namespace Poco;
using namespace Poco::Net;
using Poco::URIStreamOpener;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;
using Poco::Net::HTTPStreamFactory;
using Poco::Net::FTPStreamFactory;
using Poco::Net::MailMessage;
using Poco::Net::MailRecipient;
using Poco::Net::SMTPClientSession;
using Poco::Net::StringPartSource;

#include <enet/enet.h>
#include "sqlite3.h"
#include "scripthandle/scripthandle.h"
#include <random>
#include <type_traits>
#include<chrono>
#include <string>
//#define SDL_MAIN_HANDLED
#include<SDL3/SDL.h>
#include <iostream>
#include<thread>
#include <vector>
#include "scriptdictionary/scriptdictionary.h"
using namespace std;
extern bool g_engineInitialized;

#ifdef _WIN32
wstring wstr(const string& utf8String);
extern bool g_COMInitialized;
#endif
void application_exit(int retcode);
uint64_t get_time_stamp_millis();
uint64_t get_time_stamp_seconds();
void as_printf(asIScriptGeneric* gen);
int get_cpu_count();
int get_system_ram();
string get_platform();
void set_exit_callback(asIScriptFunction* callback = nullptr);
void set_update_window_freq(long);
long get_update_window_freq();
int64_t random(int64_t min, int64_t max);
double randomDouble(double min, double max);
bool random_bool();
int get_last_error();

void set_screen_reader_interrupt(bool);
bool get_screen_reader_interrupt();
bool speak(const string& text, bool stop = get_screen_reader_interrupt());
bool braille(const string& text);

void stop_speech();
string screen_reader_detect();
bool show_window(const string& title, int width = 640, int height = 480, bool enable_renderer = true);
void* get_window_handle();
void hide_window();
void set_window_title(const string&);
bool get_window_active();
void set_window_fullscreen(bool);
SDL_Renderer* get_window_renderer();
void window_present();
string key_to_string(SDL_Scancode);
SDL_Scancode string_to_key(const string&);
void garbage_collect();
bool mouse_pressed(unsigned char);
bool mouse_released(unsigned char);
bool mouse_down(unsigned char);
int get_MOUSE_X();
int get_MOUSE_Y();
int get_MOUSE_Z();
class NGTScripting;
NGTScripting* get_scripting_instance();
extern bool g_shutdown;
extern int g_retcode;
void exit_engine(int = 0);
string number_to_words(uint64_t, bool);
bool clipboard_copy_text(const string&);
string clipboard_read_text();
string get_input();
string input_box(const string& title, const string& text, const string& default_text = "", bool secure = false, bool multiline = false);
bool key_pressed(int);
bool key_released(int);
bool key_down(int);
bool key_repeat(int);
bool force_key_down(SDL_Scancode);
bool force_key_up(SDL_Scancode);
void reset_keyboard();
bool quit_requested();
CScriptArray* keys_pressed();
CScriptArray* keys_released();
CScriptArray* keys_down();
CScriptArray* keys_repeat();
std::string float_to_bytes(float f);
float bytes_to_float(const std::string& s);
std::string double_to_bytes(double d);
double bytes_to_double(const std::string& s);

string string_encrypt(const string& str, string encryption_key);
string string_decrypt(const string& str, string encryption_key);
string url_decode(const string& url);
string url_encode(const string& url);
string url_get(const string& url);
string url_post(const string& url, const string& parameters);
void ftp_download(const string& url, const string& file);
void mail_send(Poco::Net::SMTPClientSession::LoginMethod login_method, asUINT port, const string& username, const string& password, const string& mailhost, const string& sender, const string& recipient, const string& subject, const string& content, const string& attachment = "");
string ascii_to_character(int the_ascii_code);
int character_to_ascii(const string& the_character);
string hex_to_string(const string& the_hexadecimal_sequence);
string number_to_hex_string(asINT64 the_number);
string string_base64_decode(const string& base64_string);
string string_base64_encode(const string& the_string);
string string_base32_decode(const string& base32_string);
string string_base32_encode(const string& the_string);
string string_to_hex(const string& the_string);
void string_pad(std::string&);
void string_unpad(std::string&);
int message_box(const std::string& title, const std::string& text, const std::vector<std::string>& buttons, unsigned int mb_flags);

int message_box_script(const std::string& title, const std::string& text, CScriptArray* buttons, unsigned int flags);

bool alert(const string& title, const string& text, const string& button_name = "OK");
int question(const string& title, const string& text);
void wait(uint64_t);
void wait_event();
string read_environment_variable(const string&);
bool write_environment_variable(const string& path, const string& value);
string serialize(CScriptDictionary*);
CScriptDictionary* deserialize(const string&);
void serialize_array(asIScriptGeneric*);
void deserialize_array(asIScriptGeneric* gen);
bool urlopen(const string& url);
std::string c_str_to_string(const char* ptr, size_t length = 0);
std::wstring wc_str_to_wstring(const wchar_t* ptr, size_t length = 0);

class timer : public as_class {
public:
    std::chrono::high_resolution_clock::time_point inittime;
    uint64_t pausedNanos = 0;

    timer() {
        inittime = std::chrono::high_resolution_clock::now();
        pausedNanos = 0;
    }

    uint64_t elapsed_seconds();
    uint64_t elapsed_minutes();
    uint64_t elapsed_hours();
    uint64_t elapsed_millis();
    uint64_t elapsed_micros();
    uint64_t elapsed_nanos();
    void force_seconds(uint64_t);
    void force_minutes(uint64_t);
    void force_hours(uint64_t);
    void force_millis(uint64_t);
    void force_micros(uint64_t);
    void force_nanos(uint64_t);
    void restart();
    void pause();
    void resume();
    bool is_running();
};

class network_event : public as_class {
public:
	const int EVENT_NONE = ENET_EVENT_TYPE_NONE;
	const int EVENT_CONNECT = ENET_EVENT_TYPE_CONNECT;
	const int EVENT_RECEIVE = ENET_EVENT_TYPE_RECEIVE;
	const int EVENT_DISCONNECT = ENET_EVENT_TYPE_DISCONNECT;
	int get_type() const {
		return m_type;
	}

	asQWORD get_peer_id() const {
		return m_peerId;
	}

	unsigned	int get_channel() const {
		return m_channel;
	}

	string get_message() const {
		return m_message;
	}

	int m_type;
	asQWORD m_peerId;
	int m_channel;
	string m_message;
	network_event& operator=(const network_event);
};
enum network_type {
	NETWORK_TYPE_SERVER,
	NETWORK_TYPE_CLIENT
};
class network : public as_class {
public:
	map<asQWORD, _ENetPeer*> peers;
	_ENetPeer* get_peer(asQWORD);
	network_type type;
	asQWORD connect(const string& host, int port);
	bool destroy();
	bool disconnect_peer(asQWORD);
	bool disconnect_peer_forcefully(asQWORD);
	bool disconnect_peer_softly(asQWORD);
	string get_peer_address(asQWORD);
	double get_peer_average_round_trip_time(asQWORD);
	CScriptArray* get_peer_list();
	network_event* request(int timeout = 0);
	bool send_reliable(asQWORD peerId, const string& packet, int channel);
	bool send_unreliable(asQWORD peerId, const string& packet, int channel);
	bool set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth);
	bool setup_client(int channels, asQWORD maxPeers);
	bool setup_server(int listeningPort, int channels, asQWORD maxPeers);
	void flush();
	asQWORD get_connected_peers() const;

	double get_bytes_sent() const;

	double get_bytes_received() const;

	bool is_active() const;

private:
	ENetAddress address;
	ENetHost* host;
	asQWORD current_peer_id = 1;
	asQWORD m_connectedPeers;
	double m_bytesSent;
	double m_bytesReceived;
	bool m_active;
};
typedef struct {
	void* address;
	ffi_type* returnType;
	std::vector<ffi_type*> parameters;
	std::vector<std::string> parameterTypes; // For pointers handling.
}LibraryFunction;
class library : public as_class {
public:
	SDL_SharedObject* lib;
	std::map<std::string, LibraryFunction> functions;
	bool active()const;
	bool load(const string&);
	void unload();
	void* get_function_pointer(const std::string& name);
	void clear_functions();
};
void library_call(asIScriptGeneric* gen);
class script_thread : public as_class {
private:
	asIScriptContext* context;
	asIScriptFunction* function;
	thread* th;
public:
	script_thread(asIScriptFunction* func, CScriptDictionary* args = nullptr);
	void join();
	void detach();
	void destroy();
};




class user_idle : public as_class {
public:
	user_idle();
	uint64_t elapsed_millis();  // Pridaná nová funkcia
	uint64_t elapsed_seconds();
	uint64_t elapsed_minutes();
	uint64_t elapsed_hours();
	uint64_t elapsed_days();
	uint64_t elapsed_weeks();

private:
	uint64_t get_idle_time();
};
class sqlite3statement : public as_class
{
public:
	int step();
	int reset();
	string get_expanded_sql_statement()const;
	string get_sql_statement() const;
	int get_bind_param_count()const;
	int get_column_count()const;
	int bind_blob(int index, const string& value, bool copy = true);
	int bind_double(int index, double value);
	int bind_int(int index, int value);
	int bind_int64(int index, int64_t value);
	int bind_null(int index);
	int bind_param_index(const string& name);
	string bind_param_name(int index);
	int bind_text(int index, const string& value, bool copy = true);
	int clear_bindings();
	string column_blob(int index);
	int column_bytes(int index);
	string column_decltype(int index);
	double column_double(int index);
	int column_int(int index);
	int64_t column_int64(int index);
	string column_name(int index);
	int column_type(int index);
	string column_text(int index);
	sqlite3_stmt* stmt;
};

using sqlite3_authorizer = int(*)(string, int, string, string, string, string);


class ngtsqlite3 : public as_class
{
public:
	ngtsqlite3();
	int close();
	int open(const string& filename, int flags = 6);
	sqlite3statement* prepare(const string& name, int& out);

	int execute(const string& sql);
	int64_t get_rows_changed() const;
	int64_t get_total_rows_changed()const;
	int limit(int id, int val);
	int set_authorizer(sqlite3_authorizer* callback, const string& arg = "");
	int64_t get_last_insert_rowid() const;
	void set_last_insert_rowid(int64_t id);
	int get_last_error();
	string get_last_error_text();
	bool get_active()const;
private:
	sqlite3* db;
};

CContextMgr* get_context_manager();

struct DeviceButton
{
	bool isDown = false;
	bool isPressed = false;
	bool isReleased = false;
};

struct RefObject {
	int refcount;
	int magic;
};

template <class T> inline void* ObgectCreate() {
	RefObject* rc = (RefObject*) malloc(sizeof(RefObject) + sizeof(T)); // We don't use the new operator here because we don't want the constructor of the containing object to fire until the user's factory function.
	rc->refcount = 1;
	rc->magic = 0x1234abcd;
	return (char*)rc + sizeof(RefObject);
}

inline RefObject* RefObjectGet(void* obj) {
	RefObject* rc = reinterpret_cast<RefObject*>((char*)obj - sizeof(RefObject));
	if (rc->magic != 0x1234abcd) return NULL; // This pointer didn't originate from our factory.
	return rc;
}


template<class T>
void ObjectAddRef(T* ptr);
template <class T>
void ObjectRelease(T* ptr);
template <class T>
inline void RegisterObject(asIScriptEngine* engine, const char* type) {
	engine->RegisterObjectType(type, 0, asOBJ_REF);
	engine->RegisterObjectBehaviour(type, asBEHAVE_ADDREF, "void f()", asFUNCTION(ObjectAddRef<T>), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectBehaviour(type, asBEHAVE_RELEASE, "void f()", asFUNCTION(ObjectRelease<T>), asCALL_CDECL_OBJFIRST);
}

template <class T, typename... A> T* ObjectFactory(A... args);


#endif