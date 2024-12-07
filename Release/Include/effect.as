/* Copyright (c) 2024 Ivan Soto (Original creater)
Updated by M_maker || M1Maker
*/

#include "hip.as" // in_range

class effect
{
	string type;
	float upper_range;
	float lower_range;
	float left_range;
	float right_range;
	float backward_range;
	float forward_range;
	protected array<sound @> attached_handles;
	protected array<bool> muted;
	effect(string&in type)
	{
		this.type = type;
		left_range = 0;
		right_range = 0;
		backward_range = 0;
		forward_range = 0;
		upper_range = 0;
		lower_range = 0;
		attached_handles.resize(0);
		muted.resize(0);
	}
	effect()
	{
		for (uint i = 0; i < attached_handles.length(); ++i)
		{
			if (is_safe(attached_handles[i]))
				this.detach(attached_handles[i]);
		}
		muted.resize(0);
	}
	bool is_safe(sound @handle) {
		if (handle is null)
			return false;
		if (!handle.active)
			return false;
		return true;
	}
	bool attach(sound @handle) {
		if (!this.is_safe(handle))
			return false;
		handle.set_fx(this.type);
		bool need_insert = true;
		for (uint i = 0; i < attached_handles.length(); ++i)
		{
			if (attached_handles[i] is handle)
			{
				need_insert = false;
				break;
			}
		}

		if (need_insert)
			attached_handles.insert_last(handle);
		muted.resize(attached_handles.length());
		return this.configure_effect(handle);
	}
	bool detach(sound @handle, bool complete = true) {
		if (!this.is_safe(handle))
			return false;
		handle.delete_fx(this.type);
		if (complete)
		{
			for (uint i = 0; i < attached_handles.length(); ++i)
			{
				if (attached_handles[i] is handle)
				{
					attached_handles.remove_at(i);
					break;
				}
			}
		}
		muted.resize(attached_handles.length());
		return true;
	}

	bool configure_effect(sound @handle) {
		return false;
	}
	void ranges_monitor()
	{
		if (left_range > 0 || right_range > 0 || backward_range > 0 || forward_range > 0 || upper_range > 0 || lower_range > 0)
		{
			for (uint i = 0; i < attached_handles.length(); ++i)
			{
				if (!is_safe(attached_handles[i]))
					continue;

				vector @pos = attached_handles[i].get_listener_position();

				// Check ranges without using in_range function
				bool in_x_range = (pos.x >= -left_range && pos.x <= right_range);
				bool in_y_range = (pos.y >= -backward_range && pos.y <= forward_range);
				bool in_z_range = (pos.z >= lower_range && pos.z <= upper_range);

				if (!(in_x_range && in_y_range && in_z_range) && !muted[i])
				{
					this.detach(attached_handles[i], false);
					muted[i] = true;
				}
				else
				{
					if (muted[i])
					{
						this.attach(attached_handles[i]);
						muted[i] = false;
					}
				}
			}
		}
	}
};
//Effects

class delay_node : effect
{
	float dry, wet, decay;
	delay_node(float dry, float wet, float decay)
	{
		super("delay");
		this.dry = dry;
		this.wet = wet;
		this.decay = decay;
	}
	delay_node()
	{
		super("delay");
		this.dry = 0.1;
		this.wet = 0.1;
		this.decay = 0.1;
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
	bool set_decay(float value) {
		if (in_range(value, 0.1, 1.0))
		{
			this.decay = value;
			return true;
		}
		return false;
	}
	bool configure_effect(sound @handle) override {
		handle.set_delay_parameters(this.dry, this.wet, this.decay);
		return true;
	}
};

class lowpass : effect
{
	lowpass()
	{
		super("lowpass");
	}
};


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
