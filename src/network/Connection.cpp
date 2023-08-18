//
// Created by RGAA on 2023/8/15.
//

#include "Connection.h"

#include "context/Context.h"
#include "AppMessages.h"

namespace rgaa {

    Connection::Connection(const std::shared_ptr<Context> &ctx, const std::shared_ptr<MessageProcessor>& processor, const std::string &ip, int port) {
        this->context_ = ctx;
        this->ip_ = ip;
        this->port_ = port;
        this->msg_processor_ = processor;
    }

    Connection::~Connection() {

    }

    void Connection::Start() {

    }

    void Connection::Exit() {

    }

    void Connection::PostTextMessage(const std::string& msg) {

    }

    void Connection::PostBinaryMessage(const std::shared_ptr<Data>& data) {

    }

    void Connection::PostBinaryMessage(const std::string& data) {

    }

    int Connection::GetConnectionPeerCount() {
       return 0;
    }

    void Connection::NotifyPeerConnected() {
        context_->SendAppMessage(PeerConnectedMessage::Make());
    }

    void Connection::NotifyPeerDisconnected() {
        context_->SendAppMessage(PeerDisconnectedMessage::Make());
    }

}
