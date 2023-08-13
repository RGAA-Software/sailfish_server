//
// Created by RGAA on 2023/8/13.
//

#ifndef SAILFISH_SERVER_MESSAGEPROCESSOR_H
#define SAILFISH_SERVER_MESSAGEPROCESSOR_H

#include <memory>
#include <string>

namespace rgaa {

    class Context;
    class EventReplayer;

    class MessageProcessor {
    public:

        explicit MessageProcessor(const std::shared_ptr<Context>& ctx);
        ~MessageProcessor();

        void ProcessMessage(const std::string& msg);

    private:

        std::shared_ptr<EventReplayer> replayer_ = nullptr;

    };

}

#endif //SAILFISH_SERVER_MESSAGEPROCESSOR_H
