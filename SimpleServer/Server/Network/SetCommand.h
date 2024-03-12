#pragma once
#include <string>

#include "NetworkDefines.h"

namespace server::network
{
	class SetCommand
	{
	public:

		SetCommand() noexcept = default;
		explicit SetCommand(ConnectionId connectionId, std::string key, std::string value) noexcept:
		Key(std::move(key)),
		Value(std::move(value)),
		ConnectionId(connectionId)
		{}

		std::string Key;
		std::string Value;

		ConnectionId ConnectionId;
	};
}
