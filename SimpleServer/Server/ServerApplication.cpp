#include "ServerApplication.h"
#include <variant>

#include "Models/DataEntry.h"
#include "Network/TcpConnection.h"

using namespace domain;
using namespace server::network;

server::ServerApplication::ServerApplication(std::unique_ptr<services::IStorageService> storage, std::unique_ptr<INetworkManager> networkManager) noexcept:
Storage(std::move(storage)),
NetworkManager(std::move(networkManager))
{

}

void server::ServerApplication::Run()
{
	NetworkManager->Initialize(this);

	while(!IsStopRequested)
	{
		
	}
}

void server::ServerApplication::Stop()
{
	IsStopRequested = true;

	NetworkManager->Shutdown();
}

void server::ServerApplication::ProcessCommandResult(ConnectionId connectionId, const CommandResult& result)
{
	// ...

	NetworkManager->Send(result, connectionId);
}

void server::ServerApplication::ProcessGetCommand(const GetCommand& command)
{
	const auto data = Storage->Find(command.Key);
	if(!data.has_value())
	{
		// Handle possible errors

		return;
	}

	const auto commandResult = CommandResult(data->Key, data->Value);

	ProcessCommandResult(command.ConnectionId, commandResult);
}

void server::ServerApplication::ProcessSetCommand(const SetCommand& command)
{
	Storage->Store({command.Key, command.Value});

	const auto commandResult = CommandResult(command.Key, command.Value);

	ProcessCommandResult(command.ConnectionId, commandResult);
}
