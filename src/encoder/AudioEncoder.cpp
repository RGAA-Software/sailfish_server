#include "AudioEncoder.h"

#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RFile.h"
#include "context/Context.h"
#include <iostream>

#define DEBUG_DECODER 0

namespace rgaa
{

	AudioEncoder::AudioEncoder(const std::shared_ptr<Context>& ctx, int samples, int channels, int bits) {
		this->context = ctx;
		this->samples = samples;
		this->channels = channels;
		this->bits = channels;
		encoder = std::make_shared<Encoder>(samples, channels, OPUS_APPLICATION_AUDIO);
		if (encoder->valid()) {
			encoder->SetComplexity(8);
		}
	}

	AudioEncoder::~AudioEncoder() {

	}

	bool AudioEncoder::IsValid() {
		return encoder && encoder->valid();
	}

	int AudioEncoder::Channels() {
		return channels;
	}

	int AudioEncoder::Samples() {
		return samples;
	}

	int AudioEncoder::Bits() {
		return bits;
	}

    std::vector<std::shared_ptr<Data>> AudioEncoder::Encode(const std::shared_ptr<Data>& data, int frame_size) {
        std::vector<DataPtr> frames;
		if (!encoder || !encoder->valid()) {
			LOGE("invalid audio encoder .");
			return frames;
		}
		std::vector<opus_int16> audio_data(frame_size * this->channels);
		if (data->Size() != audio_data.size() * 2) {
			LOGE("audio frame size invalid .");
			return frames;
		}
		
		memcpy((char*)audio_data.data(), data->DataAddr(), data->Size());
		auto encoded_frame = encoder->Encode(audio_data, frame_size);
		if (encoded_frame.empty()) {
			LOGE("no frames after encode.");
			return frames;
		}

		for (auto& piece : encoded_frame) {
#if DEBUG_DECODER
			if (decoder_check && decoder) {
				auto decoded_frame = decoder->Decode(piece, frame_size, false);
				std::cout << "piece size : " << piece.size() << " decoded frame size : " << decoded_frame.size() << std::endl;

				static int offset = 0;
				int bytes_size = decoded_frame.size() * 2;
				debug_decode_file->Write(offset, (char*)decoded_frame.data(), bytes_size);
				offset += bytes_size;
			}
#endif
            frames.push_back(Data::Make((char*)piece.data(), (int)piece.size()));
		}

        return frames;
	}

    void AudioEncoder::Exit() {
        if (encoder) {
            encoder->ResetState();
            encoder.reset();

            LOGI("Audio encoder exit.");
        }
    }
}