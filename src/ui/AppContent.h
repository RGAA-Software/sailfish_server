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

namespace rgaa {

    class AppContent : public QWidget {
    public:

        explicit AppContent(QWidget* parent = nullptr);
        ~AppContent();

        virtual void OnContentShow();
        virtual void OnContentHide();

    };

}

#endif //SAILFISH_SERVER_APPCONTENT_H
