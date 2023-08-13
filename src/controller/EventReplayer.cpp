#include "EventReplayer.h"

#include "messages.pb.h"
#include "MouseReplayer.h"
#include "KeyboardReplayer.h"
#include "context/Context.h"
#include "rgaa_common/RLog.h"

namespace rgaa
{

	EventReplayer::EventReplayer(const std::shared_ptr<Context>& ctx) {
		context = ctx;
		mouse_replayer = std::make_shared<MouseReplayer>(ctx);
		keyboard_replayer = std::make_shared<KeyboardReplayer>(ctx);
	}

	EventReplayer::~EventReplayer() {

	}

	void EventReplayer::Replay(const std::shared_ptr<NetMessage>& msg) {
		if (msg->has_mouse_info()) {
            mouse_replayer->ReplayGlobalAbsolute(msg);
		}
		else if (msg->has_keyboard_info()) {
            keyboard_replayer->ReplayKeyboardGlobal(msg);
		}
	}

}