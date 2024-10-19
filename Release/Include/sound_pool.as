class sound_pool_item
{
	sound @sound_instance;

	sound_pool_item()
	{
		@sound_instance = sound(); // Create a new sound instance
	}
	~sound_pool_item()
	{
		sound_instance.close();
		@sound_instance = null;
	}
	void reset() {
		if (sound_instance.active)
			sound_instance.close();
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
	}

	void update_listener_position(float listener_x, float listener_y, float listener_z, float rotation) {
		sound_instance.set_position(listener_x, listener_y, listener_z, sound_instance.get_source_position().x, sound_instance.get_source_position().y, sound_instance.get_source_position().z, rotation);
	}

	float get_total_distance(float listener_x, float listener_y, float listener_z) {
		vector @source_pos = sound_instance.get_source_position();
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
		if (loop)
			return sound_instance.play_looped();
		else
			return sound_instance.play();
		return false;
	}
};

class sound_pool
{
	protected array<sound_pool_item @> pool;
	int max_sounds;

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
		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);
		pool[i].play();
		return i; // Return index
	}

	int play_stationary(string filename, bool looping, float offset, float start_pan, float start_volume, float start_pitch, bool memory = false, bool persistent = false) {
		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);
		pool[i].sound_instance.set_pan(start_pan);
		pool[i].sound_instance.set_volume(start_volume);
		pool[i].sound_instance.set_pitch(start_pitch);
		pool[i].sound_instance.seek(offset);
		pool[i].play();
		return i; // Return index
	}

	int play_1d(string filename, int listener_x, int sound_x, bool looping, bool memory = false, bool persistent = false) {
		int i = get_free_sound_id();
		if (i == -1)
			return -1;

		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Calculate pan based on position
		float pan = (sound_x - listener_x) / 100.0; // Assuming a range of -100 to 100
		pool[i].sound_instance.set_pan(pan);

		pool[i].play();
		return i; // Return index
	}

	int play_1d(string filename, int listener_x, int sound_x, int left_r, int right_r, bool looping, float offset, float start_pan, float start_volume, float start_pitch, bool memory = false, bool persistent = false) {
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

		pool[i].play();
		return i; // Return index
	}

	int play_2d(string filename, int listener_x, int listener_y, int sound_x, int sound_y, bool looping, bool memory = false, bool persistent = false) {
		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Calculate distance and pan based on positions
		float dx = (sound_x - listener_x);
		float dy = (sound_y - listener_y);
		float distance = sqrt(dx * dx + dy * dy);

		// Normalize for pan
		float pan = dx / (distance + 1e-6); // Add small value to avoid division by zero
		pool[i].sound_instance.set_pan(pan);

		pool[i].play();
		return i; // Return index
	}

	int play_2d(string filename, int listener_x, int listener_y,
				int sound_x, int sound_y,
				int left_r, int right_r,
				int backward_r,
				int forward_r,
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

		pool[i].play();
		return i; // Return index
	}

	int play_3d(string filename, float listener_x, float listener_y, float listener_z,
				float sound_x, float sound_y, float sound_z,
				float rotation,
				bool looping,
				bool memory = false,
				bool persistent = false)
	{

		int i = get_free_sound_id();
		if (i == -1)
			return -1;
		pool[i].sound_instance.load(filename);
		pool[i].sound_instance.set_looping(looping);

		// Set position in 3D space
		pool[i].sound_instance.set_position(listener_x, listener_y, listener_z, sound_x, sound_y, sound_z, rotation);

		pool[i].play();
		return i; // Return index
	}

	int play_3d(string filename, float listener_x, float listener_y, float listener_z,
				float sound_x, float sound_y, float sound_z,
				float rotation,
				int left_r,
				int right_r,
				int backward_r,
				int forward_r,
				int upper_r,
				int lower_r,
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

		pool[i].play();
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
