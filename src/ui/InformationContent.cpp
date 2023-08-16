//
// Created by RGAA on 2023/8/16.
//

#include "InformationContent.h"

namespace rgaa {

    InformationContent::InformationContent(QWidget* parent) : AppContent(parent) {
        auto root_layout = new QVBoxLayout();

        //
        auto title = new QLabel(this);
        title->setText(tr("Information"));
        title->setStyleSheet(R"(font-size:20px; color: #333333;)");
        root_layout->addWidget(title);

        setLayout(root_layout);
    }

    InformationContent::~InformationContent() {

    }

    void InformationContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void InformationContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}