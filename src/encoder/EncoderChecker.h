//
// Created by RGAA on 2023/8/11.
//

#ifndef SAILFISH_SERVER_ENCODERCHECKER_H
#define SAILFISH_SERVER_ENCODERCHECKER_H

#include <string>
#include <memory>
#include <vector>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/error.h>
}


namespace rgaa {

    enum class EncoderKind {
        kFFmpeg,
        kNvidiaSdk,
    };

    class SupportedEncoder {
    public:
        bool IsValid() const {
            return !name_.empty();
        }

    public:
        EncoderKind kind_{};
        std::string name_{};
        std::string readable_name_{};
    };

    class EncoderChecker {
    public:

        static EncoderChecker* Instance() {
            static EncoderChecker ec;
            return &ec;
        }

        void CheckSupportedEncoders();
        std::vector<std::shared_ptr<SupportedEncoder>> GetSupportedEncoders();
        void DumpSupportedEncoders();

    private:

        std::vector<std::shared_ptr<SupportedEncoder>> supported_encoders_;

    };

}

#endif //SAILFISH_SERVER_ENCODERCHECKER_H
