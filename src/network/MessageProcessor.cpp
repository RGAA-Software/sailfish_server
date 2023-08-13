//
// Created by RGAA on 2023/8/13.
//

#include "MessageProcessor.h"

#include "context/Context.h"
#include "messages.pb.h"
#include "controller/EventReplayer.h"

namespace rgaa {

    MessageProcessor::MessageProcessor(const std::shared_ptr<Context>& ctx) {
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

        if (message->has_mouse_info() || message->has_keyboard_info()) {
            replayer_->Replay(message);
            return;
        }


    }

}