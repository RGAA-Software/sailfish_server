//
// Created by RGAA on 2023/8/10.
//

#include <iostream>
#include <thread>
#include <chrono>

#include "Application.h"
#include "context/Context.h"
#include "settings/Settings.h"
#include "capture/Capture.h"
#include "capture/DDACapture.h"
#include "capture/CaptureFactory.h"
#include "capture/CapturedFrame.h"
#include "encoder/VideoEncoder.h"
#include "encoder/EncoderFactory.h"
#include "encoder/FFmpegEncoder.h"
#include "rgaa_common/RLog.h"
#include "network/WSServer.h"
#include "encoder/EncodedFrame.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RCloser.h"
#include "audio/AudioCaptureFactory.h"
#include "audio/AudioCapture.h"
#include "rgaa_common/RThread.h"
#include "encoder/AudioEncoder.h"
#include "network/MessageMaker.h"

#ifdef _OS_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace rgaa {

    Application::Application(const std::shared_ptr<Context>& ctx, bool audio) {
        context_ = ctx;
        connection_ = context_->GetConnection();
        audio_enabled_ = audio;
    }

    Application::~Application() {

    }

    void Application::Start() {
        settings_ = Settings::Instance();
        settings_->LoadSettings();

        if (audio_enabled_) {
            StartAudioCapturing();
        }

        StartVideoCapturing();
    }

    void Application::StartVideoCapturing() {
        capture_ = CaptureFactory::MakeCapture(context_);
        bool ok = capture_->Init();
        if (!ok) {
            LOGE("Init video capture failed.");
            return;
        }

        capture_->SetOnFrameCapturedCallback([this](const std::shared_ptr<CapturedFrame>& cp_frame){
            auto encoder = GetEncoderForIndex(cp_frame->dup_index_);
            if (!encoder) {
                encoder = MakeEncoder(cp_frame->dup_index_, cp_frame->frame_width_, cp_frame->frame_height_);
                if (!encoder) {
                    LOGE("Create encoder failed , encode type : {} , dup idx: {}, with: {}, height: {}",
                         (int)settings_->GetEncodeType(), cp_frame->dup_index_, cp_frame->frame_width_, cp_frame->frame_height_);
                    return;
                }

                if (connection_) {
                    auto msg = MessageMaker::MakeVideoConfigSync(settings_->GetEncodeType(),
                                                                 cp_frame->frame_width_,
                                                                 cp_frame->frame_height_);
                    context_->PostNetworkBinaryMessage(msg);
                }
            }
            auto encoded_frame = encoder->Encode(cp_frame);
            if (encoded_frame && connection_) {
                auto msg = encoded_frame->AsProtoMessage();
                auto duration_from_capture = GetCurrentTimestamp() - encoded_frame->captured_time_;
                auto duration_from_encode = GetCurrentTimestamp() - encoded_frame->encoded_time_;
                //LOGI("Duration from capture: {}", duration_from_capture);

                context_->PostNetworkBinaryMessage(msg);
            }
        });

        if (settings_->GetCaptureAPI() == CaptureAPI::kDesktopDuplication) {
            for (;;) {
                if (!capture_ || !capture_->CaptureNextFrame()) {
                    break;
                }
            }

            capture_loop_exit_ = true;
        }
    }

    std::shared_ptr<VideoEncoder> Application::GetEncoderForIndex(int dup_idx) {
        for (auto& en : encoders_) {
            if (en.first == dup_idx) {
                return en.second;
            }
        }
        return nullptr;
    }

    std::shared_ptr<VideoEncoder> Application::MakeEncoder(int dup_idx, int w, int h) {
        auto encoder = EncoderFactory::MakeEncoder(context_, dup_idx,w, h);
        bool ok = encoder->Init();
        encoders_.insert(std::make_pair(dup_idx, encoder));
        return ok ? encoder : nullptr;
    }

    void Application::StartAudioCapturing() {
        audio_thread_ = std::make_shared<Thread>([=, this] (){

            audio_capture_ = AudioCaptureFactory::MakeAudioCapture();
            if (audio_capture_->Prepare() != 0) {
                LOGE("Init audio capture failed.");
                return;
            }

            audio_capture_->RegisterFormatCallback([=, this](int samples, int channels, int bits) {
                audio_encoder_ = std::make_shared<AudioEncoder>(context_, samples, channels, bits);
                if (connection_) {
                    auto msg = MessageMaker::MakeAudioConfigSync(samples, channels);
                    context_->PostNetworkBinaryMessage(msg);
                }
            });

            audio_capture_->RegisterDataCallback([=, this] (rgaa::DataPtr data){
                if (!audio_encoder_) {
                    return;
                }
                // 2 channels, 16bits/8 = 2bytes, frame size is int16 format
                int frame_size = data->Size() / 2 / 2;
                auto frames = audio_encoder_->Encode(data, frame_size);
                if (!frames.empty() && connection_) {
                    auto samples = audio_encoder_->Samples();
                    auto channels = audio_encoder_->Channels();
                    for (const auto& frame : frames) {
                        auto msg = MessageMaker::MakeAudioFrameSync(frame, frame_size, samples, channels);
                        context_->PostNetworkBinaryMessage(msg);
                    }
                }
            });

            audio_capture_->StartRecording();

        }, "audio_capture", false);

    }

    void Application::Exit() {
        if (capture_) {
            capture_->Exit();
            capture_.reset();
            LOGI("Video capture released...");
        }
        if (video_thread_ && video_thread_->IsJoinable()) {
            video_thread_->Join();
            LOGI("Video thread exit...");
        }
        for (auto& [k, encoder] : encoders_) {
            if (encoder) {
                encoder->Exit();
                encoder.reset();
            }
            LOGI("Encoders released...");
        }
        if (audio_capture_) {
            audio_capture_->Pause();
            audio_capture_->Stop();
            audio_capture_.reset();
            LOGI("Audio capture released...");
        }
        if (audio_encoder_) {
            audio_encoder_->Exit();
            LOGI("Audio encoder release...");
        }
        if (audio_thread_ && audio_thread_->IsJoinable()) {
            audio_thread_->Join();
            audio_thread_.reset();
            LOGI("Audio thread exit...");
        }

    }

}