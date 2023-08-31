//
// Created by RGAA on 2023/8/18.
//

#ifndef SAILFISH_SERVER_APPMESSAGES_H
#define SAILFISH_SERVER_APPMESSAGES_H

#include <string>
#include <utility>

#include "rgaa_common/RMessage.h"

namespace rgaa {

    constexpr int kMessageCodeTimer1S = 0x1001;
    constexpr int kMessageIDR = 0x1002;
    constexpr int kPeerConnected = 0x1003;
    constexpr int kPeerDisconnected = 0x1004;
    constexpr int kCodeClipboard = 0x1005;
    constexpr int kCodeSettingsChanged = 0x1006;

    // send per second
    class Timer1SMessage : public Message {
    public:
        static std::shared_ptr<Message> Make() {
            return std::make_shared<Timer1SMessage>(kMessageCodeTimer1S);
        }

        explicit Timer1SMessage(int code) : Message(code) {}
    };

    // Peer connected
    class PeerConnectedMessage : public Message {
    public:
        static std::shared_ptr<Message> Make() {
            return std::make_shared<PeerConnectedMessage>(kPeerConnected);
        }

        explicit PeerConnectedMessage(int c) : Message(c) {}
    };

    // Peer disconnected
    class PeerDisconnectedMessage : public Message {
    public:
        static std::shared_ptr<Message> Make() {
            return std::make_shared<PeerDisconnectedMessage>(kPeerDisconnected);
        }

        explicit PeerDisconnectedMessage(int c) : Message(c) {}
    };

    // IDR
    class IDRMessage : public Message {
    public:

        static std::shared_ptr<Message> Make() {
            return std::make_shared<IDRMessage>(kMessageIDR);
        }

        explicit IDRMessage(int code) : Message(code) {}
    };

    // Clipboard
    class ClipboardMessage : public Message {
    public:
        static std::shared_ptr<Message> Make(const std::string& msg) {
            return std::make_shared<ClipboardMessage>(msg);
        }

        explicit ClipboardMessage(std::string  m) : Message(kCodeClipboard), msg_(std::move(m)) {}

    public:

        std::string msg_;
    };

    // settings changed
    class SettingsChangedMessage : public Message {
    public:
        static std::shared_ptr<Message> Make() {
            return std::make_shared<SettingsChangedMessage>();
        }

        explicit SettingsChangedMessage() : Message(kCodeSettingsChanged) {}
    };
}

#endif //SAILFISH_SERVER_APPMESSAGES_H
