//
// Created by RGAA on 2023/8/10.
//

#include <iostream>
#include <thread>
#include <chrono>

#include "Application.h"
#include "context/Context.h"
#include "context/Message.h"
#include "context/MessageQueue.h"
#include "settings/Settings.h"
#include "capture/Capture.h"
#include "capture/DDACapture.h"
#include "capture/CaptureFactory.h"
#include "capture/CapturedFrame.h"
#include "encoder/Encoder.h"
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

#ifdef _OS_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace rgaa {

    Application::Application() {

    }

    Application::~Application() {
        if (audio_capture_) {
            audio_capture_->Pause();
            audio_capture_->Stop();
        }
        if (audio_thread_ && audio_thread_->IsJoinable()) {
            audio_thread_->Join();
        }
    }

    void Application::Init() {
        settings_ = Settings::Instance();
        settings_->LoadSettings();

        context_ = std::make_shared<Context>();
        context_->Init();

        capture_ = CaptureFactory::MakeCapture(context_);

        ws_server_ = std::make_shared<WSServer>(context_, "0.0.0.0", 9090);
    }

    void Application::Start() {

        ws_server_->Start();
        StartAudioCapturing();
        StartVideoCapturing();

        auto msg_queue = context_->GetMessageQueue();
        std::shared_ptr<Message> msg = nullptr;
        while ((msg = msg_queue->Peek()) != nullptr) {
            std::cout << "msg : " << msg->code << std::endl;
        }
    }

    void Application::StartVideoCapturing() {
        if (!capture_) {
            return;
        }

        bool ok = capture_->Init();
        if (!ok) {
            std::cout << "Init capture failed !" << std::endl;
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
            }
            auto encoded_frame = encoder->Encode(cp_frame);
            if (encoded_frame && ws_server_) {
                auto msg = encoded_frame->AsProtoMessageStr();
                auto duration_from_capture = GetCurrentTimestamp() - encoded_frame->captured_time_;
                auto duration_from_encode = GetCurrentTimestamp() - encoded_frame->encoded_time_;
                //LOGI("Duration from capture: {}", duration_from_capture);

                ws_server_->PostBinaryMessage(msg);
            }
        });

        for (;;) {
            capture_->CaptureNextFrame();
            //std::cout << ".";
        }
    }

    std::shared_ptr<Encoder> Application::GetEncoderForIndex(int dup_idx) {
        for (auto& en : encoders_) {
            if (en.first == dup_idx) {
                return en.second;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Encoder> Application::MakeEncoder(int dup_idx, int w, int h) {
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

            });

            audio_capture_->RegisterDataCallback([=, this] (rgaa::DataPtr data){

            });

            audio_capture_->StartRecording();

        }, "", false);

    }

}