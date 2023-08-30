//
// Created by RGAA on 2023-08-15.
//

#include "MessageMaker.h"

#include "messages.pb.h"
#include "rgaa_common/RTime.h"
#include "rgaa_common/RData.h"

namespace rgaa {

    std::shared_ptr<NetMessage> MessageMaker::MakeVideoConfigSync(EncodeType type, int width, int height) {
        auto message = std::make_shared<NetMessage>();
        message->set_send_time(GetCurrentTimestamp());
        message->set_type(MessageType::kVideoConfigSync);
        auto sync = new VideoConfigSync();
        if (type == EncodeType::kH264) {
            sync->set_type(VideoType::kH264);
        }
        else if (type == EncodeType::kH265) {
            sync->set_type(VideoType::kH265);
        }

        sync->set_width(width);
        sync->set_height(height);
        message->set_allocated_video_config_sync(sync);
        return message;
    }

    std::shared_ptr<NetMessage> MessageMaker::MakeAudioConfigSync(int samples, int channels) {
        auto message = std::make_shared<NetMessage>();
        message->set_type(MessageType::kAudioConfigSync);
        message->set_send_time(GetCurrentTimestamp());
        auto sync = new AudioConfigSync();
        sync->set_samples(samples);
        sync->set_channels(channels);
        message->set_allocated_audio_config_sync(sync);
        return message;
    }

    std::shared_ptr<NetMessage> MessageMaker::MakeAudioFrameSync(const std::shared_ptr<Data>& data, int frame_size, int samples, int channels, int bits) {
        auto net_msg = std::make_shared<NetMessage>();
        net_msg->set_type(MessageType::kAudioFrame);
        net_msg->set_send_time(GetCurrentTimestamp());

        auto audio_frame = new AudioFrameSync();

        audio_frame->set_data(data->AsString());
        audio_frame->set_frame_size(frame_size);
        audio_frame->set_samples(samples);
        audio_frame->set_channels(channels);
        audio_frame->set_bits(bits);

        net_msg->set_allocated_audio_frame(audio_frame);

        return net_msg;
    }

    std::shared_ptr<NetMessage> MessageMaker::MakeHeartBeat(uint64_t idx) {
        auto message = std::make_shared<NetMessage>();
        message->set_send_time(GetCurrentTimestamp());
        message->set_type(MessageType::kHeartBeat);
        auto beat = new HeartBeat();
        beat->set_index(idx);
        message->set_allocated_heart_beat(beat);
        return message;
    }

    std::shared_ptr<NetMessage> MessageMaker::MakeClipboard(const std::string& msg) {
        auto message = std::make_shared<NetMessage>();
        message->set_type(MessageType::kClipboard);
        message->set_send_time(GetCurrentTimestamp());
        auto clipboard = new Clipboard();
        clipboard->set_msg(msg);
        message->set_allocated_clipboard(clipboard);
        return message;
    }

}