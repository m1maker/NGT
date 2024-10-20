/* Copyright (c) 2024 Ivan Soto (Original creater)
Updated by M_maker || M1Maker
*/

#include "hip" // in_range

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
