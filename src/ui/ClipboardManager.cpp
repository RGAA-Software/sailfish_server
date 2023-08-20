//
// Created by RGAA on 2023-08-19.
//

#include "ClipboardManager.h"

#include <QObject>
#include <QTimer>

#include "rgaa_common/RLog.h"
#include "context/Context.h"
#include "network/MessageMaker.h"
#include "rgaa_common/RMessageQueue.h"
#include "AppMessages.h"

namespace rgaa {

    ClipboardManager::ClipboardManager(const std::shared_ptr<Context> & ctx) {
        this->context_ = ctx;

        clipboard_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kCodeClipboard, [=, this](auto& msg) {
            auto target_msg = std::dynamic_pointer_cast<ClipboardMessage>(msg);
            if (target_msg) {
                SetText(target_msg->msg_.c_str());
            }
        }));

    }

    ClipboardManager::~ClipboardManager() {
        StopMonitoringClipboard();
        if (clipboard_task_id_ != -1) {
            context_->RemoveMessageTask(clipboard_task_id_);
        }
    }

    void ClipboardManager::Init() {
        clipboard_ = QApplication::clipboard();
        StartMonitoringClipboard();

//        std::thread* t = new std::thread([=](){
//            for (int i = 0; i < 10000; i++) {
//                auto msg = QString::number(i) + " //// test";
//                SetText(msg);
//                std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            }
//        });
//        t->detach();
    }

    void ClipboardManager::OnClipboardDataChanged() {
        QClipboard *clipboard = QApplication::clipboard();
        auto text = clipboard->text();
        if (text.isEmpty()) {
            return;
        }

        LOGI("// clip board : {} {}", text.toStdString(), (void*)this);
        auto msg = MessageMaker::MakeClipboard(text.toStdString());
        context_->PostNetworkBinaryMessage(msg);
    }

    void ClipboardManager::SetText(const QString &msg) {
        QTimer::singleShot(0, this, [=, this](){
            StopMonitoringClipboard();
            clipboard_->setText(msg);
            StartMonitoringClipboard();
        });
    }

    void ClipboardManager::StartMonitoringClipboard() {
        QTimer::singleShot(0, this, [=, this]() {
            connect(clipboard_, &QClipboard::dataChanged, this,
                             &ClipboardManager::OnClipboardDataChanged);
        });
    }

    void ClipboardManager::StopMonitoringClipboard() {
        disconnect(clipboard_, &QClipboard::dataChanged, this,
                            &ClipboardManager::OnClipboardDataChanged);
    }
}