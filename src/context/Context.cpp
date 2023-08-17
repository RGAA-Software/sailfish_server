//
// Created by RGAA on 2023/8/10.
//

#include "Context.h"

#include "Message.h"
#include "MessageQueue.h"
#include "network/WSServer.h"
#include "settings/Settings.h"
#include "network/MessageProcessor.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RThread.h"
#include "Application.h"
#include "encoder/EncoderChecker.h"
#include "messages.pb.h"

namespace rgaa {

    Context::Context() {
        settings_ = Settings::Instance();
        encoder_checker_ = std::make_shared<EncoderChecker>();
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
        task_thread_ = std::make_shared<Thread>("task_thread", 128);
        task_thread_->Poll();

        msg_queue_ = std::make_shared<MessageQueue>();
        auto self = shared_from_this();
        msg_processor_ = std::make_shared<MessageProcessor>(self);

        encoder_checker_->CheckSupportedEncoders();
        encoder_checker_->DumpSupportedEncoders();

        InitTimers();

        EstablishConnection();
    }

    void Context::InitTimers() {
        timer_ = std::make_shared<Timer>();
        auto timer_1s_id = timer_->add(std::chrono::milliseconds(10), [=, this](rgaa::timer_id){
            task_thread_->Post(SimpleThreadTask::Make([=, this] () {
                CheckHeartBeat();
            }));
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
        heart_beat_time_ = GetCurrentTimestamp();
        if (app_) {
            LOGI("App exit, return.");
            return;
        }
        LOGI("111");

        app_thread_ = std::make_shared<Thread>([=, this]() {
            app_ = std::make_shared<Application>(shared_from_this(), audio);
            app_->Start();
            LOGI("Application exit...");
        }, "", false);
    }

    void Context::StopApplication() {
        if (app_) {
            app_->Exit();
            app_.reset();
            app_ = nullptr;
        }
        if (app_thread_ && app_thread_->IsJoinable()) {
            app_thread_->Join();
            LOGI("after app thread ...");
        }
    }

    void Context::CheckHeartBeat() {
        auto current_time = GetCurrentTimestamp();
        if (current_time - heart_beat_time_ > 5000 && heart_beat_time_ > 0) {
            heart_beat_time_ = 0;
            // close the application
            LOGI("Will stop application.");
            StopApplication();
        }
    }

    void Context::UpdateHeartBeat(uint64_t time, uint64_t index) {
        heart_beat_time_ = time;
        heart_beat_index_ = index;
    }

    void Context::PostNetworkBinaryMessage(const std::shared_ptr<NetMessage>& msg) {
        task_thread_->Post(SimpleThreadTask::Make([=, this] () {
            if (connection_) {
                connection_->PostBinaryMessage(msg->SerializeAsString());
            }
        }));
    }

    void Context::PostNetworkBinaryMessage(const std::shared_ptr<Data>& data) {
        task_thread_->Post(SimpleThreadTask::Make([=, this]() {
            if (connection_) {
                connection_->PostBinaryMessage(data);
            }
        }));
    }

    std::shared_ptr<EncoderChecker> Context::GetEncoderChecker() {
        return encoder_checker_;
    }

    bool Context::HasConnectedPeer() {
        return connection_ && connection_->GetConnectionPeerCount() > 0;
    }

    void Context::PostTask(std::function<void()>&& task) {
        task_thread_->Post(SimpleThreadTask::Make(std::move(task)));
    }

}
