#include "FFmpegEncoder.h"

#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RCloser.h"

#include "capture/CapturedFrame.h"
#include "settings/Settings.h"
#include "EncodedFrame.h"

#include <fstream>

#define DEBUG_ENCODE_FILE 0

namespace rgaa {

	FFmpegEncoder::FFmpegEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, const std::string& encoder_name, int width, int height)
        : VideoEncoder(ctx, dup_idx, encoder_name, width, height) {
#if DEBUG_ENCODE_FILE
        auto suffix = settings_->GetEncodeType() == EncodeType::kH265 ? ".h265" : ".h264";
        auto debug_file_name = "debug_encode_" + std::to_string(dup_idx) + suffix;
        debug_encoder_file_ = std::ofstream(debug_file_name, std::ios::binary);
#endif
	}
	
	FFmpegEncoder::~FFmpegEncoder() {

	}

	bool FFmpegEncoder::Init() {
        VideoEncoder::Init();

        av_log_set_level(AV_LOG_WARNING);
        av_log_set_callback([](void* ptr, int level, const char* fmt, va_list vl) {
            int print_prefix = 1;
            std::string message;
            message.resize(1024 * 4);
            av_log_format_line(ptr, level, fmt, vl, message.data(), message.size(), &print_prefix);
            //LOGI("FFmpeg: {}", message);
        });

        LOGI("FFmpeg use encoder : {}", encoder_name_);
        const AVCodec* codec = avcodec_find_encoder_by_name(encoder_name_.c_str());

        codec_context_ = avcodec_alloc_context3(codec);
		if (!codec_context_) {
			LOGI("avcodec_alloc_context3 error!");
			return false;
		}

        codec_context_->width = width_;
        codec_context_->height = height_;
        codec_context_->time_base = {1, 60 };
        codec_context_->pix_fmt = AV_PIX_FMT_YUV420P;
        codec_context_->thread_count = (int)std::thread::hardware_concurrency()/2;
        codec_context_->gop_size = 180;
        //codec_context_->gop_size = -1;
//        codec_context_->bit_rate = 400000;
        codec_context_->time_base =  {1, 90000 };
        codec_context_->max_b_frames = 0;
        codec_context_->flags |= AV_CODEC_FLAG_LOW_DELAY;

        LOGI("Will config : {}", encoder_name_);

        std::string qp = "23";
        if (encoder_name_.find("nvenc") != std::string::npos) {
            LOGI("Config : {}", encoder_name_);
            av_opt_set(codec_context_->priv_data, "preset", "llhp", 0);
            av_opt_set(codec_context_->priv_data, "profile", "main", 0);
            av_opt_set(codec_context_->priv_data, "delay", "0", 0);
            av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);

            if (encoder_name_ == "h264_nvenc") {
                av_opt_set(codec_context_->priv_data, "rc", "cbr", 0);
                av_opt_set(codec_context_->priv_data, "cq", qp.c_str(), 0);
            }
            else if (encoder_name_ == "hevc_nvenc"/*AV_CODEC_ID_HEVC == GetCodecID()*/) {
                av_opt_set(codec_context_->priv_data, "qp", qp.c_str(), 0);
            }
            av_opt_set(codec_context_->priv_data, "tune", "ull", 0);
            av_opt_set(codec_context_->priv_data, "zerolatency", "1", 0);

        }
        else if (encoder_name_.find("libx") != std::string::npos) {
            LOGI("Config : {}", encoder_name_);
            av_opt_set(codec_context_->priv_data, "preset", "ultrafast", 0);
            av_opt_set(codec_context_->priv_data, "crf", "23", 0);
            av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);
            av_opt_set(codec_context_->priv_data, "tune", "zerolatency", 0);
            av_opt_set(codec_context_->priv_data, "rc", "cbr", 0);

            if (encoder_name_ == "libx264") {
                av_opt_set(codec_context_->priv_data, "profile", "baseline", 0);
            }
            else if (encoder_name_ == "libx265") {
                av_opt_set(codec_context_->priv_data, "profile", "main", 0);
            }
        }

		auto ret = avcodec_open2(codec_context_, codec, NULL);
		if (ret != 0) {
            LOGI("avcodec_open2 error ! {0:x}, msg : {1} ", ret, GetErrorString(ret).c_str());
			return false;
		}

		frame_ = av_frame_alloc();
		frame_->width = codec_context_->width;
		frame_->height = codec_context_->height;
		frame_->format = codec_context_->pix_fmt;

        av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);
//        av_opt_set(codec_context_->priv_data, "tune", "zerolatency", 0);
//        av_opt_set(codec_context_->priv_data, "preset", "superfast", 0);
//        av_opt_set(codec_context_->priv_data, "usage", "ultralowlatency", 0);



		av_frame_get_buffer(frame_, 0);

		LOGI("Line 1: {} 2: {} 3: {}", frame_->linesize[0], frame_->linesize[1], frame_->linesize[2]);

		packet_ = av_packet_alloc();

		return true;
	}

	void FFmpegEncoder::Exit() {
        VideoEncoder::Exit();

        if (codec_context_) {
            avcodec_close(codec_context_);
            avcodec_free_context(&codec_context_);
        }
        if (frame_) {
            av_frame_free(&frame_);
        }
        if (packet_) {
            av_packet_unref(packet_);
        }
        LOGI("FFmpeg eocoder exit.");
#if DEBUG_ENCODE_FILE
        if (debug_encoder_file_.is_open()) {
            debug_encoder_file_.close();
        }
#endif
	}

    std::shared_ptr<EncodedVideoFrame> FFmpegEncoder::Encode(const std::shared_ptr<CapturedFrame> &cp_frame) {
        auto encode_used_time = RegionTimeCount::Make([](auto count) {
            //LOGI("Encode time : {}", count);
        });
        if (cp_frame->capture_type_ != CaptureType::kRawData) {
            return nullptr;
        }
        frame_->pts = cp_frame->frame_index_;

        auto counter = RegionTimeCount::Make([](auto duration) {

        });

        auto i420_data = cp_frame->raw_data_;
        if (!i420_data) {
            return nullptr;
        }

		int y_size = width_ * height_;
		int uv_size = y_size / 4;
		memcpy(frame_->data[0], i420_data->CStr(), y_size);
		memcpy(frame_->data[1], i420_data->CStr() + y_size, uv_size);
		memcpy(frame_->data[2], i420_data->CStr() + y_size + uv_size, uv_size);

        if (insert_idr_) {
            frame_->pict_type = AV_PICTURE_TYPE_I;
            insert_idr_ = false;
        }

		int send_result = avcodec_send_frame(codec_context_, frame_);

        std::shared_ptr<EncodedVideoFrame> encoded_video_frame = nullptr;

		while (send_result >= 0)
		{
			int recv_result = avcodec_receive_packet(codec_context_, packet_);

			if (recv_result == AVERROR(EAGAIN) || recv_result == AVERROR_EOF) break;
#if DEBUG_ENCODE_FILE
            debug_encoder_file_.write((char*)packet_->data, packet_->size);
#endif

            bool key_frame = packet_->flags & AV_PKT_FLAG_KEY;

			auto encoded_data = Data::Make((char*)packet_->data, packet_->size);
            encoded_video_frame = std::make_shared<EncodedVideoFrame>();
            encoded_video_frame->frame_index_ = cp_frame->frame_index_;
            encoded_video_frame->data_ = encoded_data;
            encoded_video_frame->width_ = cp_frame->frame_width_;
            encoded_video_frame->height_ = cp_frame->frame_height_;
            encoded_video_frame->dup_idx_ = cp_frame->dup_index_;
            encoded_video_frame->key_frame_ = key_frame;
            encoded_video_frame->encoded_time_ = GetCurrentTimestamp();
            encoded_video_frame->captured_time_ = cp_frame->captured_time_;

            //LOGI("frame encoded.");

			av_packet_unref(packet_);
		}
        return encoded_video_frame;
	}

    std::string FFmpegEncoder::GetErrorString(int code) {
        char err_buf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(code, err_buf, sizeof(err_buf));
        return std::string{err_buf};
    }

}