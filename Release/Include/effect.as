/* Copyright (c) 2024 Ivan Soto (Original creater)
Updated by M_maker || M1Maker
*/

#include "hip" // in_range

class effect
{
	string type;
	effect(string&in type)
	{
		this.type = type;
	}
	effect()
	{
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
		return this.configure_effect(handle);
	}
	bool detach(sound @handle) {
		if (!this.is_safe(handle))
			return false;
		handle.delete_fx(this.type);
		return true;
	}

	bool configure_effect(sound @handle) {
		return false;
	}
};
