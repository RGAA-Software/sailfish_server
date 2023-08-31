//
// Created by RGAA on 2023/8/16.
//

#include "SettingsContent.h"

#include "AppColorTheme.h"
#include "WidgetHelper.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QIntValidator>

#include "context/Context.h"
#include "context/Settings.h"
#include "encoder/EncoderChecker.h"
#include "rgaa_common/RLog.h"
#include "AppMessages.h"

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

        settings_ = Settings::Instance();

        auto enc_checker = context_->GetEncoderChecker();

        // encoder type
        encoder_types_ = {
            UIEncoderType {
                .name = "H264",
                .type = static_cast<int>(EncoderType::kH264),
            },
            UIEncoderType{
                .name = "H265",
                .type = static_cast<int>(EncoderType::kH265),
            },
        };

        // capture mode
        capture_modes_ = {
            UICaptureMode {
                .name = "Single Monitor",
                .type = static_cast<int>(CaptureMonitorType::kSingle),
            },
            UICaptureMode {
                .name = "All Monitors",
                .type = static_cast<int>(CaptureMonitorType::kAll),
            }
        };

        // running mode
        running_modes_ = {
            UIRunningMode {
                .name = "Auto FPS",
                .type = static_cast<int>(RunningMode::kAuto),
            },
            UIRunningMode {
                .name = "Try Fixing FPS",
                .type = static_cast<int>(RunningMode::kFix),
            },
        };

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
            port_et_ = value;
            auto validator = new QIntValidator(value);
            value->setValidator(validator);
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            value->setText(QString::number(settings_->GetListenPort()));

            auto apply = new QPushButton(this);
            apply->hide();
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
            encoder_cb_ = value;
            for (auto& encoder : encoder_types_) {
                value->addItem(encoder.name.c_str());
            }
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            auto current_encoder_type = settings_->GetEncodeType();
            LOGI("load encoder: {}", (int)current_encoder_type);
            int idx = 0;
            for (auto& encoder : encoder_types_) {
                if (encoder.type == (int)current_encoder_type) {
                    break;
                }
                idx++;
            }
            value->setCurrentIndex(idx);

            auto apply = new QPushButton(this);
            apply->hide();
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
            label->setText(tr("CAPTURE MODE"));
            label->setFixedSize(label_width, 30);
            item_layout->addWidget(label);

            auto value = new QComboBox(this);
            capture_cb_ = value;
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);

            for (auto& capture : capture_modes_) {
                value->addItem(capture.name.c_str());
            }

            int idx = 0;
            auto current_capture_type = settings_->GetCaptureMonitorType();
            for (auto& capture : capture_modes_) {
                if (capture.type == (int)current_capture_type) {
                    break;
                }
                idx++;
            }
            value->setCurrentIndex(idx);

            auto apply = new QPushButton(this);
            apply->hide();
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
            running_cb_ = value;
            value->setFixedSize(label_width, 32);
            item_layout->addWidget(value);
            for (auto& running: running_modes_) {
                value->addItem(running.name.c_str());
            }

            auto current_running_mode = settings_->GetRunningMode();
            int idx = 0;
            for (auto& running: running_modes_) {
                if (running.type == (int)current_running_mode) {
                    break;
                }
                idx++;
            }
            running_cb_->setCurrentIndex(idx);

            auto apply = new QPushButton(this);
            apply->hide();
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
            multi_client_enabled_cb_ = value;
            value->setChecked(settings_->IsMultiClientsEnabled());
            value->setText(tr("multiple clients connect at the same time"));
            value->setStyleSheet("font-size: 14px;");
            value->setFixedSize(label_width * 2, 32);
            item_layout->addWidget(value);

            item_layout->addStretch();
            content_layout->addLayout(item_layout);
        }
        content_layout->addStretch();

        {
            auto layout = new QHBoxLayout();
            WidgetHelper::ClearMargin(layout);
            layout->addSpacing(20);

            auto apply = new QPushButton(this);
            layout->addWidget(apply);
            layout->addStretch();
            apply->setText(tr("APPLY"));
            apply->setFixedSize(130, 35);
            content_layout->addLayout(layout);
            content_layout->addSpacing(20);

            connect(apply, &QPushButton::clicked, this, [=]() {
                // port
                int port = port_et_->text().toInt();
                settings_->SavePort(port);

                // encoder type
                int encoder_idx = encoder_cb_->currentIndex();
                auto encoder = static_cast<EncoderType>(encoder_types_[encoder_idx].type);
                settings_->SaveEncoder(encoder);

                // capture mode
                int capture_idx = capture_cb_->currentIndex();
                auto capture = capture_modes_[capture_idx].type;
                settings_->SaveCaptureMonitorType(static_cast<CaptureMonitorType>(capture));

                // running mode
                int running_idx = running_cb_->currentIndex();
                auto running = running_modes_[running_idx].type;
                settings_->SaveRunningMode(static_cast<RunningMode>(running));

                // multi clients
                settings_->SaveEnableMultiClients(multi_client_enabled_cb_->isChecked());

                // notify ....
                auto msg = SettingsChangedMessage::Make();
                context_->SendAppMessage(msg);

            });
        }

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