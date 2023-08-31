//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_INFORMATIONCONTENT_H
#define SAILFISH_SERVER_INFORMATIONCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class InformationContent : public AppContent {
    public:

        explicit InformationContent(const std::shared_ptr<Context>& ctx, QWidget* parent = nullptr);
        ~InformationContent() override;

        void OnContentShow() override;
        void OnContentHide() override;

    private:

        QCheckBox* cb_server_mode_ = nullptr;
        QCheckBox* cb_relay_mode_ = nullptr;

        QLabel* port_label_ = nullptr;
        int setting_changed_task_id_ = -1;

    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
