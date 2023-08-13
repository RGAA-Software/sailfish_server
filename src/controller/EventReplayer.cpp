#include "EventReplayer.h"

#include "messages.pb.h"
#include "MouseReplayer.h"
#include "KeyboardReplayer.h"
#include "config/config.h"
#include "Context.h"
#include "common/RLog.h"

namespace rgaa
{

	EventReplayer::EventReplayer(std::shared_ptr<Context> ctx) {
		context = ctx;
		mouse_replayer = std::make_shared<MouseReplayer>(ctx);
		keyboard_replayer = std::make_shared<KeyboardReplayer>(ctx);
	}

	EventReplayer::~EventReplayer() {

	}

	void EventReplayer::Replay(std::shared_ptr<Message> msg) {
		auto config = context->GetConfig();
		if (msg->has_mouse_info()) {
			LOGI("Mouse info comming...");
			// global // special window
			if (config->running.replay_mode == Running::kGlobal) {
				if (config->running.mouse_mode == Running::kAbsolute) {
					mouse_replayer->ReplayGlobalAbsolute(msg);
				}
			}
			else if (config->running.replay_mode == Running::kAppOnly) {
				mouse_replayer->ReplayAppOnly(msg);
			}
		}
		else if (msg->has_keyboard_info()) {
			LOGI("Keyboard info comming... : {}, vk : {} ", (int)config->running.replay_mode, msg->keyboard_info().vk());
			if (config->running.replay_mode == Running::kGlobal) {
				keyboard_replayer->ReplayKeyboard(msg);
			}
			else if (config->running.replay_mode == Running::kAppOnly) {
				keyboard_replayer->ReplayKeyboardAppOnly(msg);
			}
		}
	}

}