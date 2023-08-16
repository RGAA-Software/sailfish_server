//
// Created by RGAA on 2023/8/16.
//

#include "AppContent.h"

#include "settings/Settings.h"

namespace rgaa {

    AppContent::AppContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : QWidget(parent) {
        context_ = ctx;
        settings_ = Settings::Instance();
    }

    AppContent::~AppContent() {

    }

    void AppContent::OnContentShow() {

    }

    void AppContent::OnContentHide() {

    }

}