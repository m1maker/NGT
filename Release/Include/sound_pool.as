class sound_pool_item
{
	float upper_range;
	float lower_range;
	float left_range;
	float right_range;
	float backward_range;
	float forward_range;

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

		if (sound_instance.active)
			sound_instance.close();
		@sound_instance = null;
		@sound_instance = sound();
	}

	void update(float listener_x, float listener_y, float listener_z, float rotation = 0.0f, int max_distance = 0) {
		float distance = get_total_distance(listener_x, listener_y, listener_z);
		if (distance < max_distance)
		{
			float volume = 0 - (distance / max_distance);
			sound_instance.set_volume(volume);
			sound_instance.set_position(listener_x, listener_y, listener_z, sound_instance.get_source_position().x, sound_instance.get_source_position().y, sound_instance.get_source_position().z, rotation);
		}
		else
		{
			sound_instance.set_volume(-100.0); // Mute if out of range
		}
		update_listener_position(listener_x, listener_y, listener_z, rotation);
	}

	void update_listener_position(float listener_x, float listener_y, float listener_z, float rotation) {
		vector @pos = sound_instance.get_source_position();
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
	int play_stationary(const string&in filename, bool looping, bool memory = false, bool persistent = false) {
		return play_stationary(filename, looping, 0, 0, 0, 100.0, memory, persistent);
	}

	int play_stationary(const string&in filename, bool looping, float offset, float start_pan, float start_volume, float start_pitch, bool memory = false, bool persistent = false) {
		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);
		pool[i].sound_instance.set_pan(start_pan);
		pool[i].sound_instance.set_volume(start_volume);
		pool[i].sound_instance.set_pitch(start_pitch);
		pool[i].sound_instance.seek(offset);
		pool[i].play(looping);
		return i; // Return index
	}

	int play_1d(const string&in filename, int listener_x, int sound_x, bool looping, bool memory = false, bool persistent = false) {
		return play_1d(filename, listener_x, sound_x, 0, 0, looping, 0, 0, 0, 100, memory, persistent);
	}

	int play_1d(const string&in filename, int listener_x, int sound_x, int left_range, int right_range, bool looping, float offset, float start_pan, float start_volume, float start_pitch, bool memory = false, bool persistent = false) {
		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Set initial properties
		pool[i].sound_instance.set_pan(start_pan);
		pool[i].sound_instance.set_volume(start_volume);
		pool[i].sound_instance.set_pitch(start_pitch);

		// Calculate pan based on position
		float pan = (sound_x - listener_x) / 100.0; // Assuming a range of -100 to 100
		pool[i].sound_instance.set_pan(pan);
		pool[i].sound_instance.set_position(listener_x, 0, 0, sound_x, 0, 0);
		pool[i].left_range = left_range;
		pool[i].right_range = right_range;
		pool[i].play(looping);
		return i; // Return index
	}

	int play_2d(const string&in filename, int listener_x, int listener_y, int sound_x, int sound_y, bool looping, bool memory = false, bool persistent = false) {
		return play_2d(filename, listener_x, listener_y, sound_x, sound_y, 0, 0, 0, 0, looping, 0, 0, 0, 100, memory, persistent);
	}

	int play_2d(const string&in filename, int listener_x, int listener_y,
				int sound_x, int sound_y,
				int left_range, int right_range,
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

		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Set initial properties
		pool[i].sound_instance.set_pan(start_pan);
		pool[i].sound_instance.set_volume(start_volume);
		pool[i].sound_instance.set_pitch(start_pitch);

		// Calculate distance and pan based on positions
		float dx = (sound_x - listener_x);
		float dy = (sound_y - listener_y);

		float distance = sqrt(dx * dx + dy * dy);

		// Normalize for pan
		float pan = dx / (distance + 1e-6); // Add small value to avoid division by zero
		pool[i].sound_instance.set_pan(pan);
		pool[i].sound_instance.set_position(listener_x, listener_y, 0, sound_x, sound_y, 0);
		pool[i].left_range = left_range;
		pool[i].right_range = right_range;
		pool[i].backward_range = backward_range;
		pool[i].forward_range = forward_range;

		pool[i].play(looping);
		return i; // Return index
	}

	int play_3d(const string&in filename, float listener_x, float listener_y, float listener_z,
				float sound_x, float sound_y, float sound_z,
				float rotation,
				bool looping,
				bool memory = false,
				bool persistent = false)
	{
		return play_3d(filename, listener_x, listener_y, listener_z, sound_x, sound_y, sound_z, rotation, 0, 0, 0, 0, 0, 0, looping, 0, 0, 0, 100, memory, persistent);
	}

	int play_3d(const string&in filename, float listener_x, float listener_y, float listener_z,
				float sound_x, float sound_y, float sound_z,
				float rotation,
				int left_range,
				int right_range,
				int backward_range,
				int forward_range,
				int upper_range,
				int lower_range,
				bool looping,
				float offset,
				float start_pan, float start_volume,
				float start_pitch,
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
		pool[i].sound_instance.set_position(listener_x, listener_y, listener_z, sound_x, sound_y, sound_z, rotation);
		pool[i].left_range = left_range;
		pool[i].right_range = right_range;
		pool[i].backward_range = backward_range;
		pool[i].forward_range = forward_range;
		pool[i].lower_range = lower_range;
		pool[i].upper_range = upper_range;

		pool[i].play(looping);
		return i; // Return index
	}
	void update(float listener_x, float listener_y, float listener_z) {
		for (int i = 0; i < max_sounds; ++i)
		{
			if (pool[i].active && pool[i].playing)
			{
				pool[i].update(listener_x, listener_y, listener_z);
			}
			else
			{
				pool[i].reset(); // Free unused sounds
			}
		}
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
