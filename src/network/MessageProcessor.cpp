//
// Created by RGAA on 2023/8/13.
//

#include "MessageProcessor.h"

#include "context/Context.h"
#include "messages.pb.h"
#include "controller/EventReplayer.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"
#include "MessageMaker.h"
#include "messages.pb.h"
#include "AppMessages.h"
#include "Statistics.h"
#include "context/Settings.h"

namespace rgaa {

    MessageProcessor::MessageProcessor(const std::shared_ptr<Context>& ctx) {
        context_ = ctx;
        replayer_ = std::make_shared<EventReplayer>(ctx);
        settings_ = Settings::Instance();
    }

    MessageProcessor::~MessageProcessor() {

    }

    void MessageProcessor::ProcessMessage(const std::string& msg) {
        std::shared_ptr<NetMessage> message = std::make_shared<NetMessage>();
        bool ok = message->ParseFromString(msg);
        if (!ok) {
            return;
        }

        if (message->has_start_recording()) {
            auto& payload = message->start_recording();
            settings_->encode_bps_ = payload.bitrate();
            settings_->encode_fps_ = payload.approximate_fps();
            context_->PostTask([=, this] () {
                context_->StartApplication(true);
            });
            return;
        }
        else if (message->has_stop_recording()) {
            auto& payload = message->stop_recording();
            context_->PostTask([=, this] () {
                context_->StopApplication();
            });
            return;
        }
        else if (message->has_mouse_info() || message->has_keyboard_info()) {
            context_->PostTask([=, this]() {
                replayer_->Replay(message);
            });
            return;
        }
        else if (message->has_heart_beat()) {
            auto time = GetCurrentTimestamp();
            auto idx = message->heart_beat().index();
            context_->UpdateHeartBeat(time, idx);

            auto resp_msg = MessageMaker::MakeHeartBeat(idx);
            context_->PostNetworkBinaryMessage(resp_msg);

            return;
        }
        else if (message->has_clipboard()) {
            auto clipboard = message->clipboard();
            auto clipboard_msg = ClipboardMessage::Make(clipboard.msg());
            context_->SendAppMessage(clipboard_msg);
            return;
        }
        else if (message->has_message_ack()) {
            auto ack = message->message_ack();

            auto frame_idx = ack.frame_index();
            auto send_time = ack.send_time();
            auto diff = GetCurrentTimestamp() - send_time;

            auto statistics = context_->GetStatistics();
            statistics->AppendVideoFrameNetworkTime(frame_idx, diff);
            return;
        }
        else if (message->has_client_cmd_report()) {
            auto cmd = message->client_cmd_report();
            auto type = cmd.type();
            if (type == ClientCommandType::kStartDebug) {

            }
            else if (type == ClientCommandType::kStopDebug) {

            }
            else if (type == ClientCommandType::kEnableAudio) {
                context_->EnableAudio();
            }
            else if (type == ClientCommandType::kDisableAudio) {
                context_->DisableAudio();
            }
            else if (type == ClientCommandType::kEnableClipboard) {
                context_->EnableClipboard();
            }
            else if (type == ClientCommandType::kDisableClipboard) {
                context_->DisableClipboard();
            }
        }
    }

}