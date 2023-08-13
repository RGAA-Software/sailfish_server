#include "MouseReplayer.h"

#include "messages.pb.h"
#include "common/RLog.h"
#include "common/RWindow.h"
#include "Context.h"
#include "AppManager.h"
#include "ipc/IPCMessage.h"
#include "ipc/InterCommServer.h"
#include "monitor/monitor.h"

//#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>

constexpr auto kMaxNDCValue = 65535;

namespace rgaa
{

	MouseReplayer::MouseReplayer(std::shared_ptr<Context> ctx) {
		this->context = ctx;
	}

	MouseReplayer::~MouseReplayer() {

	}

	void MouseReplayer::ReplayGlobalAbsolute(std::shared_ptr<Message> msg) {
		auto& info = msg->mouse_info();
		MouseKey key = info.key();
		bool pressed = info.pressed();
		bool released = info.released();
		float x = info.mouse_x();
		float y = info.mouse_y();
		float dx = info.mouse_dx();
		float dy = info.mouse_dy();
		int scroll = info.middle_scroll();

		int cx_screen = ::GetSystemMetrics(SM_CXSCREEN);
		int cy_screen = ::GetSystemMetrics(SM_CYSCREEN);

		int real_x = 65535 * x;
		int real_y = 65535 * y;

		LOG_INFO("key : %d, pressed : %d, x : %f, y : %f, dx : %f, dy : %f", key, pressed, x, y, dx, dy);

		LOG_INFO("cx screen : %d x %d, real : %d x %d", cx_screen, cy_screen, real_x, real_y);

		INPUT input;
		input.type = INPUT_MOUSE;
		input.mi.dx = real_x;
		input.mi.dy = real_y;
		input.mi.mouseData = 0;
		input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
		if (scroll == 0) {
			input.mi.dwFlags |= MOUSEEVENTF_MOVE;
		}
		else {
			input.mi.dwFlags |= MOUSEEVENTF_WHEEL;
			input.mi.mouseData = scroll;
		}

		if (pressed) {
			if (key == MouseKey::kLeft) {
				input.mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
			}
			else if (key == MouseKey::kRight) {
				input.mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
			}
			else if (key == MouseKey::kMiddle) {
				input.mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
			}
		}
		else if (released) {
			if (key == MouseKey::kLeft) {
				input.mi.dwFlags |= MOUSEEVENTF_LEFTUP;
			}
			else if (key == MouseKey::kRight) {
				input.mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
			}
			else if (key == MouseKey::kMiddle) {
				input.mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
			}
		}
		input.mi.time = 0;
		input.mi.dwExtraInfo = 0;

		SendInput(1, &input, sizeof(INPUT));
	}

	void MouseReplayer::ReplayAppOnly(std::shared_ptr<Message> msg) {
		auto win_info = context->GetAppManager()->GetWindowInfo();
		if (!win_info.win_handle) {
			return;
		}

		auto win_size = win_info.GetWindowSize();

		auto& info = msg->mouse_info();
		MouseKey key = info.key();
		bool pressed = info.pressed();
		bool released = info.released();
		float x = info.mouse_x();
		float y = info.mouse_y();
		float dx = info.mouse_dx();
		float dy = info.mouse_dy();
		int scroll = info.middle_scroll();

		RECT app_position = {0, 0, 0, 0};
		bool obtain_pos_result = WinInfo::GetWindowPositionByHwnd(win_info.win_handle, app_position);
		if (!obtain_pos_result) {
			//LOG_INFO("Don't find app position when replay mouse.");
			return;
		}

		//LOG_INFO("app pos, left : %d, top %d, right %d, bottom %d", app_position.left, app_position.top, app_position.right, app_position.bottom);

		auto monitor = MonitorDetector::Instance()->GetPrimaryMonitor();
		if (!monitor) {
			LOG_INFO("Don't have monitor.");
			return;
		}

		int app_width = app_position.right - app_position.left;
		int app_height = app_position.bottom - app_position.top;
		
		int x_pos = app_position.left + app_width * x;
		int y_pos = app_position.top + app_height * y;
		int dx_pos = dx * monitor->width;
		int dy_pos = dy * monitor->height;

		LOG_INFO("screen width : %d, screen height : %d", monitor->width, monitor->height);

		auto message = IPCMouseMessage::MakeEmptyMessage();
		message->type = IPCMessageType::kSharedMouseEvent;
		message->sender = IPCMessageSender::kSenderHost;
		message->hwnd = (uint64_t)win_info.win_handle;
		message->key = key;
		message->pressed = pressed;
		message->released = released;
		message->mouse_x = x_pos;
		message->mouse_y = y_pos;
		message->mouse_dx = dx_pos;
		message->mouse_dy = dy_pos;
		message->middle_scroll = scroll;
		auto data = IPCMouseMessage::ConvertToData(message);
		context->GetInterCommServer()->SendForward(data);
	}


}