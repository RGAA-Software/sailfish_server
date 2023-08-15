//
// Created by RGAA on 2023/8/15.
//

#include "Workspace.h"

#include <QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QElapsedTimer>
#include <QFile>

namespace rgaa {

    Workspace::Workspace(const std::shared_ptr<Context>& ctx, QWidget* parent) : QMainWindow(parent) {
        setWindowTitle(tr("Sailfish Server"));
        CreateLayout();
        LoadStyle("");
    }

    Workspace::~Workspace() {}

    void Workspace::Show() {
        resize(960, 540);
        show();
    }

    void Workspace::CreateLayout() {
        auto root_widget = new QWidget(this);
        auto root_layout = new QVBoxLayout(this);
        root_layout->setSpacing(0);
        root_layout->setContentsMargins(0,0,0,0);

        // 1. app menu
        auto app_menu = new QPushButton(this);
        app_menu->setText("dfsakfsdakfjk");
        root_layout->addWidget(app_menu);

        // 2. stream list
        auto stream_list = new QPushButton(this);
        stream_list->setText("dfsakfsdakfjk");
        root_layout->addWidget(stream_list);

        root_widget->setLayout(root_layout);
        setCentralWidget(root_widget);
    }

    void Workspace::LoadStyle(const std::string &name) {
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