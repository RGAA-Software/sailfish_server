//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CONTEXT_H
#define SAILFISH_SERVER_CONTEXT_H

#include <memory>
#include <string>

namespace rgaa {

    class Data;
    class MessageQueue;
    class Connection;
    class Settings;
    class MessageProcessor;

    class Context : public std::enable_shared_from_this<Context> {
    public:

        Context();
        ~Context();

        void Init();

        std::shared_ptr<MessageQueue> GetMessageQueue();
        std::shared_ptr<Connection> GetConnection();

        void PostNetworkBinaryMessage(const std::string& msg);
        void PostNetworkBinaryMessage(const std::shared_ptr<Data>& data);
        void PostNetworkTextMessage(const std::string& msg);

    private:

        std::shared_ptr<MessageQueue> msg_queue_ = nullptr;
        std::shared_ptr<Connection> connection_ = nullptr;
        std::shared_ptr<MessageProcessor> msg_processor_ = nullptr;
        Settings* settings_ = nullptr;
    };

    using ContextPtr = std::shared_ptr<Context>;

}

#endif //SAILFISH_SERVER_CONTEXT_H
