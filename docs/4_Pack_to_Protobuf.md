#### Pack to protobuf and send packet to client

##### It's easy to do this, see src/encoder/EncodedFrame.cpp
```c++
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
```

##### Then sending via Websocket, see src/network/WSServer.cpp
```c++
void WSServer::PostBinaryMessage(const std::string& data) {
    if (!ws_server_) {
        return;
    }

    std::lock_guard<std::mutex> guard(session_mtx_);
    if (sessions_.empty()) {
        return;
    }
    for (auto& [handle, session] : sessions_) {
        try {
            ws_server_->send(handle, data, binary);
        }
        catch (std::exception &e) {
            LOGE("Send binary error : {}", e.what());
        }
    }
}
```