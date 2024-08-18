#ifndef NGT_H
#define NGT_H
#pragma once
#include"as_class.h"
#ifdef _WIN32
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "../GUI/GUI.h"
#include <Windows.h>
#include"tts_voice.h"
#endif
#define FFI_BUILDING
#include "ffi.h"
#include "ffi_cfi.h"
#include "ffi_common.h"
#include "Poco/Base32Decoder.h"
#include "Poco/Base32Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Exception.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/Net/FTPStreamFactory.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPStreamFactory.h"
#include "Poco/Net/MailMessage.h"
#include "Poco/Net/MailRecipient.h"
#include "Poco/Net/SMTPClientSession.h"
#include "Poco/Net/StringPartSource.h"
#include "Poco/NumberFormatter.h"
#include "Poco/Path.h"
#include "Poco/StreamCopier.h"
#include "Poco/String.h"
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "poco/mutex.h"
#include"poco/NamedMutex.h"
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

#include "enet/enet.h"
#include <fcntl.h>
#include <io.h>
#include "sqlite3.h"
#include "scripthandle/scripthandle.h"
#include <random>
#include <type_traits>
#include<chrono>
#include <string>
//#define SDL_MAIN_HANDLED
#include"sdl3/SDL.h"
#include <iostream>
#include<thread>
#include <vector>
#include "scriptdictionary/scriptdictionary.h"
using namespace std;
class ngtvector : public as_class {
public:
	float x, y, z;
	float get_length()const;
	ngtvector& operator=(const ngtvector);
	void reset();
	ngtvector();
};

#ifdef _WIN32
wstring wstr(const string& utf8String);
extern bool g_COMInitialized;
#endif
string unicode_convert(string str, string from, string to);
uint64_t get_time_stamp_millis();
uint64_t get_time_stamp_seconds();
void as_printf(asIScriptGeneric* gen);
int get_cpu_count();
int get_system_ram();
string get_platform();
void set_exit_callback(asIScriptFunction* callback = nullptr);
void set_update_window_freq(long);
long get_update_window_freq();
void init_engine();
void set_library_path(const string& path);
long random(long min, long max);
double randomDouble(double min, double max);
bool random_bool();
int get_last_error();

void set_screen_reader_interrupt(bool);
bool get_screen_reader_interrupt();
void speak(const string& text, bool stop = get_screen_reader_interrupt());
void speak_wait(const string& text, bool stop = true);
void stop_speech();
string screen_reader_detect();
void show_console();
void hide_console();
bool show_window(const string& title, int width = 640, int height = 480, bool closable = true);
void* get_window_handle();
void hide_window();
void set_window_title(const string&);
void set_window_closable(bool);
bool get_window_active();
void set_window_fullscreen(bool);
SDL_Surface* get_window_surface();
SDL_Surface* load_bmp(const string& file);
string key_to_string(SDL_Scancode);
SDL_Scancode string_to_key(const string&);
void garbage_collect();
bool mouse_pressed(Uint8);
bool mouse_released(Uint8);
bool mouse_down(Uint8);
int get_MOUSE_X();
int get_MOUSE_Y();
int get_MOUSE_Z();
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
void reset_all_forced_keys();

CScriptArray* keys_pressed();
CScriptArray* keys_released();
CScriptArray* keys_down();
CScriptArray* keys_repeat();
string string_encrypt(const string& str, string& encryption_key);
string string_decrypt(const string& str, string& encryption_key);
string url_decode(const string& url);
string url_encode(const string& url);
string url_get(const string& url);
string url_post(const string& url, const string& parameters);
void ftp_download(const string& url, const string& file);
void mail_send(Poco::Net::SMTPClientSession::LoginMethod login_method, asUINT port, const string& username, const string& password, const string& mailhost, const string& sender, const string& recipient, const string& subject, const string& content, const string& attachment = "");
string ascii_to_character(int the_ascii_code);
int character_to_ascii(string the_character);
string hex_to_string(string the_hexadecimal_sequence);
string number_to_hex_string(double the_number);
string string_base64_decode(string base64_string);
string string_base64_encode(string the_string);
string string_base32_decode(string base32_string);
string string_base32_encode(string the_string);
string string_to_hex(string the_string);
bool alert(const string& title, const string& text, const string& button_name = "OK");
int question(const string& title, const string& text);
void wait(uint64_t);
string read_environment_variable(const string&);
bool write_environment_variable(const string& path, const string& value);
string serialize(CScriptDictionary*);
CScriptDictionary* deserialize(const string&);
string serialize_array(CScriptArray*);
CScriptArray* deserialize_array(const string&);
bool urlopen(const string& url);
std::string c_str_to_string(const char* ptr, size_t length = 0);
std::wstring wc_str_to_wstring(const wchar_t* ptr, size_t length = 0);
class timer : public as_class {
public:
	chrono::time_point<chrono::steady_clock> inittime;
	uint64_t pausedNanos = 0;

	timer() {
		inittime = chrono::steady_clock::now();
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

	unsigned    int get_peer_id() const {
		return m_peerId;
	}

	unsigned    int get_channel() const {
		return m_channel;
	}

	string get_message() const {
		return m_message;
	}

	int m_type;
	unsigned    int m_peerId;
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
	map<asUINT, _ENetPeer*> peers;
	_ENetPeer* get_peer(asUINT);
	network_type type;
	asUINT connect(const string& host, int port);
	bool destroy();
	bool disconnect_peer(asUINT);
	bool disconnect_peer_forcefully(asUINT);
	bool disconnect_peer_softly(asUINT);
	string get_peer_address(asUINT);
	double get_peer_average_round_trip_time(asUINT);
	CScriptArray* get_peer_list();
	network_event* request(int timeout = 0);
	bool send_reliable(asUINT peerId, const string& packet, int channel);
	bool send_unreliable(asUINT peerId, const string& packet, int channel);
	bool set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth);
	bool setup_client(int channels, int maxPeers);
	bool setup_server(int listeningPort, int channels, int maxPeers);
	void flush();
	int get_connected_peers() const;

	double get_bytes_sent() const;

	double get_bytes_received() const;

	bool is_active() const;

private:
	ENetAddress address;
	ENetHost* host;
	asUINT current_peer_id = 1;
	int m_connectedPeers;
	double m_bytesSent;
	double m_bytesReceived;
	bool m_active;
};
typedef struct {
	ffi_type* returnType;
	std::vector<ffi_type*> parameters;
	std::vector<std::string> parameterTypes; // For pointers handling.
}LibraryFunction;
class library : public as_class {
public:
	void* lib;
	std::map<std::string, LibraryFunction> functions;
	bool active()const;
	bool load(const string&);
	void unload();
};
void library_call(asIScriptGeneric* gen);
class script_thread : public as_class {
private:
	asIScriptContext* context;
	asIScriptFunction* function;
	thread* th;
public:
	script_thread(asIScriptFunction* func);
	void join();
	void detach();
	void destroy();
};

class instance : public as_class {
private:
	Poco::NamedMutex* mutex = nullptr;
public:
	instance(const string& application_name) {
		try {
			mutex = new Poco::NamedMutex(application_name);
			mutex->tryLock();
		}
		catch (std::exception& e) {}
	}
	bool is_running();
	~instance() {
		mutex->unlock();
		delete mutex;
		mutex = nullptr;
	}
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

struct DeviceButton
{
	bool isDown = false;
	bool isPressed = false;
	bool isReleased = false;
};
#endif