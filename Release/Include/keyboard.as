class key_hold
{
	bool status;
	short key_flag = 0;
	int key_code;
	uint64 repeat_time;
	uint64 setting_1;
	uint64 setting_2;
	timer key_timer;
	uint64 wait_time;
	key_hold(const int&in _key_code, const uint64&in _setting_1, const uint64&in _setting_2)
	{
		key_code = _key_code;
		setting_1 = _setting_1;
		setting_2 = _setting_2;
		repeat_time = setting_1;
	}
	key_hold()
	{
		this.reset();
	}
	~key_hold()
	{
		this.reset();
	}
	bool pressing()
	{
		status = key_down(key_code);
		if (!status)
		{
			repeat_time = 0;
			key_timer.restart();
			key_flag = 0;
			return false;
		}
		int elapsed = key_timer.elapsed_millis;
		if (elapsed >= repeat_time || wait_time > 0 && (key_timer.elapsed_millis > wait_time))
		{
			switch (key_flag)
			{
				case 0:
					key_flag = 1;
					repeat_time = setting_1;
					key_timer.restart();
					break;
				case 1:
					repeat_time = setting_2;
					key_timer.restart();
					break;
			}
			return true;
		}
		return false;
	}
	void reset(){
		status = false;
		key_flag = 0;
		key_code = 0;
		repeat_time = 0;
		setting_1 = 0;
		setting_2 = 0;
		key_timer.restart();
		wait_time = 0;
	}
};
