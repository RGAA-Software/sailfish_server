//
// Created by RGAA on 2023/8/10.
//

#include "VideoEncoder.h"

#include "settings/Settings.h"
#include "context/Context.h"
#include "rgaa_common/RMessageQueue.h"
#include "rgaa_common/RLog.h"
#include "AppMessages.h"

namespace rgaa {

    VideoEncoder::VideoEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, const std::string& encoder_name, int width, int height) {
        context_ = ctx;
        width_ = width;
        height_ = height;
        settings_ = Settings::Instance();
        encoder_name_ = encoder_name;
        dup_idx_ = dup_idx;

        peer_conn_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kPeerConnected, [=, this](auto& msg) {
            InsertIDR();
        }));

        idr_task_id_ = context_->RegisterMessageTask(MessageTask::Make(kMessageIDR, [=, this](auto& msg) {
            InsertIDR();
        }));
    }

    VideoEncoder::~VideoEncoder() {

    }

    bool VideoEncoder::Init() {
        return false;
    }

    void VideoEncoder::Exit() {
        context_->RemoveMessageTask(peer_conn_task_id_);
        context_->RemoveMessageTask(idr_task_id_);
    }

    std::shared_ptr<EncodedVideoFrame> VideoEncoder::Encode(const std::shared_ptr<CapturedFrame>& cp_frame) {
        return nullptr;
    }

    void VideoEncoder::InsertIDR() {
        insert_idr_ = true;
    }

}