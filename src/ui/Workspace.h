//
// Created by RGAA on 2023/8/15.
//

#ifndef SAILFISH_SERVER_WORKSPACE_H
#define SAILFISH_SERVER_WORKSPACE_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

#include <memory>
#include <vector>

namespace rgaa {

    class Context;
    class Application;
    class AppMenu;
    class AppContent;

    class Workspace : public QMainWindow {
    public:

        Workspace(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~Workspace();

        void Show();

    private:

        void CreateLayout();

        void LoadStyle(const std::string& name);

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<Application> application_ = nullptr;

        AppMenu* app_menu_ = nullptr;

        QStackedWidget* content_widget_ = nullptr;
        std::vector<AppContent*> contents_;
    };

}

#endif //SAILFISH_SERVER_WORKSPACE_H
