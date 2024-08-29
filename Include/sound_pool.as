/*
Sound Pool (An alternative way to manage sounds).
*/

// Sound Pool handles registered in application
array<sound_pool @> g_sound_pools(0);

enum sp_result
{
	SP_SUCCESS = 0,
	SP_ALL_SOUNDS_BUSY = -1,
	SP_INVALID_ARGS = -2,
	SP_FILE_NOT_FOUND = -3,
	SP_PLAYING_ERROR = -4,
	SP_OUT_OF_MEMORY = -5
};
bool g_sound_pool_thread_running = false;
void sound_pool_thread(){
	while (g_sound_pool_thread_running)
	{
		wait(5);

		for (uint64 i = 0; i < g_sound_pools.length(); ++i)
		{
			if (g_sound_pools[i] is null)
				continue;
			for (uint64 i2 = 0; i2 < g_sound_pools[i].maximum_sounds; ++i2)
			{
				sound @handle = g_sound_pools[i].get_handle(i2);
				if (handle is null or !handle.active)
					continue;
				if (handle.volume > g_sound_pools[i].volume)
					handle.volume = g_sound_pools[i].volume;
			}
		}
	}
}
class sound_pool
{
	private array<sound @> sounds;
	uint64 window_handle;
	long free_time;
	timer free_timer;
	vector @listener = null;
	private array<audio_effect> effects;
	protected long max_sounds;
	private bool use_hrtf;
	private uint64 id;
	bool interrupt;
	sp_result last_error;
	float volume;
	sound_pool()
	{
		this.reset();
		g_sound_pools.insert_last(this);
		this.id = g_sound_pools.length() - 1;
		if (!g_sound_pool_thread_running)
		{
			g_sound_pool_thread_running = true;
			thread t(sound_pool_thread);
			t.detach();
		}
	}
	~sound_pool()
	{
		for (uint i = 0; i < sounds.length(); i++)
		{
			if (@sounds[i] != null && sounds[i].active)
			{
				sounds[i].close();
				@sounds[i] = null;
			}
		}
		this.reset();
		g_sound_pools.remove_at(this.id);
		if (g_sound_pools.length() == 0)
		{
			g_sound_pool_thread_running = false;
		}
	}
	void reset() {
		last_error = SP_SUCCESS;
		window_handle = 0;
		free_time = 3000;
		free_timer.restart();
		@listener = null;
		effects.resize(0);
		max_sounds = 100;
		sounds.resize(max_sounds);
		use_hrtf = true;
		this.interrupt = false;
		this.volume = 0.0f;
	}
	protected uint get_free_sound_id() {
		last_error = SP_SUCCESS;
		uint it;
		bool found = false;
		for (it = 0; it < sounds.length(); it++)
		{
			if (sounds[it] is null)
			{
				found = true;
				break;
			}
			else if (sounds[it].active && !(sounds[it].playing && !interrupt)) {
				sounds[it].close();
				found = true;
				break;
			}
			else if (!sounds[it].active) {
				found = true;
				break;
			}
		}
		if (found)
			return it;
		last_error = SP_ALL_SOUNDS_BUSY;
		return last_error;
	}
	void set_maximum_sounds(long count) property {
		max_sounds = count;
		sounds.resize(max_sounds);
	}
	long get_maximum_sounds() const property {
		return max_sounds;
	}
	int play(const string&in file, float listener_x = 0, float listener_y = 0, float listener_z = 0, float source_x = 0, float source_y = 0, float source_z = 0, double rotation = 0, int left_range = 0, int right_range = 0, int backward_range = 0, int forward_range = 0, int upper_range = 0, int lower_range = 0, bool loop = false, bool wait = false, double offset = 0, float start_pan = 0, float start_volume = 0, float start_pitch = 100.0f, bool memory = false, size_t memory_size = 0) {
		last_error = SP_SUCCESS;
		if (loop && wait)
		{
			last_error = SP_INVALID_ARGS;
			return last_error;
		}
		uint id = get_free_sound_id();
		if (id == SP_ALL_SOUNDS_BUSY)
			return last_error;
		if (@sounds[id] == null)
			@sounds[id] = sound();
		bool success = false;
		if (memory)
		{
			success = sounds[id].load_from_memory(file, memory_size);
		}
		else
		{
			success = sounds[id].load(file);
		}
		if (!success)
		{
			last_error = SP_FILE_NOT_FOUND;
			return last_error;
		}
		if (sounds[id].volume > this.volume)
			sounds[id].volume = this.volume;
		if (loop)
		{
			success = sounds[id].play_looped();
		}
		else if (wait) {
			success = sounds[id].play_wait();
		}
		else
		{
			success = sounds[id].play();
		}
		if (!success)
		{
			last_error = SP_PLAYING_ERROR;
			return last_error;
		}
		return id;
	}
	uint64 opImplConv()
	{
		return this.id;
	}
	sound @ const get_handle(uint64 slot)
	{
		if (slot > max_sounds)
		{
			last_error = SP_OUT_OF_MEMORY;
			return null;
		}
		return sounds[slot];
	}
};

shared class audio_effect
{
	string type;
	audio_effect(const string&in type)
	{
		this.type = type;
	}
	audio_effect()
	{
	}
	protected bool in_range(float value, float min, float max) {
		bool ir = true;
		if (value > max || value < min)
			ir = false;
		return ir;
	}
	bool is_safe(sound @handle) {
		if (@handle == null)
			return false;
		if (!handle.active)
			return false;
		return true;
	}
	protected bool attach(sound @handle) {
		if (!this.is_safe(handle))
			return false;
		handle.set_fx(this.type);
		return this.configure_effect(handle);
	}
	protected bool detach(sound @handle) {
		if (!this.is_safe(handle))
			return false;
		handle.delete_fx(this.type);
		return true;
	}
	bool configure_effect(sound @handle) {
		return false;
	}
};
