//
// Created by RGAA on 2023/8/11.
//

#include "EncoderChecker.h"
#include "rgaa_common/RLog.h"

#include <map>

namespace rgaa {

    EncoderChecker::EncoderChecker() {

    }

    EncoderChecker::~EncoderChecker() {

    }

    void EncoderChecker::CheckSupportedEncoders() {
        supported_encoders_.clear();
        // 1. check ffmpeg
        std::map<std::string, std::string> encoder_names = {
            {"libx264", "Software H264"},
            {"libx265", "Software H265"},
            {"h264_nvenc", "Hardware H264"},
            {"hevc_nvenc", "Hardware H265"},
        };

        for (auto& [encoder_name, readable_name] : encoder_names) {
            const AVCodec* codec = avcodec_find_encoder_by_name(encoder_name.c_str());
            if (!codec) {
                LOGW("VideoEncoder : {} not supported.", encoder_name);
                continue;
            }

            auto context = avcodec_alloc_context3(codec);
            if (!context) {
                LOGI("avcodec_alloc_context3 error, {} not supported", encoder_name);
                continue;
            }

            context->width = 960;
            context->height = 540;
            context->time_base = {1, 60};
            context->pix_fmt = AV_PIX_FMT_YUV420P;
            context->thread_count = 4;

            auto ret = avcodec_open2(context, codec, NULL);
            if (ret != 0) {
                LOGI("avcodec_open2 error , {} not supported", encoder_name);
                continue;
            }

            auto se = std::make_shared<SupportedEncoder>();
            se->kind_ = EncoderKind::kFFmpeg;
            se->name_ = encoder_name;
            se->readable_name_ = readable_name;
            supported_encoders_.push_back(se);

            avcodec_close(context);
            avcodec_free_context(&context);
        }

        // 2. check nvenc from Nvidia sdk, if supported, we will encode by d3d texture.
        // todo
    }

    std::vector<std::shared_ptr<SupportedEncoder>> EncoderChecker::GetSupportedEncoders() {
        return supported_encoders_;
    }

    void EncoderChecker::DumpSupportedEncoders() {
        LOGI("Total supported encoders : {}", supported_encoders_.size());
        for (auto& encoder : supported_encoders_) {
            LOGI("VideoEncoder name : {}, encoder kind : {}", encoder->name_, (int)encoder->kind_);
        }
    }

}
