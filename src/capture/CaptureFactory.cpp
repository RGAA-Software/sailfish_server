//
// Created by RGAA on 2023/8/10.
//

#include "CaptureFactory.h"

#include "Capture.h"
#include "DDACapture.h"
#include "GraphicsCapture.h"
#include "encoder/EncoderFactory.h"
#include "encoder/EncoderChecker.h"
#include "rgaa_common/RLog.h"
#include "settings/Settings.h"

namespace rgaa {

    std::shared_ptr<Capture> CaptureFactory::MakeCapture(const std::shared_ptr<Context>& ctx) {
        std::shared_ptr<Capture> capture = nullptr;
        auto settings = Settings::Instance();
        auto supported_encoder = EncoderFactory::SelectEncoder();
        if (!supported_encoder) {
            LOGE("Can't find a encoder!");
            return nullptr;
        }
        auto capture_result_type = supported_encoder->kind_ == EncoderKind::kNvidiaSdk
                ? CaptureResultType::kTexture2D : CaptureResultType::kRawI420;
        LOGI("Capture result type : {}, 0 = Texture2D, 1 = RawI420", (int)capture_result_type);
#ifdef _OS_WINDOWS_
        auto capture_api = settings->GetCaptureAPI();
        if (capture_api == CaptureAPI::kDesktopDuplication) {
            capture = std::make_shared<DDACapture>(ctx, capture_result_type);
        }
        else if (capture_api == CaptureAPI::kWinGraphicsCapture) {
            capture = std::make_shared<GraphicsCapture>(ctx, capture_result_type);
        }
        
#endif

#ifdef OS_LINUX

#endif

        return capture;
    }

}