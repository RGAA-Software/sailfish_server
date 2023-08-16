//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

namespace rgaa {

    SettingsContent::SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();

        //
        auto title = new QLabel(this);
        title->setText(tr("Settings"));
        title->setStyleSheet(R"(font-size:20px; color: #333333;)");
        root_layout->addWidget(title);

        setLayout(root_layout);
    }

    SettingsContent::~SettingsContent() {

    }

    void SettingsContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void SettingsContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}