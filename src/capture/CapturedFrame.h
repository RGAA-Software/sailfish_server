//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CAPTUREDFRAME_H
#define SAILFISH_SERVER_CAPTUREDFRAME_H

#include <memory>

namespace rgaa {

    class Data;

    enum class CaptureType {
        kD3DTexture,
        kRawData,
    };

    class CapturedFrame {
    public:
        CaptureType capture_type_;
        uint64_t handle_ = 0;
        std::shared_ptr<Data> raw_data_ = nullptr;
        int64_t frame_index_ = 0;
        int frame_width_ = 0;
        int frame_height_ = 0;
        int dup_index_ = -1;
        uint64_t captured_time_ = 0;
    };

    using CapturedFramePtr = std::shared_ptr<CapturedFrame>;
}

#endif //SAILFISH_SERVER_CAPTUREDFRAME_H
