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
		if (this.in_range(value, 0.1, 1.0))
		{
			this.dry = value;
			return true;
		}
		alert("error", "Value out of range. " + value);
		return false;
	}
	bool set_wet(float value) {
		if (this.in_range(value, 0.1, 1.0))
		{
			this.wet = value;
			return true;
		}
		alert("error", "Value out of range. " + value);
		return false;
	}
	bool set_room_size(float value) {
		if (this.in_range(value, 0.1, 1.0))
		{
			this.room_size = value;
			return true;
		}
		alert("error", "Value out of range. " + value);
		return false;
	}
	bool set_damping(float value) {
		if (this.in_range(value, 0.1, 1.0))
		{
			this.damping = value;
			return true;
		}
		alert("error", "Value out of range. " + value);
		return false;
	}
	bool set_mode(float value) {
		if (this.in_range(value, 0.1, 1.0))
		{
			this.mode = value;
			return true;
		}
		alert("error", "Value out of range. " + value);
		return false;
	}
	bool config_from_preset(reverb_preset @preset) {
		if (@preset == null)
			return false;
		bool[] results(0);
		results.insert_last(this.set_dry(preset.dry));
		results.insert_last(this.set_wet(preset.wet));
		results.insert_last(this.set_room_size(preset.room_size));
		results.insert_last(this.set_damping(preset.damping));
		results.insert_last(this.set_mode(preset.mode));
		bool success = true;
		for (uint i = 0; i < results.length(); i += 1)
		{
			if (!results[i])
			{
				alert("error", "Couldn't configure reverb. Index is " + i);
				success = false;
				break;
			}
		}
		return success;
	}
	bool configure_effect(sound @handle) {
		handle.set_reverb_parameters(this.dry, this.wet, this.room_size, this.damping, this.mode);
		return true;
	}
}