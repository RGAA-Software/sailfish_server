//
// Created by RGAA on 2023/8/16.
//

#ifndef SAILFISH_SERVER_INFORMATIONCONTENT_H
#define SAILFISH_SERVER_INFORMATIONCONTENT_H

#include "AppContent.h"

namespace rgaa {

    class InformationContent : public AppContent {
    public:

        InformationContent(QWidget* parent = nullptr);
        ~InformationContent();

        void OnContentShow() override;
        void OnContentHide() override;
    };

}

#endif //SAILFISH_SERVER_INFORMATIONCONTENT_H
