#pragma once

#include <libyuv.h>

#include <string>
#include <fstream>

#include "VideoEncoder.h"

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/error.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
}


namespace rgaa {

    class Data;

	class FFmpegEncoder : public VideoEncoder {
	public:
        FFmpegEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, const std::string& encoder_name, int width, int height);
		~FFmpegEncoder();

        bool Init() override;
        void Exit() override;
		std::shared_ptr<EncodedVideoFrame> Encode(const std::shared_ptr<CapturedFrame>& cp_frame) override;

    private:

        static std::string GetErrorString(int code);

	private:

		AVCodecContext* codec_context_ = nullptr;
		AVFrame* frame_ = nullptr;
		AVPacket* packet_ = nullptr;

        std::ofstream debug_encoder_file_;
	};

}