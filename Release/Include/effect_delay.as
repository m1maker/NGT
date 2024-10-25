/* Copyright (c) 2024 Ivan Soto (Original creater)
Updated by M_maker || M1Maker
*/

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
		if (this.in_range(value, 0.1, 1.0))
		{
			this.dry = value;
			return true;
		}
		return false;
	}
	bool set_wet(float value) {
		if (this.in_range(value, 0.1, 1.0))
		{
			this.wet = value;
			return true;
		}
		return false;
	}
	bool set_decay(float value) {
		if (this.in_range(value, 0.1, 1.0))
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
