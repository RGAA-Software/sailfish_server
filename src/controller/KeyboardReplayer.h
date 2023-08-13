#pragma once

#include <memory>

namespace rgaa
{

	class Message;
	class Context;

	class KeyboardReplayer {
	public:
		
		KeyboardReplayer(std::shared_ptr<Context> context);
		~KeyboardReplayer();

		void ReplayKeyboard(std::shared_ptr<Message> msg);
		void ReplayKeyboardAppOnly(std::shared_ptr<Message> msg);

	private:

		std::shared_ptr<Context> context = nullptr;

	};
}