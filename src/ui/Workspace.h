//
// Created by RGAA on 2023/8/15.
//

#ifndef SAILFISH_SERVER_WORKSPACE_H
#define SAILFISH_SERVER_WORKSPACE_H

#include <QWidget>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>

#include <memory>

namespace rgaa {

    class Context;
    class Application;

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

    };

}

#endif //SAILFISH_SERVER_WORKSPACE_H