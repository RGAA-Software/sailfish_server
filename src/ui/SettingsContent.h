//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_SETTINGSCONTENT_H
#define SAILFISH_SERVER_SETTINGSCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class SettingsContent : public AppContent {
    public:

        SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~SettingsContent();

        void OnContentShow() override;
        void OnContentHide() override;
    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
