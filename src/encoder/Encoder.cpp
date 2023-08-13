//
// Created by RGAA on 2023/8/10.
//

#include "Encoder.h"

#include "settings/Settings.h"

namespace rgaa {

    Encoder::Encoder(const std::shared_ptr<Context>& ctx, int dup_idx, const std::string& encoder_name, int width, int height) {
        context_ = ctx;
        width_ = width;
        height_ = height;
        settings_ = Settings::Instance();
        encoder_name_ = encoder_name;
        dup_idx_ = dup_idx;
    }

    Encoder::~Encoder() {

    }

    bool Encoder::Init() {
        return false;
    }

    void Encoder::Exit() {

    }

    std::shared_ptr<EncodedVideoFrame> Encoder::Encode(const std::shared_ptr<CapturedFrame>& cp_frame) {
        return nullptr;
    }

    void Encoder::InsertIDR() {
        insert_idr_ = true;
    }

}