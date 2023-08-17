//
// Created by RGAA on 2023/8/10.
//

#ifndef SAILFISH_SERVER_ENCODERFACTORY_H
#define SAILFISH_SERVER_ENCODERFACTORY_H

#include <memory>
#include <string>

namespace rgaa {

    class VideoEncoder;
    class Context;
    class SupportedEncoder;

    class EncoderFactory {
    public:

        static std::shared_ptr<VideoEncoder> MakeEncoder(const std::shared_ptr<Context>& ctx, int dup_idx, int width, int height);
        static std::shared_ptr<SupportedEncoder> SelectEncoder(const std::shared_ptr<Context>& ctx);
    };

}

#endif //SAILFISH_SERVER_ENCODERFACTORY_H
