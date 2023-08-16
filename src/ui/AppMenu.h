//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_APPMENU_H
#define SAILFISH_SERVER_APPMENU_H

#include <QWidget>
#include <QString>
#include <QLabel>

#include <memory>
#include <functional>

namespace rgaa {

    using OnItemClickedCallback = std::function<void(const QString& name, int idx)>;

    class AppMenuItem : public QWidget {
    public:

        AppMenuItem(const QString& name, int idx, QWidget* parent = nullptr);
        ~AppMenuItem();

        void SetOnItemClickedCallback(const OnItemClickedCallback& cbk);
        void Select();
        void UnSelect();
        QString GetName();
        bool IsSelected();

    protected:
        void paintEvent(QPaintEvent *event) override;
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;

    private:

        bool entered_ = false;
        bool pressed_ = false;
        bool selected_ = false;

        float round_radius_ = 6.0f;

        QString name_;
        int idx_;
        OnItemClickedCallback callback_;

        QLabel* icon_ = nullptr;
        QLabel* text_ = nullptr;
    };


    class AppMenu : public QWidget {
    public:

        AppMenu(const std::vector<QString>& items, QWidget* parent = nullptr);
        ~AppMenu();

        void SetOnItemClickedCallback(OnItemClickedCallback cbk);

    private:

        void CreateLayout();

    private:

        OnItemClickedCallback callback_;
        std::vector<AppMenuItem*> app_items;

    };

}

#endif //SAILFISH_SERVER_APPMENU_H
