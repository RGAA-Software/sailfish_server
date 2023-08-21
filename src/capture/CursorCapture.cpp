//
// Created by RGAA on 2023/8/20.
//

#include "CursorCapture.h"

#include "context/Context.h"
#include "messages.pb.h"

namespace rgaa {

    CursorCapture::CursorCapture(const std::shared_ptr<Context>& ctx) {
        context_ = ctx;
    }

    std::shared_ptr<NetMessage> CursorCapture::Capture() {
        static HCURSOR cursor_hand = LoadCursorW(NULL, IDC_HAND);
        static HCURSOR cursor_ibeam = LoadCursorW(NULL, IDC_IBEAM);

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

        return msg;
//        if (cursor_callback && width > 0 && height > 0 && cursor_data.size() > 0) {
//            auto data = Data::Make(cursor_data.data(), cursor_data.size());
//            auto cursor_info_msg = std::make_shared<CursorInfoMessage>();
//            cursor_info_msg->data = data;
//            cursor_info_msg->width = width;
//            cursor_info_msg->height = height;
//            cursor_info_msg->x = x;
//            cursor_info_msg->y = y;
//            if (capture_monitor) {
//                cursor_info_msg->x_in_self_monitor = x - capture_monitor->left;
//                cursor_info_msg->y_in_self_monitor = y;
//                cursor_info_msg->screen_width = capture_monitor->width;
//                cursor_info_msg->screen_height = capture_monitor->height;
//                cursor_info_msg->xf = cursor_info_msg->x_in_self_monitor * 1.0f / cursor_info_msg->screen_width;
//                cursor_info_msg->yf = cursor_info_msg->y_in_self_monitor * 1.0f / cursor_info_msg->screen_height;
//            }
//
//            if (cursor_hand == cursor_info.hCursor) {
//                cursor_info_msg->type = rgaa::CursorType::kIDC_HAND;
//            }
//            else if (cursor_ibeam == cursor_info.hCursor) {
//                cursor_info_msg->type = rgaa::CursorType::kIDC_IBEAM;
//            }
//            cursor_callback(cursor_info_msg);
//        }
        return nullptr;
    }

}
