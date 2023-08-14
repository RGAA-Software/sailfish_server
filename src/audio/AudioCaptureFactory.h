//
// Created by RGAA on 2023-08-14.
//

#ifndef SAILFISH_SERVER_AUDIOCAPTUREFACTORY_H
#define SAILFISH_SERVER_AUDIOCAPTUREFACTORY_H

#include <memory>

namespace rgaa {

    class IAudioCapture;

    class AudioCaptureFactory {
    public:

        static std::shared_ptr<IAudioCapture> MakeAudioCapture();

    };

}

#endif //SAILFISH_SERVER_AUDIOCAPTUREFACTORY_H
