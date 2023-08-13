#include "MouseReplayer.h"

#include "messages.pb.h"
#include "rgaa_common/RLog.h"
#include "rgaa_common/RWindow.h"
#include "context/Context.h"
#include "capture/Monitor.h"

#include "messages.pb.h"

constexpr auto kMaxNDCValue = 65535;

namespace rgaa
{

	MouseReplayer::MouseReplayer(const std::shared_ptr<Context>& ctx) {
		this->context = ctx;
	}

	MouseReplayer::~MouseReplayer() {

	}

	void MouseReplayer::ReplayGlobalAbsolute(const std::shared_ptr<NetMessage>& msg) {
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

		int real_x = (int)(kMaxNDCValue * x);
		int real_y = (int)(kMaxNDCValue * y);

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

}