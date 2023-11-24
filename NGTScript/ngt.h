#pragma once
#ifndef BGT_H
#define BGT_H


#include <random>
#include <type_traits>

#include<chrono>
#include <string>
#include "bass.h"
#include <Windows.h>
void sound_system_init();
float random(long min, long max);

void speak(std::wstring text, bool stop = true);
void stop_speech();
bool show_game_window(std::wstring title);
void update_game_window();
bool key_pressed(int key_code);
	bool key_released(int key_code);
bool key_down(int key_code);
bool alert(std::wstring title, std::wstring text);
void set_listener_position(float l_x, float l_y, float l_z);
void wait(int time);
class sound {
public:
	HSTREAM handle_;

	bool load(std::wstring filename, bool hrtf=false);
	bool load_looped(std::wstring filename, bool hrtf = false);

	bool play();
//	bool play_looped();
	bool pause();
	bool play_wait();
	bool stop();
	bool close();
	void set_sound_position(float s_x, float s_y, float s_z);
	float get_pan() const;
	void set_pan(float pan);
	float get_volume() const;
	void set_volume(float volume);
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
	int paused;

	timer() {
		inittime = std::chrono::steady_clock::now();
		paused = 0;
	}

	int elapsed();
//	void elapsed(int amount);
	void restart();
	void pause();
	void resume();
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
#endif