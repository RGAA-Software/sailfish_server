//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

namespace rgaa {

    SettingsContent::SettingsContent(QWidget* parent) : AppContent(parent) {
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