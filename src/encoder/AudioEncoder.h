#pragma once

#include "rgaa_opus_codec/OpusCodec.h"

#include <memory>
#include <functional>
#include <vector>

constexpr auto kAudioFrameSize = 960;

namespace rgaa
{
	
	class Data;
	class File;
	class Thread;
	class Context;

	class AudioEncoder {
	public:
		
		// eg: 48000 2 16
		AudioEncoder(const std::shared_ptr<Context>& ctx, int samples, int channels, int bits);
		~AudioEncoder();

		bool IsValid();
		std::vector<std::shared_ptr<Data>> Encode(const std::shared_ptr<Data>& data, int frame_size);
		int Channels();
		int Samples();
		int Bits();
        void Exit();

	private:
		std::shared_ptr<Context> context = nullptr;
		std::shared_ptr<rgaa::Encoder> encoder = nullptr;
		std::shared_ptr<rgaa::Decoder> decoder = nullptr;
		std::shared_ptr<File> debug_decode_file = nullptr;
		int channels;
		int samples;
		int bits;
		bool decoder_check = false;
	};

	using AudioFrameEncoderPtr = std::shared_ptr<AudioEncoder>;
}