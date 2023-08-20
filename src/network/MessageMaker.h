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
        static std::shared_ptr<NetMessage> MakeVideoConfigSync(EncodeType type, int width, int height);
        static std::shared_ptr<NetMessage> MakeAudioConfigSync(int samples, int channels);
        static std::shared_ptr<NetMessage> MakeAudioFrameSync(const std::shared_ptr<Data>& data, int frame_size, int samples, int channels);
        static std::shared_ptr<NetMessage> MakeHeartBeat(uint64_t idx);
        static std::shared_ptr<NetMessage> MakeClipboard(const std::string& msg);
    };

}

#endif //SAILFISH_SERVER_MESSAGEMAKER_H
