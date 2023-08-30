//
// Created by RGAA on 2023-08-30.
//

#include "AboutContent.h"

#include "AppColorTheme.h"

namespace rgaa {

    AboutContent::AboutContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        int margin = 20;
        root_layout->setContentsMargins(0, margin, margin, margin);

        auto root_widget = new RoundRectWidget(AppColorTheme::kAppBackgroundColor, 10, this);
        root_layout->addWidget(root_widget);

        setLayout(root_layout);

    }

    AboutContent::~AboutContent() {

    }

    void AboutContent::OnContentShow() {

    }

    void AboutContent::OnContentHide() {

    }

}