//
// Created by RGAA on 2023/8/15.
//

#include "MainPanel.h"

#include <QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QElapsedTimer>
#include <QFile>

#include "Application.h"
#include "context/Context.h"
#include "AppMenu.h"
#include "rgaa_common/RLog.h"
#include "AppContent.h"
#include "InformationContent.h"
#include "SettingsContent.h"

namespace rgaa {

    MainPanel::MainPanel(const std::shared_ptr<Context>& ctx, QWidget* parent) : QMainWindow(parent) {
        context_ = ctx;
        setWindowTitle(tr("Sailfish Server"));
        CreateLayout();
        LoadStyle("");
    }

    MainPanel::~MainPanel() {}

    void MainPanel::Show() {
        setFixedSize(960, 540);
        show();
    }

    void MainPanel::CreateLayout() {
        auto root_widget = new QWidget(this);
        auto root_layout = new QHBoxLayout();
        root_layout->setSpacing(0);
        root_layout->setContentsMargins(0,0,0,0);

        // 1. app menu
        std::vector<QString> menus = {
                tr("INFORMATION"),
                tr("SETTINGS"),
        };
        app_menu_ = new AppMenu(menus, this);
        app_menu_->SetOnItemClickedCallback([this](const QString& name, int idx) {
            content_widget_->setCurrentIndex(idx);
        });
        root_layout->addWidget(app_menu_);
//        connect(app_menu, &QPushButton::clicked, this, [=, this]() {
//            application_ = std::make_shared<Application>(context_);
//            application_->Init();
//            application_->Start();
//        });

        // 2. app content
        content_widget_ = new QStackedWidget(this);
        // 2.1 information
        auto information_content = new InformationContent(context_, this);
        content_widget_->addWidget(information_content);
        contents_.push_back(information_content);

        // 2.2 settings
        auto settings_content = new SettingsContent(context_, this);
        content_widget_->addWidget(settings_content);
        contents_.push_back(settings_content);

        root_layout->addWidget(content_widget_);

        root_widget->setLayout(root_layout);
        setCentralWidget(root_widget);

        //
        content_widget_->setCurrentIndex(0);
    }

    void MainPanel::LoadStyle(const std::string &name) {
        QElapsedTimer time;
        time.start();

        auto qssFile = ":/qss/lightblue.css";

        QString qss;
        QFile file(qssFile);
        if (file.open(QFile::ReadOnly)) {
            qDebug() << "open success...";
            QStringList list;
            QTextStream in(&file);
            //in.setCodec("utf-8");
            while (!in.atEnd()) {
                QString line;
                in >> line;
                list << line;
            }

            file.close();
            qss = list.join("\n");
            QString paletteColor = qss.mid(20, 7);
            qApp->setPalette(QPalette(paletteColor));
            qApp->setStyleSheet(qss);
        }

        qDebug() << "用时:" << time.elapsed();
    }

}