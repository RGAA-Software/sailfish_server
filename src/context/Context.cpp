//
// Created by RGAA on 2023/8/10.
//

#include "Context.h"

#include "MessageQueue.h"
#include "network/WSServer.h"
#include "settings/Settings.h"
#include "network/MessageProcessor.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"

namespace rgaa {

    Context::Context() {
        settings_ = Settings::Instance();
    }

    Context::~Context() {

    }

    void Context::Init() {
        msg_queue_ = std::make_shared<MessageQueue>();
        auto self = shared_from_this();
        msg_processor_ = std::make_shared<MessageProcessor>(self);

        if (settings_->GetConnectionMode() == ConnectionMode::kDirect) {
            LOGI("Running network mode : {}, port : {}", (int)settings_->GetConnectionMode(), settings_->GetListenPort());
            connection_ = std::make_shared<WSServer>(self, msg_processor_, "0.0.0.0", settings_->GetListenPort());
        }
        else if (settings_->GetConnectionMode() == ConnectionMode::kRelay) {

        }
        connection_->Start();
    }

    std::shared_ptr<MessageQueue> Context::GetMessageQueue() {
        return msg_queue_;
    }

    std::shared_ptr<Connection> Context::GetConnection() {
        return connection_;
    }

    void Context::PostNetworkBinaryMessage(const std::string& msg) {
        if (connection_) {
            connection_->PostBinaryMessage(msg);
        }
    }

    void Context::PostNetworkBinaryMessage(const std::shared_ptr<Data>& data) {
        if (connection_) {
            connection_->PostBinaryMessage(data);
        }
    }

    void Context::PostNetworkTextMessage(const std::string& msg) {
        if (connection_) {
            connection_->PostTextMessage(msg);
        }
    }

}
