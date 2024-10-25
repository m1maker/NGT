/* Copyright (c) 2024 Ivan Soto (Original creater)
Updated by M_maker || M1Maker
*/

class reverb : effect
{
	float dry, wet, room_size, damping, mode;
	reverb(float dry, float wet, float room_size, float damping, float mode)
	{
		super("reverb");
		this.dry = dry;
		this.wet = wet;
		this.room_size = room_size;
		this.damping = damping;
		this.mode = mode;
	}
	reverb()
	{
		super("reverb");
	}
	bool set_dry(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.dry = value;
			return true;
		}
		return false;
	}
	bool set_wet(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.wet = value;
			return true;
		}
		return false;
	}
	bool set_room_size(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.room_size = value;
			return true;
		}
		return false;
	}
	bool set_damping(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.damping = value;
			return true;
		}
		return false;
	}
	bool set_mode(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.mode = value;
			return true;
		}
		return false;
	}
	bool configure_effect(sound @handle) {
		handle.set_reverb_parameters(this.dry, this.wet, this.room_size, this.damping, this.mode);
		return true;
	}
};
