//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_SETTINGS_H
#define SAILFISH_SERVER_SETTINGS_H

#include <string>

namespace rgaa {

    enum class EncodeType {
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

    class Settings {
    public:

        static Settings* Instance() {
            static Settings inst;
            return &inst;
        }

        void LoadSettings();

        EncodeType GetEncodeType();
        CaptureMonitorType GetCaptureMonitorType();
        CaptureAPI GetCaptureAPI();

        int GetListenPort();
        ConnectionMode GetConnectionMode();

        std::string GetRelayHost();
        int GetRelayPort();


    private:

        EncodeType encode_type_ = EncodeType::kH264;
        CaptureMonitorType capture_monitor_type_ = CaptureMonitorType::kAll;
        CaptureAPI capture_api_ = CaptureAPI::kWinGraphicsCapture;
        ConnectionMode connection_mode_ = ConnectionMode::kDirect;
        int listen_port_ = 9090;

        std::string relay_host;
        int relay_port = 9092;

    };

}

#endif //SAILFISH_SERVER_SETTINGS_H
