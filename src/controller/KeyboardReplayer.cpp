#include "KeyboardReplayer.h"

#include "messages.pb.h"
#include "rgaa_common/RLog.h"
#include "context/Context.h"


namespace rgaa
{

	KeyboardReplayer::KeyboardReplayer(const std::shared_ptr<Context>& ctx) {
		this->context = ctx;
	}

	KeyboardReplayer::~KeyboardReplayer() {

	}

	void KeyboardReplayer::ReplayKeyboardGlobal(const std::shared_ptr<NetMessage>& msg) {
		auto& info = msg->keyboard_info();
		int vk = info.vk();
		bool pressed = info.pressed();
		int scancode = info.scancode();
		bool caps_lock = info.caps_lock();
		bool num_lock = info.num_lock();
		LOGI("vk : {}, pressed : {}, scancode : {}, caps lock : {}, num lock : {} ", vk, pressed, scancode, caps_lock, num_lock);

		INPUT m_InPut;
        memset(&m_InPut, 0, sizeof(INPUT));
		m_InPut.type = INPUT_KEYBOARD;
		m_InPut.ki.wVk = vk;
        m_InPut.ki.wScan = MapVirtualKey(vk, 0);
		if (pressed) {
			m_InPut.ki.dwFlags = KEYEVENTF_KEYUP;
		}
		SendInput(1, &m_InPut, sizeof(INPUT));
	}

}