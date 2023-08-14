//
// Created by RGAA on 2023/8/10.
//

#include "EncoderFactory.h"

#include "FFmpegEncoder.h"
#include "settings/Settings.h"
#include "encoder/EncoderChecker.h"
#include "rgaa_common/RLog.h"

namespace rgaa {

    std::shared_ptr<VideoEncoder> EncoderFactory::MakeEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, int width, int height) {
        auto supported_encoder = SelectEncoder();
        if (!supported_encoder) {
            return nullptr;
        }

        if (supported_encoder->kind_ == EncoderKind::kFFmpeg) {
            return std::make_shared<FFmpegEncoder>(ctx, dup_idx,supported_encoder->name_, width, height);
        }
        else if (supported_encoder->kind_ == EncoderKind::kNvidiaSdk) {
            // todo
            return nullptr;
        }
        return nullptr;
    }

    std::shared_ptr<SupportedEncoder> EncoderFactory::SelectEncoder() {
        auto supported_encoders = EncoderChecker::Instance()->GetSupportedEncoders();
        auto func_find_encoder_by_name = [&](const std::string& name)-> std::shared_ptr<SupportedEncoder> {
            for (const auto& encoder : supported_encoders) {
                if (encoder->name_ == name) {
                    return encoder;
                }
            }
            return nullptr;
        };

        auto settings = Settings::Instance();
        auto target_type = settings->GetEncodeType();

        // 1. Nvidia
        std::shared_ptr<SupportedEncoder> nvidia_encoder = nullptr;
        for (const auto& encoder : supported_encoders) {
            if (encoder->kind_ == EncoderKind::kNvidiaSdk) {
                nvidia_encoder = encoder;
            }
        }
        if (nvidia_encoder && nvidia_encoder->IsValid()) {
            return nvidia_encoder;
        }

        // 2. FFmpeg
        std::shared_ptr<SupportedEncoder> ffmpeg_encoder = nullptr;
        if (target_type == EncodeType::kH265) {
            ffmpeg_encoder = func_find_encoder_by_name("hevc_nvenc");
            if (!ffmpeg_encoder || !ffmpeg_encoder->IsValid()) {
                ffmpeg_encoder = func_find_encoder_by_name("libx265");
            }
        }
        else if (target_type == EncodeType::kH264) {
            ffmpeg_encoder = func_find_encoder_by_name("h264_nvenc");
            if (!ffmpeg_encoder || !ffmpeg_encoder->IsValid()) {
                ffmpeg_encoder = func_find_encoder_by_name("libx264");
            }
        }
        return ffmpeg_encoder;
    }

}