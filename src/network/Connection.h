//
// Created by RGAA on 2023/8/15.
//

#ifndef SAILFISH_SERVER_CONNECTION_H
#define SAILFISH_SERVER_CONNECTION_H

#include <memory>
#include <string>

namespace rgaa {

    class Data;
    class Context;
    class MessageProcessor;

    class Connection {
    public:

        Connection(const std::shared_ptr<Context>& ctx, const std::shared_ptr<MessageProcessor>& processor, const std::string& ip, int port);
        virtual ~Connection();

        virtual void Start();
        virtual void Exit();

        virtual void PostTextMessage(const std::string& msg);
        virtual void PostBinaryMessage(const std::shared_ptr<Data>& data);
        virtual void PostBinaryMessage(const std::string& data);

    protected:

        std::shared_ptr<Context> context_ = nullptr;
        std::string ip_{};
        int port_ = 0;
        std::shared_ptr<MessageProcessor> msg_processor_ = nullptr;

    };

}

#endif //SAILFISH_SERVER_CONNECTION_H
