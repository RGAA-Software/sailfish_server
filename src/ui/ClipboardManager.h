//
// Created by RGAA on 2023-08-19.
//

#ifndef SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H
#define SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H

#include <memory>

#include <QObject>
#include <QClipboard>
#include <QApplication>
#include <QMimeData>
#include <QString>

#include <atomic>

namespace rgaa {

    class Context;

    class ClipboardManager : public QObject {
        //Q_OBJECT
    public:

        explicit ClipboardManager(const std::shared_ptr<Context> & ctx);
        ~ClipboardManager() override;

        void Init();
        void SetText(const QString& msg);

    private slots:
        void OnClipboardDataChanged();

    private:
        void StopMonitoringClipboard();

    private:

        std::shared_ptr<Context> context_ = nullptr;

        QClipboard* clipboard_ = nullptr;

        int clipboard_task_id_ = -1;

        QString manual_set_msg_;

    };

}

#endif //SAILFISH_CLIENT_PC_CLIPBOARDMANAGER_H
