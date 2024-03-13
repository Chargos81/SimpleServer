#pragma once

namespace server
{
	class ServerApplication;
}

namespace server::network
{
	class CommandResult;

	class INetworkManager
	{
	public:

		virtual void Initialize(ServerApplication* application) = 0;
		virtual void Run() = 0;
		virtual void Stop() = 0;
		virtual void Send(const CommandResult& commandResult, uint32_t connectionId) = 0;

		virtual ~INetworkManager() = default;
	};
}
