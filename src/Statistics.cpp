//
// Created by RGAA on 2023/8/25.
//

#include "Statistics.h"

#include "rgaa_common/RLog.h"

constexpr int kMaxNetworkTime = 120;

namespace rgaa {

    Statistics::Statistics(const std::shared_ptr<Context>& ctx, const std::shared_ptr<SailfishSDK>& sdk) {
        context_ = ctx;
        sdk_ = sdk;
    }

    void Statistics::AppendVideoFrameNetworkTime(uint64_t frame_idx, uint64_t diff) {
        video_frame_network_times_.insert({frame_idx, diff});
        if (video_frame_network_times_.size() > kMaxNetworkTime) {
            auto it = video_frame_network_times_.begin();
            video_frame_network_times_.erase(it);
            LOGI("remove : {} {} ", (*it).first, (*it).second);
        }
    }

    uint64_t Statistics::GetFrameNetworkTime(uint64_t frame_idx) {
        if (video_frame_network_times_.find(frame_idx) != video_frame_network_times_.end()) {
            return video_frame_network_times_[frame_idx];
        }
        return 0;
    }

}