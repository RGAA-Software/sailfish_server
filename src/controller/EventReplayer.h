#pragma once

#include <memory>

namespace rgaa
{

	class Message;
	class Context;
	class MouseReplayer;
	class KeyboardReplayer;

	class EventReplayer {

	public:

		EventReplayer(std::shared_ptr<Context> ctx);
		~EventReplayer();

		void Replay(std::shared_ptr<Message> msg);

	private:

		std::shared_ptr<Context> context = nullptr;
		std::shared_ptr<MouseReplayer> mouse_replayer = nullptr;
		std::shared_ptr<KeyboardReplayer> keyboard_replayer = nullptr;

	};

}