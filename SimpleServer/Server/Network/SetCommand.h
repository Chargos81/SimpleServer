#pragma once
#include <string>

namespace server::network
{
	class SetCommand
	{
	public:

		SetCommand() noexcept = default;
		explicit SetCommand(std::string key, std::string value) noexcept:
		Key(std::move(key)),
		Value(std::move(value)){}

		std::string Key;
		std::string Value;
	};
}
