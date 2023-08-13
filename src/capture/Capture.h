//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_CAPTURE_H
#define SAILFISH_SERVER_CAPTURE_H

#include <memory>
#include <functional>

namespace rgaa {

    class Context;
    class CapturedFrame;
    class Settings;

    enum class CaptureResultType {
        kTexture2D,
        kRawI420,
    };

    using OnFrameCapturedCallback = std::function<void(const std::shared_ptr<CapturedFrame>&)>;

    class Capture {
    public:

        Capture(const std::shared_ptr<Context>& ctx, const CaptureResultType& crt);
        virtual ~Capture();

        virtual bool Init();
        virtual bool CaptureNextFrame();
        virtual void Exit();

        void SetOnFrameCapturedCallback(OnFrameCapturedCallback cbk);

    protected:

        Settings* settings_ = nullptr;

        std::shared_ptr<Context> context_ = nullptr;
        OnFrameCapturedCallback captured_cbk_;

        bool exit_ = false;

        int capture_monitor_idx_ = 0;

        CaptureResultType capture_result_type_;

    };

}

#endif //SAILFISH_SERVER_CAPTURE_H
