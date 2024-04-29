#pragma once
#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
#include "Poco/URI.h"
#include "Poco/URIStreamOpener.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Base64Decoder.h"
#include "Poco/HexBinaryEncoder.h"
#include "Poco/HexBinaryDecoder.h"
#include "Poco/String.h"
#include "Poco/NumberFormatter.h"
#include"tts_voice.h"
using namespace Poco;
using namespace Poco::Net;
#include "dyn/dyncall.h"
#include "dyn/dynload.h"
#include <Windows.h>
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
#include"sdl/SDL.h"
#include <iostream>
#include <vector>
#include "scriptdictionary/scriptdictionary.h"
class ngtvector {
public:
	void construct();
	void destruct();
	float x, y, z;
	float get_length()const;
	ngtvector& operator=(const ngtvector);
};


std::wstring wstr(const std::string & utf8String);
uint64_t get_time_stamp_millis();
uint64_t get_time_stamp_seconds();
void as_printf(asIScriptGeneric* gen);

void init_engine();
void set_library_path(const std::string& path);
long random(long min, long max);
 double randomDouble(double min, double max);
 bool random_bool();
 int get_last_error();
void speak(const std::string &	 text, bool stop = true);
void speak_wait(const std::string &	 text, bool stop = true);
void stop_speech();
std::string screen_reader_detect();
void show_console();
void hide_console();
bool show_game_window(const std::string & title,int width=640, int height=480, bool closable=true);
bool focus_game_window();
void hide_game_window();
void set_game_window_title(const std::string & );
void set_game_window_closable(bool);
bool is_game_window_active();
void update_game_window();
void exit_engine(int=0);
bool clipboard_copy_text(const std::string&);
std::string clipboard_read_text();
std::string get_input();
bool key_pressed(SDL_Scancode);
bool key_released(SDL_Scancode);
bool key_down(SDL_Scancode);
bool key_repeat(SDL_Scancode);
SDL_Keycode* keys_pressed();
SDL_Keycode* keys_released();
SDL_Keycode* keys_down();
SDL_Keycode* keys_repeat();
std::string string_encrypt(std::string the_string, std::string encryption_key);
std::string string_decrypt(std::string the_string, std::string encryption_key);
std::string url_decode(const std::string& url);
std::string url_encode(const std::string& url);
std::string url_get(const std::string& url);
std::string url_post(const std::string& url, const std::string& parameters);
std::string ascii_to_character(int the_ascii_code);
int character_to_ascii(std::string the_character);
std::string hex_to_string(std::string the_hexadecimal_sequence);
std::string number_to_hex_string(double the_number);
std::string string_base64_decode(std::string base64_string);
std::string string_base64_encode(std::string the_string);
std::string string_to_hex(std::string the_string);
bool alert(const std::string &	 title, const std::string &	 text, const std::string &button_name="OK");
int question(const std::string& title, const std::string &text);
void wait(int);
void delay(int);
std::string read_environment_variable(const std::string&);
std::string serialize(CScriptDictionary* the_data);
CScriptDictionary* deserialize(const std::string &serialized_data);

class timer {
public:

	std::chrono::time_point<std::chrono::steady_clock> inittime;
	uint64_t pausedNanos = 0;

	timer() {
		inittime = std::chrono::steady_clock::now();
		pausedNanos = 0;
	}
	void construct();
	void destruct();
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
class network_event {
public:
    const int EVENT_NONE = ENET_EVENT_TYPE_NONE;
    const int EVENT_CONNECT = ENET_EVENT_TYPE_CONNECT;
    const int EVENT_RECEIVE = ENET_EVENT_TYPE_RECEIVE;
    const int EVENT_DISCONNECT = ENET_EVENT_TYPE_DISCONNECT;
	void construct();
	void destruct();
    int get_type() const {
        return m_type;
    }

unsigned    int get_peer_id() const {
        return m_peerId;
    }

unsigned    int get_channel() const {
        return m_channel;
    }

    std::string get_message() const {
        return m_message;
    }

    int m_type;
unsigned    int m_peerId;
    int m_channel;
    std::string m_message;
};

class network {
public:
	void construct();
	void destruct();
	unsigned    int connect(const std::string& host, int port);
	bool destroy();
	bool disconnect_peer(unsigned int);
	bool disconnect_peer_forcefully(unsigned int);
	bool disconnect_peer_softly(unsigned int);
	std::string get_peer_address(unsigned int);
	double get_peer_average_round_trip_time(unsigned int);
	CScriptArray* get_peer_list();
	network_event* request();
	bool send_reliable(unsigned int peerId, const std::string& packet, int channel);
	bool send_unreliable(unsigned int peerId, const std::string& packet, int channel);
	bool set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth);
	bool setup_client(int channels, int maxPeers);
	bool setup_server(int listeningPort, int channels, int maxPeers);

	int get_connected_peers() const;

	double get_bytes_sent() const;

	double get_bytes_received() const;

	bool is_active() const;

private:
	ENetAddress address;
	ENetHost* host;
	unsigned int current_peer_id;
	int m_connectedPeers;
	double m_bytesSent;
	double m_bytesReceived;
	bool m_active;
};
class library {
public:
	HMODULE lib;
	void construct();
	void destruct();
	bool load(const std::string &);
	void call(asIScriptGeneric* gen);
		void unload();
};

class instance {
private:
	HANDLE mutex;
public:
	instance(const std::string & application_name) {
		mutex = CreateMutexA(NULL, TRUE, application_name.c_str());
	}
	void construct();
	void destruct();
	bool is_running();
	~instance() {
		CloseHandle(mutex);
	}
};
class user_idle {
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
class sqlite3statement
{
public:
	void construct();
	void destruct();
	int step();
	int reset();
	std::string get_expanded_sql_statement()const;
	std::string get_sql_statement() const;
	int get_bind_param_count()const;
	int get_column_count()const;
	int bind_blob(int index, const std::string& value, bool copy = true);
	int bind_double(int index, double value);
	int bind_int(int index, int value);
	int bind_int64(int index, int64_t value);
	int bind_null(int index);
	int bind_param_index(const std::string& name);
	std::string bind_param_name(int index);
	int bind_text(int index, const std::string& value, bool copy = true);
	int clear_bindings();
	std::string column_blob(int index);
	int column_bytes(int index);
	std::string column_decltype(int index);
	double column_double(int index);
	int column_int(int index);
	int64_t column_int64(int index);
	std::string column_name(int index);
	int column_type(int index);
	std::string column_text(int index);
	sqlite3_stmt* stmt;
};

using sqlite3_authorizer = int(*)(std::string, int, std::string, std::string, std::string, std::string);


class ngtsqlite3
{
public:
	void construct();
	void destruct();

	int close();
	int open(const std::string& filename, int flags = 6);
	sqlite3statement* prepare(const std::string& name, int& out);

	int execute(const std::string& sql);
	int64_t get_rows_changed() const;
	int64_t get_total_rows_changed()const;
	int limit(int id, int val);
	int set_authorizer(sqlite3_authorizer* callback, const std::string& arg = "");
	int64_t get_last_insert_rowid() const;
	void set_last_insert_rowid(int64_t id);
	int get_last_error();
	std::string get_last_error_text();
	bool get_active()const;
private:
	sqlite3* db;
};


