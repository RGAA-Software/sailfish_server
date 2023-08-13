//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CONTEXT_H
#define SAILFISH_SERVER_CONTEXT_H

#include <memory>

namespace rgaa {

    class MessageQueue;
    class WSServer;

    class Context {
    public:

        Context();
        ~Context();

        void Init();

        std::shared_ptr<MessageQueue> GetMessageQueue();

    private:

        std::shared_ptr<MessageQueue> msg_queue_ = nullptr;
        std::shared_ptr<WSServer> ws_server_ = nullptr;

    };

    using ContextPtr = std::shared_ptr<Context>;

}

#endif //SAILFISH_SERVER_CONTEXT_H
