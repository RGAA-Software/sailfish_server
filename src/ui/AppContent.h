//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_APPCONTENT_H
#define SAILFISH_SERVER_APPCONTENT_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

namespace rgaa {

    class Context;
    class Settings;

    class AppContent : public QWidget {
    public:

        explicit AppContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~AppContent();

        virtual void OnContentShow();
        virtual void OnContentHide();

    protected:

        std::shared_ptr<Context> context_ = nullptr;

        Settings* settings_ = nullptr;

    };

}

#endif //SAILFISH_SERVER_APPCONTENT_H
