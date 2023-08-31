//
// Created by RGAA on 2023/8/10.
//

#include "Settings.h"
#include "SharedPreference.h"
#include "rgaa_common/RLog.h"
#include "encoder/EncoderChecker.h"
#include "Context.h"

static const std::string kKeyInit = "key_init";

static const std::string kKeyPort = "key_port";
static const std::string kKeyEncoder = "key_encoder";
static const std::string kKeyCaptureMonitorType = "key_capture_monitor_type";
static const std::string kKeyRunningMode = "key_running_mode";
static const std::string kKeyEnableMultiClients = "key_enable_multi_clients";
static const std::string kKeyPreserveTime = "key_preserve_time";

namespace rgaa {

    void Settings::SetSharedPreference(const std::shared_ptr<SharedPreference>& sp) {
        this->sp_ = sp;
    }

    void Settings::SetContext(const std::shared_ptr<Context>& ctx) {
        this->context_ = ctx;
    }

    void Settings::LoadSettings() {
        std::string value = sp_->Get(kKeyInit);
        //value = "";
        if (value.empty()) {
            // init ...
            sp_->Put(kKeyInit, "inited");

            SavePort(listen_port_);
            SaveEncoder(encoder_type_);
            SaveCaptureMonitorType(capture_monitor_type_);
            SaveRunningMode(running_mode_);
            SaveEnableMultiClients(enable_multi_clients_);
            SavePreserveTime(preserve_time_);
        }
        else {
            // load from database...
            LOGI("Load settings from database...");
            GetPortFromDB();
            GetEncoderFromDB();
            GetCaptureMonitorTypeFromDB();
            GetRunningModeFromDB();
            IsMultiClientsEnabledFromDB();
            GetPreserveTimeFromDB();
        }
    }

    EncoderType Settings::GetEncodeType() {
        return encoder_type_;
    }

    CaptureMonitorType Settings::GetCaptureMonitorType() {
        return capture_monitor_type_;
    }

    CaptureAPI Settings::GetCaptureAPI() {
        return capture_api_;
    }

    int Settings::GetListenPort() const {
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

    int Settings::GetRelayPort() const {
        return relay_port;
    }

    RunningMode Settings::GetRunningMode() {
        return running_mode_;
    }

    bool Settings::IsMultiClientsEnabled() const {
        return enable_multi_clients_;
    }

    int Settings::GetPreserveTime() const {
        return preserve_time_;
    }

    // --------- Put ---------

    void Settings::SavePort(int port) {
        listen_port_ = port;
        sp_->Put(kKeyPort, std::to_string(port));
    }

    void Settings::SaveEncoder(EncoderType encoder) {
        encoder_type_ = encoder;
        sp_->Put(kKeyEncoder, std::to_string((int)encoder));
        LOGI("the encode int db: {}", sp_->Get(kKeyEncoder));
    }

    void Settings::SaveCaptureMonitorType(CaptureMonitorType type) {
        capture_monitor_type_ = type;
        sp_->Put(kKeyCaptureMonitorType, std::to_string((int)type));
    }

    void Settings::SaveRunningMode(RunningMode mode) {
        running_mode_ = mode;
        sp_->Put(kKeyRunningMode, std::to_string((int)mode));
    }

    void Settings::SaveEnableMultiClients(bool enable) {
        enable_multi_clients_ = enable;
        sp_->Put(kKeyEnableMultiClients, std::to_string((int)enable));
    }

    void Settings::SavePreserveTime(int time) {
        preserve_time_ = time;
        sp_->Put(kKeyPreserveTime, std::to_string(time));
    }

    // --------- Get ---------

    int Settings::GetPortFromDB() {
        std::string value = sp_->Get(kKeyPort);
        listen_port_ = std::atoi(value.c_str());
        return listen_port_;
    }

    EncoderType Settings::GetEncoderFromDB() {
        auto value = sp_->Get(kKeyEncoder);
        encoder_type_ = static_cast<EncoderType>(std::atoi(value.c_str()));
        return encoder_type_;
    }

    CaptureMonitorType Settings::GetCaptureMonitorTypeFromDB() {
        std::string value = sp_->Get(kKeyCaptureMonitorType);
        capture_monitor_type_ = static_cast<CaptureMonitorType>(std::atoi(value.c_str()));
        return capture_monitor_type_;
    }

    RunningMode Settings::GetRunningModeFromDB() {
        auto value = sp_->Get(kKeyRunningMode);
        running_mode_ = static_cast<RunningMode>(std::atoi(value.c_str()));
        return running_mode_;
    }

    bool Settings::IsMultiClientsEnabledFromDB() {
        auto value = sp_->Get(kKeyEnableMultiClients);
        enable_multi_clients_ = (bool)std::atoi(value.c_str());
        return enable_multi_clients_;
    }

    int Settings::GetPreserveTimeFromDB() {
        auto value = sp_->Get(kKeyPreserveTime);
        preserve_time_ = std::atoi(value.c_str());
        return preserve_time_;
    }

    void Settings::Dump() {
        std::stringstream ss;
        ss << "Settings: " << std::endl;
        ss << "Port: " << listen_port_ << std::endl;
        ss << "Encoder: " << (int)encoder_type_ << std::endl;
        ss << "Capture monitor type: " << (int)capture_monitor_type_ << std::endl;
        ss << "Running mode: " << (int)running_mode_ << std::endl;
        ss << "Enable multi clients: " << enable_multi_clients_ << std::endl;
        ss << "Preserve time: " << preserve_time_ << " S" << std::endl;
        auto msg = ss.str();
        LOGI("\n- - - - - - - - - - - - - - - - - - - - - - -\n{}- - - - - - - - - - - - - - - - - - - - - - -", msg);
    }
}