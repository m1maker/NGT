#include "hip.as" // clamp

string pack_audio(array<float>@ data)
{
	string result;
	for (uint i = 0; i < data.length(); ++i)
	{
		result += float_to_bytes(data[i]);
	}
	return result;
}

array<float>@ unpack_audio(const string &in data)
{
	array<float> result;

	uint numFloats = data.length() / sizeof(0.0f);
	result.resize(numFloats);

	for (uint i = 0; i < numFloats; ++i)
	{
		string floatBytes = data.substr(i * sizeof(0.0f), sizeof(0.0f));
		result[i] = bytes_to_float(floatBytes);
	}

	return result;
}


array<float>@ mix_audio(array<array<float>>@ audioInputs, bool preventClipping = true)
{
	uint maxLength = 0;
	for (uint i = 0; i < audioInputs.length(); ++i)
	{
		if (audioInputs[i].length() > maxLength)
		{
			maxLength = audioInputs[i].length();
		}
	}

	array<float> mixedAudio;
	mixedAudio.resize(maxLength);

	for (uint i = 0; i < maxLength; ++i)
	{
		float combinedSample = 0.0f;

		for (uint j = 0; j < audioInputs.length(); ++j)
		{
			if (i < audioInputs[j].length())
			{
				combinedSample += audioInputs[j][i];
			}
		}

		mixedAudio[i] = combinedSample;

		if (preventClipping)
		{
			if (mixedAudio[i] > 1.0f) mixedAudio[i] = 1.0f;
			else if (mixedAudio[i] < -1.0f) mixedAudio[i] = -1.0f;
		}
	}

	return mixedAudio;
}



array<float>@ generate_sine(int start_freq, int end_freq, float bend_duration, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float bend_samples = bend_duration * sample_rate;
	float freq_change_per_sample = float(end_freq - start_freq) / bend_samples;
	float current_freq = start_freq;

	for (int i = 0; i < samples; ++i) {
		if (i < bend_samples) {
			current_freq = start_freq + (freq_change_per_sample * i); // Linear frequency bend
		} else {
			current_freq = end_freq;
		}

		float increment = (2.0 * 3.14159265 * current_freq) / sample_rate;
		float sample = sin(increment * i);

		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}

array<float>@ generate_square(int start_freq, int end_freq, float bend_duration, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float bend_samples = bend_duration * sample_rate;
	float freq_change_per_sample = float(end_freq - start_freq) / bend_samples;
	float current_freq = start_freq;

	for (int i = 0; i < samples; ++i) {
		if (i < bend_samples) {
			current_freq = start_freq + (freq_change_per_sample * i); // Linear frequency bend
		} else {
			current_freq = end_freq;
		}

		float period = sample_rate / current_freq;
		float sample = (i % period < period / 2) ? 1.0 : -1.0;

		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}

array<float>@ generate_sawtooth(int start_freq, int end_freq, float bend_duration, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float bend_samples = bend_duration * sample_rate;
	float freq_change_per_sample = float(end_freq - start_freq) / bend_samples;
	float current_freq = start_freq;

	for (int i = 0; i < samples; ++i) {
		if (i < bend_samples) {
			current_freq = start_freq + (freq_change_per_sample * i); // Linear frequency bend
		} else {
			current_freq = end_freq;
		}

		float increment = (2.0 / sample_rate) * current_freq;
		float sample = (2.0 * (i * increment - floor(0.5 + i * increment))) - 1.0;

		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}

array<float>@ generate_triangle(int start_freq, int end_freq, float bend_duration, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float bend_samples = bend_duration * sample_rate;
	float freq_change_per_sample = float(end_freq - start_freq) / bend_samples;
	float current_freq = start_freq;

	for (int i = 0; i < samples; ++i) {
		if (i < bend_samples) {
			current_freq = start_freq + (freq_change_per_sample * i); // Linear frequency bend
		} else {
			current_freq = end_freq;
		}

		float increment = (2.0 / sample_rate) * current_freq;
		float sample = 2.0 * abs(2.0 * ((i * increment) - floor((i * increment) + 0.5))) - 1.0;

		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}



array<float>@ generate_noise(int channels, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	for (int i = 0; i < samples; ++i) {
		float sample = random(0, 1);
		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}
	return waveform;
}

array<float>@ generate_silence(int channels, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	for (int i = 0; i < samples * channels; ++i) {
		waveform[i] = 0.0f; // Silence
	}
	return waveform;
}

array<float>@ apply_envelope(const array<float>@ audio, float attack, float decay, float sustain, float release, int sample_rate) {
	array<float> @envelopedAudio = array<float>(audio.length());
	int totalSamples = audio.length();
	int attackSamples = int(sample_rate * attack);
	int decaySamples = int(sample_rate * decay);
	int releaseSamples = int(sample_rate * release);
	
	// Attack phase
	for (int i = 0; i < attackSamples && i < totalSamples; ++i) {
		envelopedAudio[i] = audio[i] * (i / float(attackSamples));
	}

	// Decay phase
	for (int i = 0; i < decaySamples && (attackSamples + i) < totalSamples; ++i) {
		envelopedAudio[attackSamples + i] = audio[attackSamples + i] * (1.0f - (i / float(decaySamples))) + sustain * (i / float(decaySamples));
	}

	// Sustain phase
	for (int i = 0; i < (totalSamples - attackSamples - decaySamples) && (attackSamples + decaySamples + i) < totalSamples; ++i) {
		envelopedAudio[attackSamples + decaySamples + i] = audio[attackSamples + decaySamples + i] * sustain;
	}

	// Release phase
	for (int i = 0; i < releaseSamples && (totalSamples - releaseSamples + i) < totalSamples; ++i) {
		envelopedAudio[totalSamples - releaseSamples + i] = audio[totalSamples - releaseSamples + i] * (1.0f - (i / float(releaseSamples)));
	}

	return envelopedAudio;
}

array<float>@ invert_phase(const array<float>@ audio) {
	array<float> @invertedAudio = array<float>(audio.length());
	
	for (uint i = 0; i < audio.length(); ++i) {
		invertedAudio[i] = -audio[i];
	}
	
	return invertedAudio;
}

array<float>@ apply_limiter(array<float>@ audio) {
	for (uint i = 0; i < audio.length(); ++i) {
		if (audio[i] > 1.0f) audio[i] = 1.0f;
		else if (audio[i] < -1.0f) audio[i] = -1.0f;
	}
	return audio;
}

array<float>@ clone_audio(const array<float>@ audio) {
	array<float> @clone = array<float>(audio.length());
	for (uint i = 0; i < audio.length(); ++i) {
		clone[i] = audio[i];
	}
	return clone;
}

array<float>@ pan_audio(const array<float>@ audio, float pan, int channels) {
	if (channels <= 0) return null;

	int numSamples = audio.length() / channels;
	array<float> @pannedAudio = array<float>(audio.length());

	pan = clamp(pan, -1.0f, 1.0f); // Clamp the pan between -1 and 1.

	if (channels == 1) {
		// Mono input, duplicate to stereo.
		for (int i = 0; i < numSamples; ++i)
		{
			pannedAudio[i * 2] = audio[i] * (1.0f - pan);
			pannedAudio[i * 2 + 1] = audio[i] * (1.0f + pan);
		}

		return pannedAudio;
	} else if (channels == 2) {
		// Stereo input.
		float leftGain = 0.5f * (1.0f - pan); // Avoids making it loud when panning.
		float rightGain = 0.5f * (1.0f + pan);
		for (int i = 0; i < numSamples; ++i)
		{
			pannedAudio[i * 2] = audio[i * 2] * leftGain;
			pannedAudio[i * 2 + 1] = audio[i * 2 + 1] * rightGain;
		}

		return pannedAudio;
	}

	// For anything else, just return a clone.
	return clone_audio(audio);
}

array<float>@ to_stereo(const array<float>@ monoAudio) {
	if (monoAudio is null) return null;

	int numSamples = monoAudio.length();
	array<float> @stereoAudio = array<float>(numSamples * 2);

	for (int i = 0; i < numSamples; ++i) {
		stereoAudio[i * 2] = monoAudio[i];	 // Left channel
		stereoAudio[i * 2 + 1] = monoAudio[i]; // Right channel
	}

	return stereoAudio;
}

array<float>@ to_mono(const array<float>@ stereoAudio) {
	if (stereoAudio is null || stereoAudio.length() % 2 != 0) return null;

	int numSamples = stereoAudio.length() / 2;
	array<float> @monoAudio = array<float>(numSamples);

	for (int i = 0; i < numSamples; ++i) {
		monoAudio[i] = (stereoAudio[i * 2] + stereoAudio[i * 2 + 1]) / 2.0f; // Average left and right channels
	}

	return monoAudio;
}
