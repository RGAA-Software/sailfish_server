//
// Created by RGAA on 2023/8/16.
//

#include "InformationContent.h"

#include "util/IPUtil.h"
#include "settings/Settings.h"
#include "rgaa_common/RLog.h"
#include "context/Context.h"

namespace rgaa {

    InformationContent::InformationContent(const std::shared_ptr<Context>& ctx, QWidget* parent) : AppContent(ctx, parent) {
        auto root_layout = new QHBoxLayout();

        // 1. local settings
        auto local_settings_layout = new QVBoxLayout();
        root_layout->addLayout(local_settings_layout);
        local_settings_layout->setSpacing(0);
        local_settings_layout->setContentsMargins(0, 0, 0, 0);

        //
        {
            auto title = new QLabel(this);
            title->setText(tr("Server Mode"));
            title->setStyleSheet(R"(font-size:20px; color: #333333;)");
            local_settings_layout->addSpacing(20);
            local_settings_layout->addWidget(title);
        }
        {
            // enable this mode
            auto cb = new QCheckBox(this);
            cb_server_mode_ = cb;
            cb->setText(tr("Enable Server Mode"));
            local_settings_layout->addSpacing(12);
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
            local_settings_layout->addSpacing(20);
            auto ips = IPUtil::ScanIPs();
            auto ip_title = new QLabel(this);
            ip_title->setStyleSheet(R"(font-size:14px; color: #333333;)");
            ip_title->setText(tr("IP Accessible"));
            local_settings_layout->addWidget(ip_title);

            for (const auto& ip : ips) {
                auto ip_value = new QLabel(this);
                ip_value->setText(ip.first.c_str());
                ip_value->setStyleSheet(R"(font-size:17px; color: #333333;)");
                local_settings_layout->addSpacing(10);
                local_settings_layout->addWidget(ip_value);
            }
        }

        {
            local_settings_layout->addSpacing(20);
            auto port_title = new QLabel(this);
            port_title->setStyleSheet(R"(font-size:14px; color: #333333;)");
            port_title->setText(tr("Port Listening"));
            local_settings_layout->addWidget(port_title);

            auto port_value = new QLabel(this);
            auto port = std::to_string(Settings::Instance()->GetListenPort());
            port_value->setText(port.c_str());
            port_value->setStyleSheet(R"(font-size:17px; color: #333333;)");
            local_settings_layout->addSpacing(10);
            local_settings_layout->addWidget(port_value);
        }

        local_settings_layout->addStretch();

        // 2. relay settings

        auto relay_settings_layout = new QVBoxLayout();
        relay_settings_layout->addSpacing(20);
        relay_settings_layout->setSpacing(0);
        relay_settings_layout->setContentsMargins(0, 0, 0, 0);
        root_layout->addLayout(relay_settings_layout);
        {
            auto title = new QLabel(this);
            title->setText(tr("Relay Mode"));
            title->setStyleSheet(R"(font-size:20px; color: #333333;)");
            relay_settings_layout->addWidget(title);
        }
        {
            // enable this mode
            auto cb = new QCheckBox(this);
            cb_relay_mode_ = cb;
            cb->setText(tr("Enable Relay Mode"));
            relay_settings_layout->addSpacing(12);
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