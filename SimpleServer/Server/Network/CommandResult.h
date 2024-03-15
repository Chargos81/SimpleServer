#pragma once
#include <string>

namespace server::network
{
	class CommandResult
	{
	public:

		explicit CommandResult() = default;
		explicit CommandResult(std::string key, std::string value, size_t reads, size_t writes) noexcept
			: Key(std::move(key)),
			  Value(std::move(value)),
		      Reads(reads),
			  Writes(writes)
		{}

		std::string Key;
		std::string Value;
		size_t Reads = 0;
		size_t Writes = 0;
	};
}
