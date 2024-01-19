#pragma once
#define MINIAUDIO_IMPLEMENTATION


#include "miniaudio.h"
#include <random>
#include <type_traits>
#include<chrono>
#include <string>
#include"sdl/SDL.h"
#include "SDL_net.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "scriptdictionary/scriptdictionary.h"

std::wstring wstr(const std::string& utf8String);

uint64_t get_time_stamp_millis();
uint64_t get_time_stamp_seconds();
void init_engine();
 long random(long min, long max);
 double randomDouble(double min, double max);
 int get_last_error();
void speak(std::string	 text, bool stop = true);
void speak_wait(std::string	 text, bool stop = true);
void stop_speech();
bool show_game_window(std::string title,int width=640, int height=480, bool closable=true);

void hide_game_window();
void set_game_window_title(std::string new_title);
void update_game_window();
void quit();
bool clipboard_copy_text(std::string text);
std::string clipboard_read_text();
std::string get_input();
bool key_pressed(SDL_Keycode key_code);
bool key_released(SDL_Keycode key_code);
bool key_down(SDL_Keycode key_code);
bool key_repeat(SDL_Keycode key_code);
bool alert(std::string	 title, std::string	 text, unsigned int flag=0);
void set_listener_position(float l_x, float l_y, float l_z);
void wait(int time);
void delay(int ms);
void set_sound_storage(std::string path);
std::string get_sound_storage();
void set_master_volume(float volume);
float get_master_volume();
void switch_audio_system(short system);
std::string read_environment_variable(const std::string& path);
class reverb {
public:
	void construct();
	void destruct();


	void set_input_gain(float input_gain);
	void set_reverb_mix(float reverb_mix);
	void  set_reverb_time(float reverb_time);
	float get_input_gain();
	float get_reverb_mix();
	float  get_reverb_time();

};
class sound {
public:
	ma_sound handle_;
	bool playing=false, paused=false, active=false;
	void construct();
	void destruct();
	bool load(std::string	 filename, bool set3d=false);
	bool load_from_memory(std::string data, bool set3d = false);
	bool play();
	bool play_looped();
	bool pause();
	bool play_wait();
	bool stop();
	bool close();
	void set_sound_position(float s_x, float s_y, float s_z);
	void set_sound_reverb(float input_gain, float reverb_mix, float reverb_time);
	void cancel_reverb();
	double get_pan() const;
	void set_pan(double pan);
	double get_volume() const;
	void set_volume(double volume);
	double get_pitch() const;
	void set_pitch(double pitch);
	double get_pitch_lower_limit() const;
	bool is_active() const;
	bool is_playing() const;
	bool is_paused() const;
	double get_position() const;
	double get_length() const;
	double get_sample_rate() const;
	double get_channels() const;
	double get_bits() const;
};
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
	void force_seconds(uint64_t seconds);
	void force_minutes(uint64_t minutes);
	void force_hours(uint64_t hours);
	void force_millis(uint64_t millis);
	void force_micros(uint64_t micros);
	void force_nanos(uint64_t nanos);
	void restart();
	void pause();
	void resume();
	bool is_running();
};
class key_hold
{
	bool status;
	short key_flag = 0;
	int key_code;
	int repeat_time;
	int setting_1;
	int setting_2;
	timer key_timer;

public:
	key_hold(int _key_code, int _setting_1, int _setting_2)
	{
		key_code = _key_code;
		setting_1 = _setting_1;
		setting_2 = _setting_2;
		repeat_time = setting_1;
	}

	bool pressing();
};
class network_event {
public:
    const int EVENT_NONE = 0;
    const int EVENT_CONNECT = 1;
    const int EVENT_RECEIVE = 2;
    const int EVENT_DISCONNECT = 3;
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
	unsigned    int connect(std::string host, int port);
    bool destroy();
    bool disconnect_peer(unsigned int peerId);
    bool disconnect_peer_forcefully(unsigned int peerId);
    bool disconnect_peer_softly(unsigned int peerId);
    std::string get_peer_address(unsigned int peerId);
    double get_peer_average_round_trip_time(unsigned int peerId);
    std	::vector<unsigned int> get_peer_list();
    network_event* request();
    bool send_reliable(unsigned int peerId, std::string packet, int channel);
    bool send_unreliable(unsigned int peerId, std::string packet, int channel);
    bool set_bandwidth_limits(double incomingBandwidth, double outgoingBandwidth);
    bool setup_client(int channels, int maxPeers);
    bool setup_server(int listeningPort, int channels, int maxPeers);

	int get_connected_peers() const;

	double get_bytes_sent() const;

	double get_bytes_received() const;

	bool is_active() const;

private:
    int m_connectedPeers;
    double m_bytesSent;
    double m_bytesReceived;
    bool m_active;
	TCPsocket m_serverSocket;
	TCPsocket m_clientSocket;
	std::map<unsigned int, TCPsocket> m_serverSockets;

	SDLNet_SocketSet m_socketSet;
};


class library {
public:
	HMODULE lib;
	void construct();
	void destruct();
	bool load(std::string libname);
	CScriptDictionary* call(std::string function_name, ...);
	void unload();
};
class instance {
private:
	HANDLE mutex;
public:
	instance(std::string application_name) {
		mutex = CreateMutexA(NULL, TRUE, application_name.c_str());
	}
	void construct();
	void destruct();
	bool is_running();
	~instance() {
		CloseHandle(mutex);
	}
};
class ngtvector {
public:
	float x, y, z;
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
