#pragma once
#include <string>

namespace server::network
{
	class CommandResult
	{
	public:

		explicit CommandResult() = default;
		explicit CommandResult(std::string key, std::string value) noexcept
			: Key(std::move(key)),
			  Value(std::move(value))
		{}


		std::string Key;
		std::string Value;
	};
}
