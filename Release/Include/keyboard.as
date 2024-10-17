funcdef void on_press_callback();
funcdef void on_release_callback();

class key_event
{
	keycode key_code;
	bool pressed;
	uint64 timestamp;

	key_event(keycode code, bool isPressed, uint64 time)
	{
		key_code = code;
		pressed = isPressed;
		timestamp = time;
	}
}

class key_hold
{
	bool status;
	short key_flag = 0;
	keycode key_code;
	uint64 repeat_time;
	uint64 setting_1;
	uint64 setting_2;
	timer key_timer;
	protected uint64 wait_time;

	// Callback functions
	on_press_callback @on_press = null;
	on_release_callback @on_release = null;

	// Key state history
	array<key_event @> key_history;

	// Constructor with parameters
	key_hold(const keycode&in _key_code, const uint64&in _setting_1, const uint64&in _setting_2)
	{
		key_code = _key_code;
		setting_1 = _setting_1;
		setting_2 = _setting_2;
		repeat_time = setting_1;
	}

	// Default constructor
	key_hold()
	{
		reset();
	}

	~key_hold()
	{
		reset();
	}

	bool pressing()
	{
		status = key_down(key_code);

		if (!status)
		{
			if (key_flag > 0)	  // If was pressed before
			{
				if (on_release !is null)
					on_release(); // Trigger release callback

				key_flag = 0;	  // Reset flag
				log_event(false); // Log release event
			}
			repeat_time = 0;
			key_timer.restart();
			return false;
		}

		if (key_flag == 0 && on_press !is null)
		{
			on_press();		 // Trigger press callback
			key_flag = 1;	 // Set flag to indicate key is pressed
			log_event(true); // Log press event
		}

		int elapsed = key_timer.elapsed_millis;
		if (elapsed >= repeat_time || (wait_time > 0 && elapsed > wait_time))
		{
			switch (key_flag)
			{
				case 1:
					repeat_time = setting_1; // First repeat time
					break;
				case 2:
					repeat_time = setting_2; // Second repeat time
					break;
			}
			key_timer.restart();
			return true; // Key is still being held down
		}

		return false;	 // No new press detected
	}

	void reset()
	{
		status = false;
		key_flag = 0;
		key_code = KEY_UNKNOWN;
		repeat_time = 0;
		setting_1 = 0;
		setting_2 = 0;
		key_timer.restart();
		wait_time = 0;
		key_history.resize(0); // Clear history
	}

	void set_repeat_times(uint64 new_setting_1, uint64 new_setting_2)
	{
		setting_1 = new_setting_1;
		setting_2 = new_setting_2;
	}

	void set_wait_time(uint64 new_wait_time)
	{
		wait_time = new_wait_time; // Set dynamic wait time
	}

	private void log_event(bool pressed)
	{
		uint64 current_time = get_time_stamp_millis();
		key_history.insert_last(key_event(key_code, pressed, current_time));
	}
};
