//
// Created by RGAA on 2023/8/19.
//

#include "WidgetHelper.h"

#include <vector>
#include <QRandomGenerator>

namespace rgaa {

    void WidgetHelper::ClearMargin(QLayout* layout) {
        layout->setSpacing(0);
        layout->setContentsMargins(0,0,0,0);
    }

}
