#include "KeyboardReplayer.h"

#include "messages.pb.h"
#include "common/RLog.h" 
#include "ipc/IPCMessage.h"
#include "ipc/InterCommServer.h"
#include "Context.h"
#include "AppManager.h"

namespace rgaa
{

	KeyboardReplayer::KeyboardReplayer(std::shared_ptr<Context> ctx) {
		this->context = ctx;
	}

	KeyboardReplayer::~KeyboardReplayer() {

	}

	void KeyboardReplayer::ReplayKeyboard(std::shared_ptr<Message> msg) {
		auto& info = msg->keyboard_info();
		int vk = info.vk();
		bool pressed = info.pressed();
		int scancode = info.scancode();
		bool caps_lock = info.caps_lock();
		bool num_lock = info.num_lock();
		LOGI("vk : {}, pressed : {}, scancode : {}, caps lock : {}, num lock : {} ", vk, pressed, scancode, caps_lock, num_lock);

		INPUT m_InPut;
		m_InPut.type = INPUT_KEYBOARD;
		m_InPut.ki.wVk = vk;
		if (pressed) {
			m_InPut.ki.dwFlags = KEYEVENTF_KEYUP;
		}
		SendInput(1, &m_InPut, sizeof(INPUT));
	}

	void KeyboardReplayer::ReplayKeyboardAppOnly(std::shared_ptr<Message> msg) {
		auto win_info = context->GetAppManager()->GetWindowInfo();
		if (!win_info.win_handle) {
			return;
		}

		auto& info = msg->keyboard_info();
		int vk = info.vk();
		bool pressed = info.pressed();
		int scancode = info.scancode();
		bool caps_lock = info.caps_lock();
		bool num_lock = info.num_lock();

		auto message = IPCKeyboardMessage::MakeEmptyMessage();
		message->type = IPCMessageType::kSharedKeyboardEvent;
		message->sender = IPCMessageSender::kSenderHost;
		message->hwnd = (uint64_t)win_info.win_handle;
		message->vk = vk;
		message->pressed = pressed;
		message->capslock = caps_lock;
		message->numlock = num_lock;
		LOGI("ReplayKeyboardAppOnly VK : {}, pressed : {} ", vk, pressed);
		auto data = IPCKeyboardMessage::ConvertToData(message);
		context->GetInterCommServer()->SendForward(data);
	}

}