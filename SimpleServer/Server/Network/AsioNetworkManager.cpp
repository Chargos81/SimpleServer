#include "AsioNetworkManager.h"

#include "TcpConnection.h"
#include "../ServerApplication.h"

using namespace boost::asio;

void server::network::AsioNetworkManager::Initialize(ServerApplication* application)
{
	Application = application;
}

void server::network::AsioNetworkManager::Stop()
{
	post(IOService, [&]
	{
		IOService.stop();
	});

	WorkingThread->join();

	WorkingThread.reset();
}

void server::network::AsioNetworkManager::Send(const CommandResult& commandResult, uint32_t connectionId)
{
	if (!IsInitialized) return;

	post(IOService, [&]
	{
		const auto connection = GetConnection(connectionId);
		if(!connection)
		{
			// TODO: error handling
			return;
		}

		connection->Send(commandResult);
	});
}

void server::network::AsioNetworkManager::OnGetCommandReceived(const GetCommand& command) const
{
	assert(IsInitialized);

	Application->ProcessGetCommand(command);
}

void server::network::AsioNetworkManager::OnSetCommandReceived(const SetCommand& command) const
{
	assert(IsInitialized);

	Application->ProcessSetCommand(command);
}

void server::network::AsioNetworkManager::StartAsioServer()
{
	const int32_t port = 7777;
	const ip::address addr = ip::address::from_string("127.0.0.1");

	const auto ep = ip::tcp::endpoint(addr, port);

	Acceptor = std::make_unique<ip::tcp::acceptor>(IOService, ep);

	IOService.run();
}

void server::network::AsioNetworkManager::Run()
{
	IsInitialized = true;

	WorkingThread = std::make_unique<std::thread>(&AsioNetworkManager::StartAsioServer, this);
}

void server::network::AsioNetworkManager::StartAccept()
{
	const auto conn = TcpConnection::Create(IOService, GenerateConnectionId());

	Acceptor->async_accept(conn->Socket, [=](auto ErrorCode)
	{
		OnAccept(conn);
	});
}

void server::network::AsioNetworkManager::OnAccept(const std::shared_ptr<TcpConnection>& conn)
{
	// It would be better to have some kind of connection factory

	Connections[conn->GetId()] = conn;

	StartAccept();
}

std::shared_ptr<server::network::TcpConnection> server::network::AsioNetworkManager::GetConnection(ConnectionId id) const
{
	const auto result = Connections.find(id);

	return result != end(Connections) ? result->second : nullptr;
}

server::network::ConnectionId server::network::AsioNetworkManager::GenerateConnectionId()
{
	static ConnectionId nextId = 0;

	return nextId++;
}
