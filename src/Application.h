//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_APPLICATION_H
#define SAILFISH_SERVER_APPLICATION_H

#include <memory>
#include <map>

namespace rgaa {

    class Context;
    class Capture;
    class VideoEncoder;
    class Settings;
    class WSServer;
    class IAudioCapture;
    class Thread;
    class AudioEncoder;
    class Connection;

    class Application {
    public:
        explicit Application(const std::shared_ptr<Context>& ctx, bool audio);
        ~Application();

        void Start();
        void Exit();

    private:

        void StartVideoCapturing();
        void StartAudioCapturing();
        std::shared_ptr<VideoEncoder> GetEncoderForIndex(int dup_idx);
        std::shared_ptr<VideoEncoder> MakeEncoder(int dup_idx, int w, int h);
        void SendBackConfig();

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<Capture> capture_ = nullptr;
        std::map<int, std::shared_ptr<VideoEncoder>> encoders_;
        std::shared_ptr<Thread> video_thread_ = nullptr;

        std::shared_ptr<Thread> audio_thread_ = nullptr;
        std::shared_ptr<IAudioCapture> audio_capture_ = nullptr;
        std::shared_ptr<AudioEncoder> audio_encoder_ = nullptr;

        std::shared_ptr<Connection> connection_ = nullptr;

        Settings* settings_ = nullptr;

        bool audio_enabled_ = false;

        int timer_1s_task_id_ = -1;
        int peer_connected_msg_id_ = -1;

    };

}

#endif //SAILFISH_SERVER_APPLICATION_H
