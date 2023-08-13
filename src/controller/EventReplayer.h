#pragma once

#include <memory>

namespace rgaa
{

	class NetMessage;
	class Context;
	class MouseReplayer;
	class KeyboardReplayer;

	class EventReplayer {

	public:

		explicit EventReplayer(const std::shared_ptr<Context>& ctx);
		~EventReplayer();

		void Replay(const std::shared_ptr<NetMessage>& msg);

	private:

		std::shared_ptr<Context> context = nullptr;
		std::shared_ptr<MouseReplayer> mouse_replayer = nullptr;
		std::shared_ptr<KeyboardReplayer> keyboard_replayer = nullptr;

	};

}