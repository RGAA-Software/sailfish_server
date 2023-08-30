//
// Created by RGAA on 2023-08-30.
//

#ifndef SAILFISH_SERVER_ABOUTCONTENT_H
#define SAILFISH_SERVER_ABOUTCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class AboutContent : public AppContent {
    public:

        explicit AboutContent(const std::shared_ptr<Context>& ctx, QWidget* parent);
        ~AboutContent() override;

        void OnContentShow() override;
        void OnContentHide() override;

    };

}

#endif //SAILFISH_SERVER_ABOUTCONTENT_H
