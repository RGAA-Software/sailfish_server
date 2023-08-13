#pragma once

#include <memory>

namespace rgaa
{

	class NetMessage;
	class Context;

	class KeyboardReplayer {
	public:
		
		explicit KeyboardReplayer(const std::shared_ptr<Context>& context);
		~KeyboardReplayer();

		void ReplayKeyboardGlobal(const std::shared_ptr<NetMessage>& msg);

	private:

		std::shared_ptr<Context> context = nullptr;

	};
}