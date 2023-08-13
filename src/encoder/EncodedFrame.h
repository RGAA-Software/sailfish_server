//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_SERVER_ENCODEDFRAME_H
#define SAILFISH_SERVER_ENCODEDFRAME_H

#include <memory>
#include <string>

namespace rgaa {

    class Data;
    class NetMessage;

    class EncodedVideoFrame {
    public:

        std::shared_ptr<NetMessage> AsProtoMessage();
        std::string AsProtoMessageStr();

    public:
        int64_t frame_index_ = 0;
        std::shared_ptr<Data> data_ = nullptr;
        int width_ = 0;
        int height_ = 0;
        int dup_idx_ = -1;
        bool key_frame_ = false;
        uint64_t captured_time_ = 0;
        uint64_t encoded_time_ = 0;
    };


    class EncodedAudioFrame {
    public:
        int64_t frame_index_ = 0;
        std::shared_ptr<Data> data_ = nullptr;
    };
}

#endif //SAILFISH_SERVER_ENCODEDFRAME_H
