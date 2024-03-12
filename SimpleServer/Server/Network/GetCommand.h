#pragma once
#include <string>

#include "NetworkDefines.h"

namespace server::network
{
	class GetCommand
	{
	public:

		GetCommand() = default;
		explicit GetCommand(ConnectionId connectionId, std::string key) noexcept:
		Key(std::move(key)),
		ConnectionId(connectionId)
		{}

		std::string Key;

		ConnectionId ConnectionId;
	};
}
