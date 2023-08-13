//
// Created by RGAA on 2023/8/10.
//

#include "Settings.h"

#include "encoder/EncoderChecker.h"

namespace rgaa {

    void Settings::LoadSettings() {
        EncoderChecker::Instance()->CheckSupportedEncoders();
        EncoderChecker::Instance()->DumpSupportedEncoders();
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

}