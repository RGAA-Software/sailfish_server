//
// Created by RGAA on 2023/8/10.
//

#include "Capture.h"
#include "src/context/Settings.h"

#include <utility>

namespace rgaa {

    Capture::Capture(const std::shared_ptr<Context>& ctx, const CaptureResultType& crt) {
        context_ = ctx;
        capture_result_type_ = crt;
        settings_ = Settings::Instance();
    }

    Capture::~Capture() {

    }

    bool Capture::Init() {
        return false;
    }

    bool Capture::CaptureNextFrame() {
        return false;
    }

    void Capture::Exit() {
        exit_ = true;
    }

    int Capture::GetCaptureCount() {
        return 0;
    }

    void Capture::SetOnFrameCapturedCallback(OnFrameCapturedCallback cbk) {
        captured_cbk_ = std::move(cbk);
    }

}
