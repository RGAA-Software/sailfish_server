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
    class Encoder;
    class Settings;
    class WSServer;
    class IAudioCapture;
    class Thread;

    class Application {
    public:
        Application();
        ~Application();

        void Init();
        void Start();

    private:

        void StartVideoCapturing();
        void StartAudioCapturing();
        std::shared_ptr<Encoder> GetEncoderForIndex(int dup_idx);
        std::shared_ptr<Encoder> MakeEncoder(int dup_idx, int w, int h);

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<Capture> capture_ = nullptr;
        std::map<int, std::shared_ptr<Encoder>> encoders_;

        std::shared_ptr<WSServer> ws_server_ = nullptr;

        std::shared_ptr<Thread> audio_thread_ = nullptr;
        std::shared_ptr<IAudioCapture> audio_capture_ = nullptr;

        Settings* settings_ = nullptr;

    };

}

#endif //SAILFISH_SERVER_APPLICATION_H
