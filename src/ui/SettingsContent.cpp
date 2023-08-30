//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

#include "AppColorTheme.h"
#include "WidgetHelper.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>

namespace rgaa {

    SettingsContent::SettingsContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QVBoxLayout();
        int margin = 20;
        root_layout->setContentsMargins(0, margin, margin, margin);

        auto root_widget = new RoundRectWidget(AppColorTheme::kAppBackgroundColor, 10, this);
        root_layout->addWidget(root_widget);

        auto content_layout = new QVBoxLayout();
        content_layout->addSpacing(20);
        WidgetHelper::ClearMargin(content_layout);
        root_widget->setLayout(content_layout);

        auto label_width = 150;

        // port
        {
            auto item_layout = new QHBoxLayout();
            item_layout->addSpacing(20);
            WidgetHelper::ClearMargin(item_layout);
            auto label = new QLabel(this);
            label->setText(tr("PORT"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QLineEdit(this);
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            auto apply = new QPushButton(this);
            apply->setText(tr("APPLY"));
            apply->setFixedSize(90, 35);
            item_layout->addSpacing(60);
            item_layout->addWidget(apply);
            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }

        content_layout->addSpacing(10);

        {
            auto item_layout = new QHBoxLayout();
            item_layout->addSpacing(20);
            WidgetHelper::ClearMargin(item_layout);
            auto label = new QLabel(this);
            label->setText(tr("ENCODER"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QComboBox(this);
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            auto apply = new QPushButton(this);
            apply->setText(tr("APPLY"));
            apply->setFixedSize(90, 35);
            item_layout->addSpacing(60);
            item_layout->addWidget(apply);
            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }

        content_layout->addSpacing(10);

        {
            auto item_layout = new QHBoxLayout();
            item_layout->addSpacing(20);
            WidgetHelper::ClearMargin(item_layout);
            auto label = new QLabel(this);
            label->setText(tr("CAPTURE"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QComboBox(this);
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            auto apply = new QPushButton(this);
            apply->setText(tr("APPLY"));
            apply->setFixedSize(90, 35);
            item_layout->addSpacing(60);
            item_layout->addWidget(apply);
            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }

        content_layout->addSpacing(10);

        {
            auto item_layout = new QHBoxLayout();
            item_layout->addSpacing(20);
            WidgetHelper::ClearMargin(item_layout);
            auto label = new QLabel(this);
            // fix fps / accessible
            label->setText(tr("RUNNING MODE"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QComboBox(this);
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            auto apply = new QPushButton(this);
            apply->setText(tr("APPLY"));
            apply->setFixedSize(90, 35);
            item_layout->addSpacing(60);
            item_layout->addWidget(apply);
            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }

        content_layout->addSpacing(10);

        {
            auto item_layout = new QHBoxLayout();
            item_layout->addSpacing(20);
            WidgetHelper::ClearMargin(item_layout);
            auto label = new QLabel(this);
            // fix fps / accessible
            label->setText(tr("MULTI CLIENTS"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QCheckBox(this);
            value->setText(tr("multiple clients connect at the same time"));
            value->setStyleSheet("font-size: 14px;");
            value->setFixedSize(label_width * 2, 32);
            item_layout->addWidget(value);

            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }
        content_layout->addStretch();

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