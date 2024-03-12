#include "ServerApplication.h"
#include <variant>

#include "Models/DataEntry.h"

using namespace domain;
using namespace server::network;

server::ServerApplication::ServerApplication(std::unique_ptr<domain::services::IStorageService> storage, std::unique_ptr<INetworkManager> networkManager) noexcept:
Storage(std::move(storage)),
NetworkManager(std::move(networkManager))
{

}

void server::ServerApplication::Run()
{
	NetworkManager->Initialize(this);
	WorkerPool.Start(5);

	while(!IsStopRequested)
	{
		{
			std::unique_lock lock(Mu);
			while(Messages.empty())
			{
				CV.wait(lock, [&] {return Messages.empty(); });
			}

			ApplicationMessage msg;
			Messages.front().swap(msg);
			Messages.pop();
			std::visit(ApplicationMessageVisitor(this),
				msg);
		}

	}
}

void server::ServerApplication::Stop()
{
	IsStopRequested = true;

	NetworkManager->Shutdown();
	WorkerPool.Stop();
}

void server::ServerApplication::ReceiveMessage(const ApplicationMessage& msg)
{
	Messages.push(msg);
}

void server::ServerApplication::ProcessCommandResult(const CommandResult& result)
{
	// ...
	const uint32_t stubId = 0;
	NetworkManager->Send(result, stubId);
}

void server::ServerApplication::ProcessGetCommand(const GetCommand& command)
{
	WorkerPool.AddJob([&]
	{
		const auto data = Storage->Find(command.Key);
		if(!data.has_value())
		{
			// Handle possible errors

			return;
		}

		auto commandResult = CommandResult(data->Key, data->Value);

		ReceiveMessage(commandResult);
	});
}

void server::ServerApplication::ProcessSetCommand(const SetCommand& command)
{
	WorkerPool.AddJob([&]
		{
			Storage->Store({command.Key, command.Value});

			auto commandResult = CommandResult(command.Key, command.Value);

			ReceiveMessage(commandResult);
		});
}
