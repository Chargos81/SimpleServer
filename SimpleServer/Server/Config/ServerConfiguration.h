#pragma once
#include <filesystem>
#include <string>

namespace server::config
{
	class ServerConfiguration
	{
	public:

		std::string ListenIp = "0.0.0.0";
		uint16_t ListenPort = 0;

		size_t WorkerThreadsPoolSize = 1;
	};
}
