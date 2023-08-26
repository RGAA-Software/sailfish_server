//
// Created by RGAA on 2023/8/25.
//

#ifndef SAILFISH_SERVER_STATISTICS_H
#define SAILFISH_SERVER_STATISTICS_H

#include <memory>
#include <map>

namespace rgaa {

    class Context;
    class SailfishSDK;

    class Statistics {
    public:

        Statistics(const std::shared_ptr<Context>& ctx);

        void AppendVideoFrameNetworkTime(uint64_t frame_idx, uint64_t diff);
        uint64_t GetFrameNetworkTime(uint64_t frame_idx);

        void Reset();

    private:

        std::shared_ptr<Context> context_ = nullptr;
        std::shared_ptr<SailfishSDK> sdk_ = nullptr;

        std::map<uint64_t, uint64_t> video_frame_network_times_;

    };

}

#endif //SAILFISH_SERVER_STATISTICS_H
