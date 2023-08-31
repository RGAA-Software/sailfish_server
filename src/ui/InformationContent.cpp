//
// Created by RGAA on 2023/8/16.
//

#include "InformationContent.h"

#include "util/IPUtil.h"
#include "src/context/Settings.h"
#include "rgaa_common/RLog.h"
#include "context/Context.h"
#include "WidgetHelper.h"
#include "AppColorTheme.h"
#include "AppMessages.h"
#include "rgaa_common/RMessageQueue.h"

#include <QPainter>

namespace rgaa {

    InformationContent::InformationContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QHBoxLayout();
        WidgetHelper::ClearMargin(root_layout);
        int margin = 20;
        root_layout->setContentsMargins(0, margin, margin, margin);

        // 1. local settings
        auto local_settings_layout = new QVBoxLayout();
        WidgetHelper::ClearMargin(local_settings_layout);

        int bg_color = 0xEAF7FF;

        auto server_mode_widget = new RoundRectWidget(bg_color, 10, this);
        server_mode_widget->setLayout(local_settings_layout);
        root_layout->addWidget(server_mode_widget);
        //
        {
            auto title = new QLabel(this);
            title->setAlignment(Qt::AlignCenter);
            title->setText(tr("SERVER MODE"));
            title->setStyleSheet(R"(font-size:20px; color: #386487;)");
            local_settings_layout->addSpacing(20);
            local_settings_layout->addWidget(title);
        }
        {
            // enable this mode
            auto cb = new QCheckBox(this);
            cb->setEnabled(false);
            cb->setStyleSheet("padding-left:38px; font-weight:bold;");
            cb_server_mode_ = cb;
            cb->setText(tr("Enable server mode"));
            local_settings_layout->addSpacing(40);
            local_settings_layout->addWidget(cb);
            connect(cb_server_mode_, &QCheckBox::clicked, this, [=, this](bool clicked) {
                LOGI("Server CB : {}", clicked);
                if (cb_relay_mode_->isChecked()) {
                    cb_relay_mode_->setChecked(false);
                }

                settings_->SetConnectionMode(ConnectionMode::kDirect);
                context_->EstablishConnection();
            });
        }
        // ips
        {
            local_settings_layout->addSpacing(30);
            auto ips = IPUtil::ScanIPs();
            auto ip_title = new QLabel(this);
            ip_title->setStyleSheet(R"(font-size:14px; color: #386487; padding-left:35px;)");
            ip_title->setText(tr("IP ACCESSIBLE:"));
            local_settings_layout->addWidget(ip_title);

            for (const auto& ip : ips) {
                auto ip_value = new QLabel(this);
                ip_value->setText(ip.first.c_str());
                ip_value->setStyleSheet(R"(font-size:26px; font-family:ScreenMatrix; color: #386487; padding-left:31px;)");
                local_settings_layout->addSpacing(10);
                local_settings_layout->addWidget(ip_value);
            }
        }

        {
            local_settings_layout->addSpacing(30);
            auto port_title = new QLabel(this);
            port_title->setStyleSheet(R"(font-size:14px; color: #386487; padding-left:35px;)");
            port_title->setText(tr("PORT LISTENING:"));
            local_settings_layout->addWidget(port_title);

            auto port_value = new QLabel(this);
            port_label_ = port_value;
            auto port = std::to_string(Settings::Instance()->GetListenPort());
            port_value->setText(port.c_str());
            port_value->setStyleSheet(R"(font-size:26px; font-family:ScreenMatrix; color: #386487; padding-left:31px;)");
            local_settings_layout->addSpacing(10);
            local_settings_layout->addWidget(port_value);
        }

        local_settings_layout->addStretch();

        root_layout->addSpacing(margin);

        // 2. relay settings

        auto relay_settings_layout = new QVBoxLayout();
        relay_settings_layout->addSpacing(20);
        WidgetHelper::ClearMargin(relay_settings_layout);
        auto relay_mode_widget = new RoundRectWidget(bg_color, 10, this);
        relay_mode_widget->setLayout(relay_settings_layout);
        root_layout->addWidget(relay_mode_widget);

        {
            auto title = new QLabel(this);
            title->setAlignment(Qt::AlignCenter);
            title->setText(tr("RELAY MODE(DEVELOPING)"));
            title->setStyleSheet(R"(font-size:20px; color: #386487;)");
            relay_settings_layout->addWidget(title);
        }
        {
            // enable this mode
            auto cb = new QCheckBox(this);
            cb_relay_mode_ = cb;
            cb->setEnabled(false);
            cb->setStyleSheet("padding-left:38px; font-weight:bold;");
            cb->setText(tr("Enable Relay Mode"));
            relay_settings_layout->addSpacing(40);
            relay_settings_layout->addWidget(cb);
            connect(cb_relay_mode_, &QCheckBox::clicked, this, [=, this](bool clicked) {
                LOGI("Relay CB : {}", clicked);
                if (cb_server_mode_->isChecked()) {
                    cb_server_mode_->setChecked(false);
                }

                settings_->SetConnectionMode(ConnectionMode::kRelay);
                context_->EstablishConnection();
            });
        }

        relay_settings_layout->addStretch();

        setLayout(root_layout);

        //
        auto settings = Settings::Instance();
        if (settings->GetConnectionMode() == ConnectionMode::kDirect) {
            cb_server_mode_->setChecked(true);
            cb_relay_mode_->setChecked(false);
        }
        else if (settings->GetConnectionMode() == ConnectionMode::kRelay) {
            cb_relay_mode_->setChecked(true);
            cb_server_mode_->setChecked(false);
        }

        //
        setting_changed_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeSettingsChanged, [=, this](auto& msg) {
            int port = settings_->GetListenPort();
            port_label_->setText(QString::number(port));
        }));
    }

    InformationContent::~InformationContent() {
        context_->RemoveMessageTask(setting_changed_task_id_);
    }

    void InformationContent::OnContentShow() {
        AppContent::OnContentShow();
    }

    void InformationContent::OnContentHide() {
        AppContent::OnContentHide();
    }

}