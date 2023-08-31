//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_SETTINGS_H
#define SAILFISH_SERVER_SETTINGS_H

#include <string>
#include <memory>

namespace rgaa {

    enum class EncoderType {
        kH264,
        kH265,
    };

    enum class CaptureMonitorType {
        kAll,
        kSingle,
    };

    enum class CaptureAPI {
        kDesktopDuplication,
        kWinGraphicsCapture,
    };

    enum class ConnectionMode {
        kDirect,
        kRelay,
    };

    enum class RunningMode {
        kAuto,
        kFix,
    };

    class Context;
    class SharedPreference;

    class Settings {
    public:

        static Settings* Instance() {
            static Settings inst;
            return &inst;
        }

        void SetContext(const std::shared_ptr<Context>& ctx);
        void SetSharedPreference(const std::shared_ptr<SharedPreference>& sp);
        void LoadSettings();

        int GetListenPort() const;
        EncoderType GetEncodeType();
        CaptureMonitorType GetCaptureMonitorType();
        RunningMode GetRunningMode();
        bool IsMultiClientsEnabled() const;
        CaptureAPI GetCaptureAPI();
        int GetPreserveTime() const;

        void SetConnectionMode(const ConnectionMode& mode);
        ConnectionMode GetConnectionMode();

        std::string GetRelayHost();
        int GetRelayPort() const;

    public:
        void SavePort(int port);
        void SaveEncoder(EncoderType encoder);
        void SaveCaptureMonitorType(CaptureMonitorType type);
        void SaveRunningMode(RunningMode mode);
        void SaveEnableMultiClients(bool enable);
        void SavePreserveTime(int time);
        void Dump();

    private:
        int GetPortFromDB();
        EncoderType GetEncoderFromDB();
        CaptureMonitorType GetCaptureMonitorTypeFromDB();
        RunningMode GetRunningModeFromDB();
        bool IsMultiClientsEnabledFromDB();
        int GetPreserveTimeFromDB();

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<SharedPreference> sp_ = nullptr;

        EncoderType encoder_type_ = EncoderType::kH264;
        CaptureMonitorType capture_monitor_type_ = CaptureMonitorType::kAll;
        CaptureAPI capture_api_ = CaptureAPI::kDesktopDuplication;
        ConnectionMode connection_mode_ = ConnectionMode::kDirect;
        RunningMode running_mode_ = RunningMode::kAuto;
        bool enable_multi_clients_ = true;
        int preserve_time_ = 10*60; // Unit : S -> second
        int listen_port_ = 9090;

        std::string relay_host;
        int relay_port = 9092;

    };

}

#endif //SAILFISH_SERVER_SETTINGS_H
