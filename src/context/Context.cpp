//
// Created by RGAA on 2023/8/10.
//

#include "Context.h"

#include "network/WSServer.h"
#include "Settings.h"
#include "network/MessageProcessor.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RThread.h"
#include "rgaa_common/RMessage.h"
#include "rgaa_common/RMessageQueue.h"
#include "Application.h"
#include "encoder/EncoderChecker.h"
#include "AppMessages.h"
#include "ui/ClipboardManager.h"
#include "Statistics.h"
#include "SharedPreference.h"

#include "messages.pb.h"

namespace rgaa {

    Context::Context() {
        settings_ = Settings::Instance();
        encoder_checker_ = std::make_shared<EncoderChecker>();
    }

    Context::~Context() {
        if (!timer_ids_.empty() && timer_) {
            for (const auto& tid : timer_ids_) {
                timer_->remove(tid);
            }
        }
        if (msg_queue_) {
            msg_queue_->Exit();
        }
        if (msg_thread_ && msg_thread_->IsJoinable()) {
            msg_thread_->Join();
        }
        if (connection_) {
            connection_->Exit();
        }

        this->RemoveMessageTask(setting_changed_task_id_);
    }

    void Context::Init() {
        sp_ = std::make_shared<SharedPreference>();
        sp_->Init("", "sp.data");
        settings_->SetContext(shared_from_this());
        settings_->SetSharedPreference(sp_);
        settings_->LoadSettings();
        settings_->Dump();

        statistics_ = std::make_shared<Statistics>(shared_from_this());

        task_thread_ = std::make_shared<Thread>("task_thread", 128);
        task_thread_->Poll();

        msg_queue_ = std::make_shared<MessageQueue>();
        msg_thread_ = std::make_shared<Thread>([=, this]() {
            msg_queue_->PollBlocked();
        }, "msg_thread", false);

        auto self = shared_from_this();
        msg_processor_ = std::make_shared<MessageProcessor>(self);

        clipboard_manager_ = std::make_shared<ClipboardManager>(self);
        clipboard_manager_->Init();

        encoder_checker_->CheckSupportedEncoders();
        encoder_checker_->DumpSupportedEncoders();

        setting_changed_task_id_ = this->RegisterMessageTask(MessageTask::Make(kCodeSettingsChanged, [=, this](auto& msg) {
            this->StopApplication();
            this->EstablishConnection();
        }));

        InitTimers();

        EstablishConnection();
    }

    void Context::InitTimers() {
        timer_ = std::make_shared<Timer>();
        auto timer_1s_id = timer_->add(std::chrono::milliseconds(10), [=, this](rgaa::timer_id){
            task_thread_->Post(SimpleThreadTask::Make([=, this] () {
                auto msg = Timer1SMessage::Make();
                msg_queue_->Queue(msg);

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
            LOGI("App already running...");
            return;
        }

        app_thread_ = std::make_shared<Thread>([=, this]() {
            app_ = std::make_shared<Application>(shared_from_this(), audio);
            app_->Start();
            LOGI("Application exit...");
        }, "app_thread", false);
    }

    void Context::StopApplication() {
        if (clipboard_manager_) {
            clipboard_manager_->Exit();
        }

        if (app_) {
            app_->Exit();
            app_.reset();
            app_ = nullptr;
        }

        if (app_thread_ && app_thread_->IsJoinable()) {
            app_thread_->Join();
            app_thread_.reset();
            app_thread_ = nullptr;
            LOGI("App thread exit...");
        }
    }

    void Context::CheckHeartBeat() {
        auto current_time = GetCurrentTimestamp();
        int preserve_time = settings_->GetPreserveTime() * 1000; // to [ms]
        if (current_time - heart_beat_time_ > preserve_time && heart_beat_time_ > 0) {
            heart_beat_time_ = 0;
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

    int Context::RegisterMessageTask(const std::shared_ptr<MessageTask>& task) {
        if (msg_queue_) {
            return msg_queue_->RegisterTask(task);
        }
        return -1;
    }

    void Context::RemoveMessageTask(int task_id) {
        if (msg_queue_) {
            msg_queue_->RemoveTask(task_id);
        }
    }

    void Context::SendAppMessage(const std::shared_ptr<Message>& msg) {
        if (msg_queue_) {
            msg_queue_->Queue(msg);
        }
    }

    std::shared_ptr<Statistics> Context::GetStatistics() {
        return statistics_;
    }

    void Context::EnableClipboard() {
        clipboard_manager_->Enable();
    }

    void Context::DisableClipboard() {
        clipboard_manager_->Disable();
    }

    void Context::EnableAudio() {
        audio_enabled_ = true;
    }

    void Context::DisableAudio() {
        audio_enabled_ = false;
    }

    bool Context::IsAudioEnabled() {
        return audio_enabled_;
    }

    std::shared_ptr<SharedPreference> Context::GetSP() {
        return sp_;
    }

    void Context::SaveToSP(const std::string& k, const std::string& v) {
        sp_->Put(k, v);
    }

    std::string Context::GetFromSP(const std::string& k) {
        return sp_->Get(k);
    }

    int Context::GetFromSPAsInt(const std::string& k) {
        auto value = sp_->Get(k);
        if (value.empty()) {
            return 0;
        }
        return std::atoi(value.c_str());
    }

}

