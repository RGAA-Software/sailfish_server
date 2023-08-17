//
// Created by RGAA on 2023/8/10.
//

#include "Settings.h"

namespace rgaa {

    void Settings::LoadSettings() {

    }

    EncodeType Settings::GetEncodeType() {
        return encode_type_;
    }

    CaptureMonitorType Settings::GetCaptureMonitorType() {
        return capture_monitor_type_;
    }

    CaptureAPI Settings::GetCaptureAPI() {
        return capture_api_;
    }

    int Settings::GetListenPort() {
        return listen_port_;
    }

    void Settings::SetConnectionMode(const ConnectionMode& mode) {
        connection_mode_ = mode;
    }

    ConnectionMode Settings::GetConnectionMode() {
        return connection_mode_;
    }

    std::string Settings::GetRelayHost() {
        return relay_host;
    }

    int Settings::GetRelayPort() {
        return relay_port;
    }

}