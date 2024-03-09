#pragma once
#include "../Domain/Application/ApplicationContext.h"

namespace server
{
	class ServerApplication
	{
	public:

		ServerApplication() noexcept;

	private:
		domain::application::ApplicationContext DomainApp;
	};
}

