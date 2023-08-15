//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_SETTINGS_H
#define SAILFISH_SERVER_SETTINGS_H

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

    private:

        EncodeType encode_type_ = EncodeType::kH264;
        CaptureMonitorType capture_monitor_type_ = CaptureMonitorType::kAll;
        CaptureAPI capture_api_ = CaptureAPI::kWinGraphicsCapture;
    };

}

#endif //SAILFISH_SERVER_SETTINGS_H
