#pragma once

#include <memory>

namespace rgaa
{

	class NetMessage;
	class Context;

	class MouseReplayer {
	public:
		
		explicit MouseReplayer(const std::shared_ptr<Context>& ctx);
		~MouseReplayer();

		void ReplayGlobalAbsolute(const std::shared_ptr<NetMessage>& msg);

	private:

		std::shared_ptr<Context> context;

	};

}