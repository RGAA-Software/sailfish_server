#pragma once

#include <memory>

namespace rgaa
{

	class Message;
	class Context;

	class MouseReplayer {
	public:
		
		MouseReplayer(std::shared_ptr<Context> ctx);
		~MouseReplayer();

		void ReplayGlobalAbsolute(std::shared_ptr<Message> msg);
		void ReplayAppOnly(std::shared_ptr<Message> msg);

	private:

		std::shared_ptr<Context> context;

	};

}