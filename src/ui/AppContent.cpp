//
// Created by RGAA on 2023/8/16.
//

#include "AppContent.h"

#include "src/context/Settings.h"

namespace rgaa {

    AppContent::AppContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : RoundRectWidget(0xffffff, 0, parent) {
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