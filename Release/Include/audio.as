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


array<float>@ mix_audio(const array<float>@ audio1, const array<float>@ audio2, bool avoid_clip = true)
{
	uint maxLength = audio1.length() > audio2.length() ? audio1.length() : audio2.length();
	array<float> mixedAudio;
	mixedAudio.resize(maxLength);

	for (uint i = 0; i < maxLength; ++i)
	{
		float sample1 = (i < audio1.length()) ? audio1[i] : 0.0f;
		float sample2 = (i < audio2.length()) ? audio2[i] : 0.0f;

		mixedAudio[i] = sample1 + sample2;
		if (avoid_clip)
		{
			if (mixedAudio[i] > 1.0f) mixedAudio[i] = 1.0f;
			else if (mixedAudio[i] < -1.0f) mixedAudio[i] = -1.0f;
		}
	}
	return mixedAudio;
}


array<float>@ generate_sine(int freq, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float increment = (2.0 * 3.14159265 * freq) / sample_rate;

	for (int i = 0; i < samples; ++i) {
		float sample = sin(increment * i);
		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}


array<float>@ generate_square(int freq, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float period = sample_rate / freq;

	for (int i = 0; i < samples; ++i) {
		float sample = (i % period < period / 2) ? 1.0 : -1.0;
		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}


array<float>@ generate_sawtooth(int freq, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float increment = (2.0 / sample_rate) * freq;

	for (int i = 0; i < samples; ++i) {
		float sample = (2.0 * (i * increment - floor(0.5 + i * increment))) - 1.0;
		for (int ch = 0; ch < channels; ++ch) {
			waveform[i * channels + ch] = sample;
		}
	}

	return waveform;
}


array<float>@ generate_triangle(int freq, int channels, int sample_rate, int samples) {
	array<float> @waveform = array<float>(samples * channels);
	float increment = (2.0 / sample_rate) * freq;

	for (int i = 0; i < samples; ++i) {
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
