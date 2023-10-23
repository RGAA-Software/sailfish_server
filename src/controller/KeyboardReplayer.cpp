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

    // todo: Alt key may be pressed all the time, press the RIGHT ALT to release it ...
    //
	void KeyboardReplayer::ReplayKeyboardGlobal(const std::shared_ptr<NetMessage>& msg) {
		auto& info = msg->keyboard_info();
		int vk = info.vk();
		bool pressed = info.pressed();
		int scancode = info.scancode();
		bool caps_lock = info.caps_lock();
		bool num_lock = info.num_lock();

		INPUT m_InPut;
        memset(&m_InPut, 0, sizeof(INPUT));
		m_InPut.type = INPUT_KEYBOARD;
		m_InPut.ki.wVk = vk;
        m_InPut.ki.dwFlags = KEYEVENTF_UNICODE;  //KEYEVENTF_SCANCODE;
        m_InPut.ki.wScan = 0;  //MapVirtualKey(vk, MAPVK_VK_TO_VSC);
        m_InPut.ki.dwExtraInfo = 0;
        m_InPut.ki.time = 0;

		if (!pressed) {
			m_InPut.ki.dwFlags |= KEYEVENTF_KEYUP;
		}
        if (IsSystemKey(vk)) {
            m_InPut.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
        }

        if (SendInput(1, &m_InPut, sizeof(INPUT)) != 1) {
            LOGE( "SendInput error : {0:x}", GetLastError());
        }
	}

    bool KeyboardReplayer::IsSystemKey(int vk) {
        static auto system_keys = std::vector<int> {
            VK_SHIFT, VK_CONTROL, VK_MENU, VK_LSHIFT, VK_RSHIFT,
            VK_LMENU, VK_RMENU, VK_RETURN, VK_DIVIDE, VK_LWIN,
            VK_RWIN, VK_HOME, VK_PRIOR, VK_NEXT, VK_END, VK_INSERT,
            VK_DELETE, VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_NUMLOCK,
            VK_CAPITAL,
        };

        for (auto k : system_keys) {
           if (vk == k) {
               return true;
           }
        }
        return false;
    }

}