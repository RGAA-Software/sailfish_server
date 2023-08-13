//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_MESSAGEQUEUE_H
#define SAILFISH_SERVER_MESSAGEQUEUE_H

#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace rgaa {

    class Message;

    class MessageQueue {
    public:

        explicit MessageQueue(int max = 1024);
        ~MessageQueue();

        void Queue(const std::shared_ptr<Message>& msg);
        std::shared_ptr<Message> Peek();

    private:

        int max_messages_;
        std::mutex messages_mtx_;
        std::condition_variable messages_cv_;
        std::queue<std::shared_ptr<Message>> messages_;

    };

}

#endif //SAILFISH_SERVER_MESSAGEQUEUE_H
