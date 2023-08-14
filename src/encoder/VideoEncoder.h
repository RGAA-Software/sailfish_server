//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_VIDEOENCODER_H
#define SAILFISH_SERVER_VIDEOENCODER_H

#include <memory>
#include <string>

namespace rgaa {

    class Context;
    class CapturedFrame;
    class Settings;
    class Data;
    class EncodedVideoFrame;

    class VideoEncoder {
    public:

        VideoEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, const std::string& encoder_name, int width, int height);
        ~VideoEncoder();

        virtual bool Init();
        virtual void Exit();
        virtual void InsertIDR();
        virtual std::shared_ptr<EncodedVideoFrame> Encode(const std::shared_ptr<CapturedFrame>& cp_frame);

    protected:

        int width_ = 0;
        int height_ = 0;

        std::shared_ptr<Context> context_ = nullptr;

        Settings* settings_ = nullptr;

        std::string encoder_name_{};
        int dup_idx_ = -1;

        bool insert_idr_ = false;

    };

}

#endif //SAILFISH_SERVER_VIDEOENCODER_H
