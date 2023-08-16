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
#include "rgaa_common/RTime.h"
#include "Application.h"

namespace rgaa {

    Context::Context() {
        settings_ = Settings::Instance();
    }

    Context::~Context() {
        if (connection_) {
            connection_->Exit();
        }
        if (!timer_ids_.empty() && timer_) {
            for (const auto& tid : timer_ids_) {
                timer_->remove(tid);
            }
        }
    }

    void Context::Init() {
        msg_queue_ = std::make_shared<MessageQueue>();
        auto self = shared_from_this();
        msg_processor_ = std::make_shared<MessageProcessor>(self);

        InitTimers();

        EstablishConnection();
    }

    void Context::InitTimers() {
        timer_ = std::make_shared<Timer>();
        auto timer_1s_id = timer_->add(std::chrono::milliseconds(10), [=, this](rgaa::timer_id){
            CheckHeartBeat();
        }, std::chrono::seconds(1));
        timer_ids_.push_back(timer_1s_id);
    }

    std::shared_ptr<MessageQueue> Context::GetMessageQueue() {
        return msg_queue_;
    }

    std::shared_ptr<Connection> Context::GetConnection() {
        return connection_;
    }

    void Context::EstablishConnection() {
        if (connection_) {
            connection_->Exit();
            connection_.reset();
        }

        if (settings_->GetConnectionMode() == ConnectionMode::kDirect) {
            LOGI("Running network mode : {}, port : {}", (int)settings_->GetConnectionMode(), settings_->GetListenPort());
            connection_ = std::make_shared<WSServer>(shared_from_this(), msg_processor_, "0.0.0.0", settings_->GetListenPort());
        }
        else if (settings_->GetConnectionMode() == ConnectionMode::kRelay) {
            LOGI("Relay not impl now.");
        }

        if (connection_) {
            connection_->Start();
        }
    }

    void Context::StartApplication(bool audio) {
        if (app_) {
            LOGI("App exit, return.");
            return;
        }
        LOGI("111");
        StopApplication();

        app_ = std::make_shared<Application>(shared_from_this(), audio);
        app_->Start();
        LOGI("Start application...");
    }

    void Context::StopApplication() {
        if (app_) {
            LOGI("222");
            app_->Exit();
            app_.reset();
            app_ = nullptr;
            LOGI("Stop application...");
        }
    }

    void Context::CheckHeartBeat() {
        auto current_time = GetCurrentTimestamp();
        if (current_time - heart_beat_time_ > 10000 && heart_beat_time_ > 0) {
            // close the application
            StopApplication();
        }
    }

    void Context::UpdateHeartBeat(uint64_t time, uint64_t index) {
        heart_beat_time_ = time;
        heart_beat_index_ = index;
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
