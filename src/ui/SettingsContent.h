//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_SETTINGSCONTENT_H
#define SAILFISH_SERVER_SETTINGSCONTENT_H

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include "AppContent.h"

namespace rgaa {

    class Settings;
    class SupportedEncoder;
    class EncoderChecker;

    class UIEncoderType {
    public:
        std::string name;
        int type;
    };

    class UICaptureMode {
    public:
        std::string name;
        int type;
    };

    class UIRunningMode {
    public:
        std::string name;
        int type;
    };

    class SettingsContent : public AppContent {
    public:

        explicit SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~SettingsContent() override;

        void OnContentShow() override;
        void OnContentHide() override;

    private:

        Settings* settings_ = nullptr;

        std::vector<UIEncoderType> encoder_types_;
        std::vector<UICaptureMode> capture_modes_;
        std::vector<UIRunningMode> running_modes_;

        QLineEdit* port_et_ = nullptr;
        QCheckBox* multi_client_enabled_cb_ = nullptr;
        QComboBox* encoder_cb_ = nullptr;
        QComboBox* capture_cb_ = nullptr;
        QComboBox* running_cb_ = nullptr;
    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
