//
// Created by RGAA on 2023/8/10.
//

#include "Context.h"

#include "MessageQueue.h"

namespace rgaa {

    Context::Context() {

    }

    Context::~Context() {

    }

    void Context::Init() {
        msg_queue_ = std::make_shared<MessageQueue>();
    }

    std::shared_ptr<MessageQueue> Context::GetMessageQueue() {
        return msg_queue_;
    }

}
