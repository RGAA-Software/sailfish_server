//
// Created by RGAA on 2023/8/13.
//

#include "MessageProcessor.h"

#include "context/Context.h"
#include "messages.pb.h"
#include "controller/EventReplayer.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"

namespace rgaa {

    MessageProcessor::MessageProcessor(const std::shared_ptr<Context>& ctx) {
        context_ = ctx;
        replayer_ = std::make_shared<EventReplayer>(ctx);
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
            LOGI("ProcessMessage, will process StartRecording");
            auto& payload = message->start_recording();
            context_->StartApplication(payload.audio());
            return;
        }
        else if (message->has_stop_recording()) {
            auto& payload = message->stop_recording();
            context_->StopApplication();
            return;
        }
        else if (message->has_mouse_info() || message->has_keyboard_info()) {
            replayer_->Replay(message);
            return;
        }
        else if (message->has_heart_beat()) {
            auto time = GetCurrentTimestamp();
            auto idx = message->heart_beat().index();
            context_->UpdateHeartBeat(time, idx);
            return;
        }
    }

}