//
// Created by RGAA on 2023/8/18.
//

#ifndef SAILFISH_SERVER_APPMESSAGES_H
#define SAILFISH_SERVER_APPMESSAGES_H

#include "rgaa_common/RMessage.h"

namespace rgaa {

    constexpr int kMessageCodeTimer1S = 0x1001;

    // send per second
    class Timer1SMessage : public Message {
    public:
        static std::shared_ptr<Message> Make() {
            return std::make_shared<Timer1SMessage>(kMessageCodeTimer1S);
        }

        explicit Timer1SMessage(int code) : Message(code) {}
    };



}

#endif //SAILFISH_SERVER_APPMESSAGES_H
