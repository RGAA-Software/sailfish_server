//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CONTEXT_H
#define SAILFISH_SERVER_CONTEXT_H

#include <memory>
#include <string>
#include <vector>

#include "rgaa_common/RTimer.h"

namespace rgaa {

    class Data;
    class MessageQueue;
    class Connection;
    class Settings;
    class MessageProcessor;
    class Application;

    class Context : public std::enable_shared_from_this<Context> {
    public:

        Context();
        ~Context();

        void Init();
        void InitTimers();

        std::shared_ptr<MessageQueue> GetMessageQueue();
        std::shared_ptr<Connection> GetConnection();
        void EstablishConnection();

        void StartApplication(bool audio);
        void StopApplication();

        void CheckHeartBeat();
        void UpdateHeartBeat(uint64_t time, uint64_t index);

        void PostNetworkBinaryMessage(const std::string& msg);
        void PostNetworkBinaryMessage(const std::shared_ptr<Data>& data);
        void PostNetworkTextMessage(const std::string& msg);

    private:

        std::shared_ptr<MessageQueue> msg_queue_ = nullptr;
        std::shared_ptr<Connection> connection_ = nullptr;
        std::shared_ptr<MessageProcessor> msg_processor_ = nullptr;
        Settings* settings_ = nullptr;
        std::shared_ptr<Application> app_ = nullptr;

        uint64_t heart_beat_time_ = 0;
        uint64_t heart_beat_index_ = 0;

        std::shared_ptr<Timer> timer_ = nullptr;
        std::vector<size_t> timer_ids_;
    };

    using ContextPtr = std::shared_ptr<Context>;

}

#endif //SAILFISH_SERVER_CONTEXT_H
