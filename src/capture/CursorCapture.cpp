//
// Created by RGAA on 2023/8/20.
//

#include "CursorCapture.h"

#include "context/Context.h"
#include "messages.pb.h"

#include "util/MonitorDetector.h"

namespace rgaa {

    CursorCapture::CursorCapture(const std::shared_ptr<Context>& ctx) {
        context_ = ctx;
        MonitorDetector::Instance()->DetectMonitors();
    }

    std::shared_ptr<NetMessage> CursorCapture::Capture() {
        CURSORINFO cursor_info = { 0 };
        cursor_info.cbSize = sizeof(CURSORINFO);
        GetCursorInfo(&cursor_info);

        std::string cursor_data;
        uint32_t width = 0;
        uint32_t height = 0;
        Point hotspot;
        GetCursorBitmap(cursor_info.hCursor, cursor_data, width, height, hotspot);

        int hotspot_x = hotspot.x;
        int hotspot_y = hotspot.y;
        int x = cursor_info.ptScreenPos.x;
        int y = cursor_info.ptScreenPos.y;

        auto monitor_detector = MonitorDetector::Instance();
        std::tuple<int, int, int> position = monitor_detector->CalculateInWhichMonitor(x);

        auto msg = std::make_shared<NetMessage>();
        msg->set_type(MessageType::kCursorInfo);
        auto info = new CursorInfo();
        msg->set_allocated_cursor_info(info);

        info->set_width(width);
        info->set_height(height);
        info->set_data(cursor_data);
        info->set_x(x);
        info->set_y(y);
        info->set_hotspot_x(hotspot_x);
        info->set_hotspot_y(hotspot_y);
        info->set_dup_idx(std::get<0>(position));
        info->set_tex_left(std::get<1>(position));
        info->set_tex_right(std::get<2>(position));

        return msg;
    }

}
