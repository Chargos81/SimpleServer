#pragma once
#include <string>
#include <vector>

namespace client::config
{
	class ClientConfiguration
	{
	public:

		std::vector<std::string> KeysList;
		std::vector<std::string> ValuesList;

		std::string ServerIpAddress;
		uint16_t ServerPort = 0;

		size_t RequestCount = 0;
	};
}
