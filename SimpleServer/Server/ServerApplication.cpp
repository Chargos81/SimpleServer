#include "ServerApplication.h"

#include <iostream>
#include <variant>

#include "Models/DataEntry.h"
#include "Network/TcpConnection.h"

using namespace domain;
using namespace server::network;

server::ServerApplication::ServerApplication(std::unique_ptr<services::IStorageService> storage, std::unique_ptr<INetworkManager> networkManager,
	std::unique_ptr<services::IStatsService> stats) noexcept:
Storage(std::move(storage)),
NetworkManager(std::move(networkManager)),
Stats(std::move(stats))
{

}

void server::ServerApplication::Run()
{
	try
	{
		NetworkManager->Initialize(this);
		NetworkManager->Run();

		Storage->Run();
		Stats->Run();

		while(!IsStopRequested)
		{
			std::this_thread::sleep_for(std::chrono::seconds(60));
		}
	}
	catch (const std::exception& e)
	{
		std::cout << e.what();

		Stop();
	}
}

void server::ServerApplication::Stop()
{
	IsStopRequested = true;

	NetworkManager->Stop();
	Storage->Stop();
	Stats->Stop();
}

void server::ServerApplication::ProcessCommandResult(ConnectionId connectionId, const CommandResult& result)
{
	// ...

	NetworkManager->Send(result, connectionId);
}

void server::ServerApplication::ProcessGetCommand(const GetCommand& command)
{
	// We may have a situation when the requested key is not found in the storage
	// I decided to just return an empty value
	auto data = Storage->Find(command.Key);
	if(!data.has_value())
	{
		data = { command.Key, "" };
	}

	const auto dataStats = Stats->UpdateReads(command.Key);
	const auto commandResult = CommandResult(data->Key, data->Value, dataStats.Reads, dataStats.Writes);

	ProcessCommandResult(command.ConnectionId, commandResult);
}

void server::ServerApplication::ProcessSetCommand(const SetCommand& command)
{
	Storage->Store({command.Key, command.Value});

	const auto dataStats = Stats->UpdateWrites(command.Key);
	const auto commandResult = CommandResult(command.Key, command.Value, dataStats.Reads, dataStats.Writes);

	ProcessCommandResult(command.ConnectionId, commandResult);
}
