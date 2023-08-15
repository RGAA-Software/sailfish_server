//
// Created by RGAA on 2023-08-15.
//

#ifndef SAILFISH_SERVER_MESSAGEMAKER_H
#define SAILFISH_SERVER_MESSAGEMAKER_H

#include <memory>
#include <string>

#include "settings/Settings.h"

namespace rgaa {

    class Data;
    class NetMessage;

    class MessageMaker {
    public:
        static std::string MakeVideoConfigSync(EncodeType type, int width, int height);
        static std::string MakeAudioConfigSync(int samples, int channels);
        static std::string MakeAudioFrameSync(const std::shared_ptr<Data>& data, int frame_size, int samples, int channels);

    };

}

#endif //SAILFISH_SERVER_MESSAGEMAKER_H
