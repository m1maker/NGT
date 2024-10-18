/**
 *  \file wave.as
 *  Simple wave file utilities for angelscript.
 *
 *  Created by Blue Cat Audio <services@bluecataudio.com>
 *  \copyright 2014-2016 Blue Cat Audio. All rights reserved.
 *
 *  Modified by M_maker || M1Maker for NGT engine.
 *
 *  Supports most 8/16/24/32/64-bit wave files but probably not all.
 *
 *
 *  - wave_file_data: load an entire wave file into memory or write
 *   a wave file to disk from memory with a single call.
 *  - wave_file_reader: read a wave file from disk sample after sample.
 *   Use this class to stream audio data from disk.
 *   - wave_file_reader: write a wave file to disk sample after sample.
 *   Use this class to stream audio data to disk.
 *   - wave_file_header: utility class to load ans save wave file header.
 */

/** Simple wave file loader/saver (limited support for simple wav files).
 *
 */
class wave_file_data
{
	array<double> interleaved_samples; ///< interleaved audio samples
	uint64 channels_count;			   ///< number of audio channels
	double sample_rate;				   ///< the sample rate of the audio data

	/// length property (number of samples)
	int length
	{
		get const
		{
			if (channels_count != 0)
				return interleaved_samples.length() / channels_count;
			else
				return 0;
		}
	}

	/** load entire audio file from disk into the interleaved_samples array.
	 *   This method allocates memory and may take a while,
	 *   so this should not be called from the real time audio thread.
	 */
	bool load_file(const string&in filePath)
	{
		bool ok = false;
		// open the file
		file f;
		if (f.open(filePath, "r") >= 0)
		{
			// read the header
			wave_file_header header;
			if (header.read(f))
			{
				ok = true;
				// store file data
				channels_count = header.channels_count;
				sample_rate = header.sample_rate;
				interleaved_samples.resize(header.samples_count * channels_count);

				switch (header.bytes_per_sample)
				{
						// 8-bit wav files use only positive values (0 to 255)
					case 1:
					{
						for (uint i = 0; i < header.samples_count; i++)
						{
							for (uint ch = 0; ch < channels_count; ch++)
							{
								interleaved_samples[i + ch] = (double(f.read_uint(header.bytes_per_sample)) - 128.0) / 128.0;
							}
						}
						break;
					}
						// single precision floating point wav file
					case 4:
					{
						for (uint i = 0; i < header.samples_count; i++)
						{
							for (uint ch = 0; ch < channels_count; ch++)
							{
								interleaved_samples[i + ch] = f.read_float();
							}
						}
						break;
					}
					case 8:
						// double precision floating point wav file
						{
							for (uint i = 0; i < header.samples_count; i++)
							{
								for (uint ch = 0; ch < channels_count; ch++)
								{
									interleaved_samples[i + ch] = f.read_double();
								}
							}
							break;
						}
						// 16 or 24-bit integer files
					default:
					{
						uint64 maxValue = (1 << (header.bytes_per_sample * 8 - 1)) - 1;
						double factor = 1.0 / double(maxValue);
						for (uint i = 0; i < header.samples_count; i++)
						{
							for (uint ch = 0; ch < channels_count; ch++)
							{
								int64 value = f.read_int(header.bytes_per_sample);
								interleaved_samples[i + ch] = double(value) * factor;
							}
						}
						break;
					}
				}
			}
			f.close();
		}
		return ok;
	}

	/** Write the audio data in interleaved_samples to disk as a wav file.
	 *   This method allocates memory and may take a while,
	 *   so this should not be called from the real time audio thread.
	 *   bitDepth is the number of bits for each sample in the wave file.
	 */
	bool write_file(const string&in filePath, uint bitDepth = 16)
	{
		bool ok = false;
		// open file to write
		file f;
		if (f.open(filePath, "w") >= 0)
		{
			// write header
			wave_file_header header;
			header.bytes_per_sample = bitDepth / 8;
			header.sample_rate = uint64(sample_rate);
			header.samples_count = interleaved_samples.length() / channels_count;
			header.channels_count = channels_count;
			header.write(f);
			// write samples
			switch (header.bytes_per_sample)
			{
				case 1:
				{
					// 8-bit wav files use only positive values (0 to 255)
					for (uint i = 0; i < header.samples_count; i++)
					{
						for (uint ch = 0; ch < header.channels_count; ch++)
						{
							f.write_uint(uint64(interleaved_samples[i + ch] * 128.0 + 128.0), 1);
						}
					}
					break;
				}
					// single precision floating point wav file
				case 4:
				{
					for (uint i = 0; i < header.samples_count; i++)
					{
						for (uint ch = 0; ch < header.channels_count; ch++)
						{
							f.write_float(float(interleaved_samples[i + ch]));
						}
					}
					break;
				}
				case 8:
					// double precision floating point wav file
					{
						for (uint i = 0; i < header.samples_count; i++)
						{
							for (uint ch = 0; ch < header.channels_count; ch++)
							{
								f.write_double(interleaved_samples[i + ch]);
							}
						}
						break;
					}
				default:
				{
					double maxValue = (1 << (header.bytes_per_sample * 8 - 1)) - 1;
					double factor = double(maxValue);
					for (uint i = 0; i < header.samples_count; i++)
					{
						for (uint ch = 0; ch < header.channels_count; ch++)
						{
							int64 value = int64(interleaved_samples[i + ch] * maxValue);
							f.write_int(value, header.bytes_per_sample);
						}
					}
					break;
				}
			}
			ok = true;
			f.close();
		}
		return ok;
	}
};

/** Simple (sample per sample) wave file reader.
 *   Note: does not handle resampling.
 */
class wave_file_reader
{
	/** open a file for streaming audio data from it.
	 *   maxChannelsCount is the maximum number of channels that will be read from the
	 *   file. Set to -1 if you intend to use the number of channels of the file.
	 */
	bool open_file(const string&in filePath, int maxChannelsCount = -1)
	{
		bool ok = false;
		if (f.open(filePath, "r") >= 0)
		{
			if (header.read(f))
			{
				ok = true;
				block_size = header.channels_count * header.bytes_per_sample;
				if (header.bytes_per_sample > 1 && header.bytes_per_sample < 4)
				{
					uint64 maxValue = (1 << (header.bytes_per_sample * 8 - 1)) - 1;
					gain_factor = 1.0 / double(maxValue);
				}
				if (maxChannelsCount > 0 && uint(maxChannelsCount) < header.channels_count)
				{
					channels_to_read = maxChannelsCount;
					channels_to_skip = (header.channels_count - maxChannelsCount);
				}
				else
				{
					channels_to_read = header.channels_count;
					channels_to_skip = 0;
				}
			}
		}
		return ok;
	}

	/** Read one sample from the current position in the file.
	 *   Assumes that the file was succesfuly opened.
	 *   oSample is an array with at least maxChannelsCount (@see openFile),
	 *   or the number of channels containedin the file (@see get_channels_count).
	 */
	void read_sample(array<double>& oSample)
	{
		switch (header.bytes_per_sample)
		{
				// 8-bit wav file contain only positive values.
			case 1:
			{
				for (uint ch = 0; ch < channels_to_read; ch++)
				{
					oSample[ch] = (double(f.read_uint(header.bytes_per_sample)) - 128.0) / 128.0;
				}
				for (uint ch = 0; ch < channels_to_skip; ch++)
				{
					f.read_uint(header.bytes_per_sample);
				}

				break;
			}
				// single precision floating point wav file
			case 4:
			{
				for (uint ch = 0; ch < channels_to_read; ch++)
				{
					oSample[ch] = f.read_float();
				}
				for (uint ch = 0; ch < channels_to_skip; ch++)
				{
					f.read_float();
				}
				break;
			}
			case 8:
				// double precision floating point wav file
				{
					for (uint ch = 0; ch < channels_to_read; ch++)
					{
						oSample[ch] = f.read_double();
					}
					for (uint ch = 0; ch < channels_to_skip; ch++)
					{
						f.read_double();
					}
					break;
				}
				// 16 or 24-bit integer wav file
			default:
			{
				for (uint ch = 0; ch < channels_to_read; ch++)
				{
					oSample[ch] = double(f.read_int(header.bytes_per_sample)) * gain_factor;
				}
				for (uint ch = 0; ch < channels_to_skip; ch++)
				{
					f.read_int(header.bytes_per_sample);
				}
				break;
			}
		}
	}
	/** Move file cursor to sample number "samplePosition".
	 *   Warning: does not check if end of data chunk is reached.
	 */
	bool set_pos(uint samplePosition)
	{
		f.set_pos(header.header_size + samplePosition * block_size);
		return true;
	}

	/** Move file cursor by "sampleOffset" samples.
	 *   returns false if failed.
	 *   Warning: does not check if end of data chunk is reached.
	 */
	bool move_pos(int sampleOffset)
	{
		int newPos = f.get_pos() + sampleOffset * block_size;
		if (newPos >= int(header.header_size))
		{
			f.set_pos(sampleOffset * block_size);
			return true;
		}
		else
			return false;
	}

	/** Check if end of file reached.
	 *
	 */
	bool is_end_of_file()
	{
		return f.is_end_of_file();
	}

	/** close the audio file.
	 *
	 */
	bool close()
	{
		return f.close() >= 0;
	}

	/** channels_count property (read only).
	 *
	 */
	int get_channels_count() const
	{
		return header.channels_count;
	}

	// private data
	private file f;					   // the file
	private uint block_size = 0;	   // sample block size (all channels)
	private uint channels_to_read = 0; // number of channels to read from file
	private uint channels_to_skip = 0; // number of channels to skip (channels not used)
	private double gain_factor = 1;	   // gain factor for integer files
	private wave_file_header header;   // the wave file header data
};

/** Simple (sample per sample) wave file writer.
 *
 */
class wave_file_writer
{
	/// header data to be used to write the file.
	wave_file_header header;

	/** Open the file for writing and pushes the current header data.
	 *   Note: header data will be updated on close, but file format cannot be changed
	 *   after the file has been opened for writing.
	 */
	bool open_file(const string&in filePath)
	{
		// reinit header
		header.samples_count = 0;

		// open the file
		bool ok = false;
		if (f.open(filePath, "w") >= 0)
		{
			if (header.write(f))
			{
				ok = true;
				if (header.bytes_per_sample > 1 && header.bytes_per_sample < 4)
				{
					uint64 maxValue = (1 << (header.bytes_per_sample * 8 - 1)) - 1;
					gain_factor = double(maxValue);
				}
			}
		}
		return ok;
	}

	/** Write one sample at the current position in the file.
	 *   Assumes that the file was succesfuly opened.
	 *   iSample is an array with at least the number of channels set in the header.
	 */
	void write_sample(const array<double>& iSample)
	{
		// increment samples count
		header.samples_count++;

		// write data
		switch (header.bytes_per_sample)
		{
				// 8-bit wav file
			case 1:
			{
				for (uint ch = 0; ch < header.channels_count; ch++)
				{
					f.write_uint(uint64(iSample[ch] * 128.0 + 128.0), 1);
				}
				break;
			}
				// single precision floating point wav file
			case 4:
			{
				for (uint ch = 0; ch < header.channels_count; ch++)
				{
					f.write_float(iSample[ch]);
				}
				break;
			}
			case 8:
				// double precision floating point wav file
				{
					for (uint ch = 0; ch < header.channels_count; ch++)
					{
						f.write_double(iSample[ch]);
					}
					break;
				}
			default:
			{
				for (uint ch = 0; ch < header.channels_count; ch++)
				{
					f.write_int(int64(iSample[ch] * gain_factor), header.bytes_per_sample);
				}
				break;
			}
		}
	}

	bool close()
	{
		// flush header to update file size
		f.set_pos(0);
		header.write(f);

		// close file
		return f.close() >= 0;
	}

	// destructor ensures that the file is properly closed,
	// with updated header, before the object is destroyed.
	~wave_file_writer()
	{
		close();
	}

	// private data
	private file f;					///< the audio file.
	private double gain_factor = 1; ///< factor used for integer wav files.
};

/** Utility class to handle Wave file header data.
 *   Used by the classes above.
 *   Ignores LIST and INFO chunks.
 */
class wave_file_header
{
	uint64 channels_count = 0; // number of audio channels
	uint64 samples_count = 0;  // number of audio samples
	uint64 sample_rate = 0;	   // the sample rate of the audio data
	uint bytes_per_sample = 0; // the number of bytes per individual channel sample
	uint header_size = 0;	   // the size of the header (offset to audio data)

	/** Read header data from file.
	 *   Assumes that the file is at position 0.
	 */
	bool read(file& f)
	{
		bool ok = false;
		string tempString;
		tempString = f.read(4);
		if (tempString == "RIFF")
		{
			int64 size = f.read_uint(4);
			tempString = f.read(4);
			if (tempString == "WAVE")
			{
				tempString = f.read(4);
				if (tempString == "fmt ")
				{
					uint64 fmtSize = f.read_uint(4);
					if (fmtSize >= 16) // PCM format size is 16 or 18
					{
						uint64 dataOffset = fmtSize - 16;
						uint64 format = f.read_int(2);
						if (format == 1 || format == 3) // uncompressed PCM format or float are supported
						{
							channels_count = f.read_uint(2);
							sample_rate = f.read_uint(4);
							uint64 byteRate = f.read_uint(4);
							uint64 block_size = f.read_uint(2);
							bytes_per_sample = f.read_uint(2) / 8;
							// end of subchunk
							f.move_pos(dataOffset);

							// looking for data subchunk => skipping all others
							tempString = f.read(4); // data id
							while (tempString != "data" && !f.is_end_of_file())
							{
								uint64 dataSize = f.read_uint(4);
								f.move_pos(dataSize);	// skip junk data
								tempString = f.read(4); // read next chunk id
							}
							if (tempString == "data")
							{
								uint64 dataSize = f.read_uint(4);
								samples_count = dataSize / block_size;
							}
							header_size = f.get_pos();
							ok = true;
						}
					}
				}
			}
		}
		return ok;
	}

	/** Write header data to file.
	 *   Assumes that the file is at position 0.
	 */
	bool write(file& f)
	{
		f.write("RIFF");
		int64 dataSize = (bytes_per_sample)*samples_count * channels_count;
		f.write_uint(36 + dataSize, 4);
		f.write("WAVE");
		f.write("fmt ");
		f.write_uint(16, 4);
		// uncompressed PCM format or float
		if (bytes_per_sample >= 4)
			f.write_int(3, 2);
		else
			f.write_int(1, 2);
		f.write_uint(channels_count, 2);
		f.write_uint(sample_rate, 4);
		f.write_uint(sample_rate * channels_count * bytes_per_sample, 4); // byte rate
		f.write_uint(channels_count * bytes_per_sample, 2);
		f.write_uint(bytes_per_sample * 8, 2);

		// data subchunk
		f.write("data"); // data id
		f.write_uint(dataSize, 4);
		return true;
	}
};
