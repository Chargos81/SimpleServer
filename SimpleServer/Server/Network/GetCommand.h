#pragma once
#include <string>

namespace server::network
{
	class GetCommand
	{
	public:

		GetCommand() = default;
		explicit GetCommand(std::string key) noexcept:
		Key(std::move(key)){}

		std::string Key;
	};
}
