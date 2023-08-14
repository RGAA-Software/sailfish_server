//
// Created by RGAA on 2023-08-14.
//

#include "AudioCaptureFactory.h"

#include "WASAPIAudioCapture.h"

namespace rgaa {

    std::shared_ptr<IAudioCapture> AudioCaptureFactory::MakeAudioCapture() {
        std::shared_ptr<IAudioCapture> capture_ = nullptr;
#ifdef _OS_WINDOWS_
        capture_ = std::make_shared<WASAPIAudioCapture>();
        return capture_;
#endif

        return nullptr;
    }

}
