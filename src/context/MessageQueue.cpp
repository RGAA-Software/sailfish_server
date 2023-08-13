//
// Created by RGAA on 2023/8/10.
//

#include "MessageQueue.h"

#include "Message.h"

namespace rgaa {

    MessageQueue::MessageQueue(int max) {
        max_messages_ = max;
    }

    MessageQueue::~MessageQueue() {

    }

    void MessageQueue::Queue(const std::shared_ptr<Message>& msg) {
        std::lock_guard<std::mutex> guard(messages_mtx_);
        messages_.push(msg);
        if (messages_.size() > max_messages_) {
            messages_.pop();
        }
        messages_cv_.notify_one();
    }

    std::shared_ptr<Message> MessageQueue::Peek() {
        std::unique_lock<std::mutex> guard(messages_mtx_);
        if (messages_.empty()) {
            messages_cv_.wait(guard);
        }
        auto msg = messages_.front();
        messages_.pop();
        return msg;
    }

}