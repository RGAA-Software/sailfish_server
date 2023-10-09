#### There are 3 steps to use FFmpeg for encoding video
- Init 
- Encode
- Release

**See src/encoder/FFmpegEncoder.cpp**   
**Encoding a video frame is pretty simple. Just take a look at the code**

##### 1.1 Init, see comments in code
```c++
bool FFmpegEncoder::Init() {

    // 1. Find encoder by name
    const AVCodec* codec = avcodec_find_encoder_by_name(encoder_name_.c_str());

    // 2. Config params
    codec_context_ = avcodec_alloc_context3(codec);
    codec_context_->width = width_;
    codec_context_->height = height_;
    codec_context_->time_base = {1, settings_->encode_fps_ };
    codec_context_->framerate = {settings_->encode_fps_, 1};
    codec_context_->pix_fmt = AV_PIX_FMT_YUV420P;
    codec_context_->thread_count = (int)std::thread::hardware_concurrency()/2;
    codec_context_->gop_size = 180;
    codec_context_->bit_rate = settings_->encode_bps_ * 1000;
    codec_context_->max_b_frames = 0;
    codec_context_->flags |= AV_CODEC_FLAG_LOW_DELAY;
    
    // 3. Different encoder with different config
    std::string qp = "23";
    // Nvidia hardware
    if (encoder_name_.find("nvenc") != std::string::npos) {
        av_opt_set(codec_context_->priv_data, "preset", "llhp", 0);
        av_opt_set(codec_context_->priv_data, "profile", "main", 0);
        av_opt_set(codec_context_->priv_data, "delay", "0", 0);
        av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);

        if (encoder_name_ == "h264_nvenc") {
            av_opt_set(codec_context_->priv_data, "rc", "cbr", 0);
            av_opt_set(codec_context_->priv_data, "cq", qp.c_str(), 0);
        }
        else if (encoder_name_ == "hevc_nvenc") {
            av_opt_set(codec_context_->priv_data, "qp", qp.c_str(), 0);
        }
        av_opt_set(codec_context_->priv_data, "tune", "ull", 0);
        av_opt_set(codec_context_->priv_data, "zerolatency", "1", 0);

    }
    // Software 
    else if (encoder_name_.find("libx") != std::string::npos) {
        av_opt_set(codec_context_->priv_data, "preset", "ultrafast", 0);
        av_opt_set(codec_context_->priv_data, "crf", "23", 0);
        av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);
        av_opt_set(codec_context_->priv_data, "tune", "zerolatency", 0);
        av_opt_set(codec_context_->priv_data, "rc", "cbr", 0);

        if (encoder_name_ == "libx264") {
            av_opt_set(codec_context_->priv_data, "profile", "baseline", 0);
        }
        else if (encoder_name_ == "libx265") {
            av_opt_set(codec_context_->priv_data, "profile", "main", 0);
        }
    }

    // 4. Open encoder 
    auto ret = avcodec_open2(codec_context_, codec, NULL);
    
    frame_ = av_frame_alloc();
    frame_->width = codec_context_->width;
    frame_->height = codec_context_->height;
    frame_->format = codec_context_->pix_fmt;

    av_opt_set(codec_context_->priv_data, "forced-idr", "1", 0);
    
    av_frame_get_buffer(frame_, 0);
    packet_ = av_packet_alloc();

    return true;
}

```

##### 1.2 Encode
- If you don't know the meaning of buffer's size, please see [2_Convert_to_I420](2_Convert_to_I420.md)
```c++
std::shared_ptr<EncodedVideoFrame> FFmpegEncoder::Encode(const std::shared_ptr<CapturedFrame> &cp_frame) {
    
    int y_size = width_ * height_;
    // U size = V size
    int uv_size = y_size / 4;
    
    // Y buffer
    memcpy(frame_->data[0], i420_data->CStr(), y_size);
    // U buffer
    memcpy(frame_->data[1], i420_data->CStr() + y_size, uv_size);
    // V buffer
    memcpy(frame_->data[2], i420_data->CStr() + y_size + uv_size, uv_size);
    
    // !!! If you need a IDR frame, insert one
    if (insert_idr_) {
        frame_->pict_type = AV_PICTURE_TYPE_I;
        insert_idr_ = false;
    }
    
    // ! Send to encode
    int send_result = avcodec_send_frame(codec_context_, frame_);

    std::shared_ptr<EncodedVideoFrame> encoded_video_frame = nullptr;

    while (send_result >= 0)
    {
        // ! After encoding
        int recv_result = avcodec_receive_packet(codec_context_, packet_);
        
        bool key_frame = packet_->flags & AV_PKT_FLAG_KEY;

        auto encoded_data = Data::Make((char*)packet_->data, packet_->size);
        encoded_video_frame = std::make_shared<EncodedVideoFrame>();
        encoded_video_frame->frame_index_ = cp_frame->frame_index_;
        encoded_video_frame->data_ = encoded_data;
        encoded_video_frame->width_ = cp_frame->frame_width_;
        encoded_video_frame->height_ = cp_frame->frame_height_;
        encoded_video_frame->dup_idx_ = cp_frame->dup_index_;
        encoded_video_frame->key_frame_ = key_frame;
        encoded_video_frame->encoded_time_ = GetCurrentTimestamp();
        encoded_video_frame->captured_time_ = cp_frame->captured_time_;
        
        av_packet_unref(packet_);
    }
    return encoded_video_frame;
}
```

##### 1.3 Release if you exit ...
