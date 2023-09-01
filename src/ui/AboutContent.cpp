//
// Created by RGAA on 2023-08-30.
//

#include "AboutContent.h"

#include "AppColorTheme.h"
#include "WidgetHelper.h"

namespace rgaa {

    AboutContent::AboutContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        int margin = 20;
        root_layout->setContentsMargins(0, margin, margin, margin);

        auto root_widget = new RoundRectWidget(AppColorTheme::kAppBackgroundColor, 10, this);
        root_layout->addWidget(root_widget);

        auto content_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(content_layout);
        content_layout->addSpacing(50);

        {
            auto title_layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(title_layout);
            title_layout->addStretch();

            auto title = new QLabel(this);
            title_layout->addWidget(title);
            title->setText(tr("LIFE WITH YOU"));
            title->setStyleSheet("font-size: 30pt; font-family: 'Demon Blade';");
            title_layout->addStretch();
            content_layout->addLayout(title_layout);
        }

        content_layout->addSpacing(50);

        {
            auto poem_layout = new QHBoxLayout();
            poem_layout->addSpacing(100);
            WidgetHelper::ClearMargin(poem_layout);

            auto poem = new QLabel(this);
            std::stringstream ss;
            ss << "Dear angel and sweet monster" << std::endl;
            ss << "You are pushed to me by the mysterious power with happiness" << std::endl;
            ss << "Cute face like pink marshmallow" << std::endl;
            ss << "Small hands became magic sticks" << std::endl;
            ss << "Rescue me from the hell of illness everyday" << std::endl;
            poem->setText(ss.str().c_str());
            poem->setStyleSheet("font-size: 25pt; font-family: 'Demon Blade';");

            poem_layout->addWidget(poem);
            poem_layout->addStretch();
            content_layout->addLayout(poem_layout);

        }

        content_layout->addStretch();
        {
            auto title_layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(title_layout);
            title_layout->addStretch();

            auto title = new QLabel(this);
            title_layout->addWidget(title);
            title->setText(tr("BEST"));
            title->setStyleSheet("font-size: 20pt; font-family: 'Demon Blade';");
            title_layout->addSpacing(30);
            content_layout->addLayout(title_layout);
            content_layout->addSpacing(30);
        }
        root_widget->setLayout(content_layout);

        setLayout(root_layout);

    }

    AboutContent::~AboutContent() {

    }

    void AboutContent::OnContentShow() {

    }

    void AboutContent::OnContentHide() {

    }

}