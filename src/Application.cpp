//
// Created by RGAA on 2023/8/10.
//

#include <iostream>
#include <thread>
#include <chrono>

#include "Application.h"
#include "context/Context.h"
#include "src/context/Settings.h"
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
#include "rgaa_common/RMessage.h"
#include "rgaa_common/RMessageQueue.h"
#include "capture/CursorCapture.h"
#include "AppMessages.h"
#include "messages.pb.h"
#include "Statistics.h"

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

        auto statistics = context_->GetStatistics();
        statistics->Reset();

        timer_1s_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kMessageCodeTimer1S, [](auto& msg){

        }));

        peer_connected_msg_id_ = context_->RegisterMessageTask(MessageTask::Make(kPeerConnected, [=, this](auto& msg) {
            SendBackConfig();
        }));

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

        cursor_capture_ = std::make_shared<CursorCapture>(context_);

        // send back config
        SendBackConfig();

        capture_->SetOnFrameCapturedCallback([this](const std::shared_ptr<CapturedFrame>& cp_frame){
            auto current_time = GetCurrentTimestamp();
            auto diff = current_time - last_send_video_time_;
            last_send_video_time_ = current_time;
            //LOGI("{} - CBK - {}", cp_frame->frame_index_, diff);

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

                auto duration_from_encode = GetCurrentTimestamp() - encoded_frame->encoded_time_;
                //LOGI("Duration from capture: {}", duration_from_capture);

                auto statistics = context_->GetStatistics();
                auto video_frame = msg->mutable_video_frame();
                video_frame->set_previous_network_time(statistics->GetFrameNetworkTime(encoded_frame->frame_index_-1));

                context_->PostNetworkBinaryMessage(msg);
            }
        });

        if (settings_->GetCaptureAPI() == CaptureAPI::kDesktopDuplication) {
            for (;;) {
                if (!capture_ || !capture_->CaptureNextFrame()) {
                    break;
                }

                if (cursor_capture_) {
                    auto cursor_info_msg = cursor_capture_->Capture();
                    context_->PostNetworkBinaryMessage(cursor_info_msg);
                }
            }
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
                if (!audio_encoder_ || !context_->IsAudioEnabled()) {
                    return;
                }
                // 2 channels, 16bits/8 = 2bytes, frame size is int16 format
                int frame_size = data->Size() / 2 / 2;
                auto frames = audio_encoder_->Encode(data, frame_size);
                if (!frames.empty() && connection_) {
                    auto samples = audio_encoder_->Samples();
                    auto channels = audio_encoder_->Channels();
                    auto bits = audio_encoder_->Bits();
                    for (const auto& frame : frames) {
                        auto msg = MessageMaker::MakeAudioFrameSync(frame, frame_size, samples, channels, bits);
                        context_->PostNetworkBinaryMessage(msg);
                    }
                }
            });

            audio_capture_->StartRecording();

        }, "audio_capture", false);

    }

    void Application::SendBackConfig() {
        int count = capture_->GetCaptureCount();
        auto msg = std::make_shared<NetMessage>();

        auto config = new ConfigSync();
        config->set_screen_size(count);

        msg->set_allocated_config(config);

        context_->PostNetworkBinaryMessage(msg);
    }

    void Application::Exit() {
        if (timer_1s_task_id_ != -1) {
            context_->RemoveMessageTask(timer_1s_task_id_);
        }
        if (peer_connected_msg_id_) {
            context_->RemoveMessageTask(peer_connected_msg_id_);
        }

        if (capture_) {
            capture_->Exit();
            capture_.reset();
            LOGI("Video capture released...");
        }
        if (cursor_capture_) {
            cursor_capture_.reset();
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