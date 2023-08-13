//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CAPTUREFACTORY_H
#define SAILFISH_SERVER_CAPTUREFACTORY_H

#include <memory>

#include "Capture.h"

namespace rgaa {

    class Capture;
    class Context;

    class CaptureFactory {
    public:

        static std::shared_ptr<Capture> MakeCapture(const std::shared_ptr<Context>& ctx);

    };

}

#endif //SAILFISH_SERVER_CAPTUREFACTORY_H
