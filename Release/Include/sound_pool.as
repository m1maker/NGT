#include "effect"
class sound_pool_item
{
	float upper_range;
	float lower_range;
	float left_range;
	float right_range;
	float backward_range;
	float forward_range;
	float rotation;

	sound @sound_instance;

	sound_pool_item()
	{
		@sound_instance = sound(); // Create a new sound instance
	}
	~sound_pool_item()
	{
		reset();
	}
	void reset() {
		left_range = 0;
		right_range = 0;
		backward_range = 0;
		forward_range = 0;
		upper_range = 0;
		lower_range = 0;
		rotation = 0;
		if (sound_instance.active)
			sound_instance.close();
		@sound_instance = null;
		@sound_instance = sound();
	}

	void update_listener_position(float listener_x, float listener_y, float listener_z, float rotation) {
		vector @pos = sound_instance.get_source_position();
		if (pos is null)
			return;
		// Calculate boundaries
		float delta_left = pos.x - left_range;
		float delta_right = pos.x + right_range;
		float delta_backward = pos.y - backward_range;
		float delta_forward = pos.y + forward_range;
		float delta_upper = pos.z + upper_range;
		float delta_lower = pos.z - lower_range;
		float true_x = listener_x;
		float true_y = listener_y;
		float true_z = listener_z;
		if (listener_x < delta_left)
			true_x = delta_left;
		else if (listener_x > delta_right)
			true_x = delta_right;

		if (listener_y < delta_backward)
			true_y = delta_backward;
		else if (listener_y > delta_forward)
			true_y = delta_forward;

		if (listener_z < delta_lower)
			true_z = delta_lower;
		else if (listener_z > delta_upper)
			true_z = delta_upper;

		sound_instance.set_position(listener_x, listener_y, listener_z, true_x, true_y, true_z, rotation);
		this.rotation = rotation;
	}
	void update_position(float x, float y, float z){
		vector @pos = sound_instance.get_listener_position();
		if (pos is null)
			return;
		sound_instance.set_position(pos.x, pos.y, pos.z, x, y, z, rotation);
	}
	float get_total_distance(float listener_x, float listener_y, float listener_z) {
		vector @source_pos = sound_instance.get_source_position();

		float delta_left = source_pos.x - left_range;
		float delta_right = source_pos.x + right_range;
		float delta_backward = source_pos.y - backward_range;
		float delta_forward = source_pos.y + forward_range;
		float delta_upper = source_pos.z + upper_range;
		float delta_lower = source_pos.z - lower_range;

		if (listener_x < delta_left || listener_x > delta_right ||
			listener_y < delta_backward || listener_y > delta_forward ||
			listener_z < delta_lower || listener_z > delta_upper)
		{
			return 0;
		}

		return sqrt(pow(source_pos.x - listener_x, 2) +
					pow(source_pos.y - listener_y, 2) +
					pow(source_pos.z - listener_z, 2));
	}

	bool get_playing() const property {
		return sound_instance.playing;
	}
	bool get_active() const property{
		return sound_instance.active;
	}
	bool play(bool loop = false) {
		return loop ? sound_instance.play_looped() : sound_instance.play();
	}
};

class sound_pool
{
	int max_distance;
	protected array<sound_pool_item @> pool;
	protected int max_sounds;

	sound_pool(int _max_sounds = 100)
	{
		this.max_sounds = _max_sounds;
		pool.resize(max_sounds);

		// Initialize all sound pool items
		for (int i = 0; i < max_sounds; ++i)
		{
			@pool[i] = sound_pool_item();
		}
	}
	int play_stationary(const string& in filename, bool looping, bool memory = false, bool persistent = false) {
		return play_3d(filename, looping : looping, memory : memory, persistent : persistent);
	}

	int play_stationary(const string& in filename, bool looping, float offset, float start_pan, float start_volume, float start_pitch, bool memory = false, bool persistent = false) {
		return play_3d(filename, looping : looping, offset : offset, start_pan : start_pan, start_volume : start_volume, start_pitch : start_pitch, memory : memory, persistent : persistent);
	}

	int play_1d(const string& in filename, float listener_x, float sound_x, bool looping, bool memory = false, bool persistent = false) {
		return play_3d(filename, listener_x : listener_x, sound_x : sound_x, looping : looping, memory : memory, persistent : persistent);
	}

	int play_1d(const string& in filename, float listener_x, float sound_x,
				int left_range, int right_range,
				bool looping,
				float offset,
				float start_pan,
				float start_volume,
				float start_pitch,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename,
					   listener_x : listener_x,
									sound_x : sound_x,
											  left_range : left_range,
														   right_range : right_range,
																		 looping : looping,
																				   offset : offset,
																							start_pan : start_pan,
																										start_volume : start_volume,
																													   start_pitch : start_pitch,
																																	 memory : memory,
																																			  persistent : persistent);
	}

	int play_2d(const string& in filename,
				float listener_x,
				float listener_y,
				float sound_x,
				float sound_y,
				bool looping,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename,
					   listener_x : listener_x,
									listener_y : listener_y,
												 sound_x : sound_x,
														   sound_y : sound_y,
																	 looping : looping,
																			   memory : memory,
																						persistent : persistent);
	}

	int play_2d(const string& in filename,
				float listener_x,
				float listener_y,
				float sound_x,
				float sound_y,
				int left_range,
				int right_range,
				int backward_range,
				int forward_range,
				bool looping,
				float offset,
				float start_pan,
				float start_volume,
				float start_pitch,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename,
					   listener_x : listener_x,
									listener_y : listener_y,
												 sound_x : sound_x,
														   sound_y : sound_y,
																	 left_range : left_range,
																				  right_range : right_range,
																								backward_range : backward_range,
																												 forward_range : forward_range,
																																 looping : looping,
																																		   offset : offset,
																																					start_pan : start_pan,
																																								start_volume : start_volume,
																																											   start_pitch : start_pitch,
																																															 memory : memory,
																																																	  persistent : persistent);
	}

	int play_3d(const string& in filename,
				float listener_x,
				float listener_y,
				float listener_z,
				float sound_x,
				float sound_y,
				float sound_z,
				float rotation,
				bool looping,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename, listener_x, listener_y, listener_z, sound_x, sound_y, sound_z, rotation, 0, 0, 0, 0, 0, 0, looping, 0, 0, 0, 100, memory, persistent);
	}

	int play_3d(const string& in filename,
				vector @listener,
				vector @source,
				float rotation,
				bool looping,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename, listener.x, listener.y, listener.z, source.x, source.y, source.z, rotation, 0, 0, 0, 0, 0, 0, looping, 0, 0, 0, 100, memory, persistent);
	}

	int play_3d(
		const string&in filename,
		vector @listener,
		vector @source,

		float rotation = 0,
		int left_range = 0,
		int right_range = 0,
		int backward_range = 0,
		int forward_range = 0,
		int upper_range = 0,
		int lower_range = 0,
		bool looping = false,
		float offset = 0,
		float start_pan = 0,
		float start_volume = 0,
		float start_pitch = 0,
		bool memory = false,
		bool persistent = false)
	{
		return play_3d(
			filename,
			listener.x,
			listener.y,
			listener.z,
			source.x,
			source.y,
			source.z,

			rotation,
			left_range,
			right_range,
			backward_range,
			forward_range,
			upper_range,
			lower_range,
			looping,
			offset,
			start_pan,
			start_volume,
			start_pitch,
			memory,
			persistent);
	}

	int play_3d(
		const string&in filename,
		float listener_x = 0,
		float listener_y = 0,
		float listener_z = 0,
		float sound_x = 0,
		float sound_y = 0,
		float sound_z = 0,
		float rotation = 0,
		int left_range = 0,
		int right_range = 0,
		int backward_range = 0,
		int forward_range = 0,
		int upper_range = 0,
		int lower_range = 0,
		bool looping = false,
		float offset = 0,
		float start_pan = 0,
		float start_volume = 0,
		float start_pitch = 0,
		bool memory = false,
		bool persistent = false)
	{

		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Set initial properties
		pool[i].sound_instance.set_pan(start_pan);
		pool[i].sound_instance.set_volume(start_volume);
		pool[i].sound_instance.set_pitch(start_pitch);

		// Set position in 3D space

		pool[i].left_range = left_range;
		pool[i].right_range = right_range;
		pool[i].backward_range = backward_range;
		pool[i].forward_range = forward_range;
		pool[i].lower_range = lower_range;
		pool[i].upper_range = upper_range;

		pool[i].play(looping);
		if (offset > 0)
			pool[i].sound_instance.seek(offset);
		pool[i].update_listener_position(listener_x, listener_y, listener_z, rotation);

		return i; // Return index
	}
	void update_listener_position(float listener_x, float listener_y, float listener_z, float rotation = 0) {
		for (int i = 0; i < max_sounds; ++i)
		{
			if (pool[i].active && pool[i].playing)
			{
				pool[i].update_listener_position(listener_x, listener_y, listener_z, rotation);
			}
			else
			{
				pool[i].reset(); // Free unused sounds
			}
		}
	}
	void pause_all()
	{
		for (int i = 0; i < max_sounds; ++i)
		{
			if (pool[i].active && pool[i].playing)
			{
				pool[i].sound_instance.pause();
			}
		}
	}

	void resume_all()
	{
		for (int i = 0; i < max_sounds; ++i)
		{
			if (pool[i].active && !pool[i].playing)
			{
				this.resume_sound(i);
			}
		}
	}

	bool sound_active(int slot)
	{
		if (slot < 0 || slot >= max_sounds)
			return false;
		return pool[slot].active;
	}

	bool sound_is_playing(int slot)
	{
		if (slot < 0 || slot >= max_sounds)
			return false;
		return pool[slot].playing;
	}

	bool pause_sound(int slot)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;
		pool[slot].sound_instance.pause();
		return true;
	}

	void destroy_all()
	{
		for (int i = 0; i < max_sounds; ++i)
		{
			pool[i].reset();
		}
	}

	bool resume_sound(int slot)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;
		pool[slot].play(pool[slot].sound_instance.looping);
		return true;
	}

	bool update_sound_position(int slot, int x = 0, int y = 0, int z = 0)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;
		pool[slot].update_position(x, y, z);
		return true;
	}

	bool update_sound_start_values(int slot, float start_pan, float start_volume, float start_pitch)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;

		pool[slot].sound_instance.set_pan(start_pan);
		pool[slot].sound_instance.set_volume(start_volume);
		pool[slot].sound_instance.set_pitch(start_pitch);
		return true;
	}

	bool destroy_sound(int slot)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;
		pool[slot].reset();
		return true;
	}

	bool update_sound_range(int slot, int left_range = 0, int right_range = 0, int backward_range = 0, int forward_range = 0, int upper_range = 0, int lower_range = 0)
	{
		if (slot < 0 || slot >= max_sounds || !pool[slot].active)
			return false;
		pool[slot].left_range = left_range;
		pool[slot].right_range = right_range;
		pool[slot].backward_range = backward_range;
		pool[slot].forward_range = forward_range;
		pool[slot].lower_range = lower_range;
		pool[slot].upper_range = upper_range;

		return true;
	}
	protected int get_free_sound_id() {
		int it;
		bool found = false;
		for (it = 0; it < pool.length(); it++)
		{
			if (pool[it].active && !pool[it].playing)
			{
				pool[it].reset();
				found = true;
				break;
			}
			else if (!pool[it].active) {
				found = true;
				break;
			}
		}
		if (found)
			return it;
		return -1;
	}
};
