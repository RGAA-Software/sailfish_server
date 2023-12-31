//
// Created by RGAA on 2023/8/11.
//

#include "EncodedFrame.h"
#include "src/context/Settings.h"
#include "rgaa_common/RData.h"
#include "rgaa_common/RTime.h"

#include "messages.pb.h"

namespace rgaa {

    std::shared_ptr<NetMessage> EncodedVideoFrame::AsProtoMessage() {
        auto net_msg = std::make_shared<NetMessage>();
        net_msg->set_type(MessageType::kVideoFrame);
        net_msg->set_send_time(GetCurrentTimestamp());

        auto video_frame = new VideoFrameSync();

        // 1. video type
        auto settings = Settings::Instance();
        if (settings->GetEncodeType() == EncoderType::kH264) {
            video_frame->set_type(VideoType::kH264);
        }
        else if (settings->GetEncodeType() == EncoderType::kH265) {
            video_frame->set_type(VideoType::kH265);
        }

        // 2. data
        video_frame->set_data(data_->AsString());

        // 3. frame index
        video_frame->set_frame_index(frame_index_);

        // 4. key frame ?
        video_frame->set_key(key_frame_);

        // 5. width & height
        video_frame->set_width(width_);
        video_frame->set_height(height_);

        // 6. dup index
        video_frame->set_dup_idx(dup_idx_);

        //
        auto duration_from_capture = GetCurrentTimestamp() - this->captured_time_;
        video_frame->set_encode_time(duration_from_capture);

        net_msg->set_allocated_video_frame(video_frame);
        return net_msg;
    }

    std::string EncodedVideoFrame::AsProtoMessageStr() {
        auto proto_msg = AsProtoMessage();
        return std::move(proto_msg->SerializeAsString());
    }

}
