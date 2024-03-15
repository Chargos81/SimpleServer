#include "ServerNetworkManager.h"

#include <iostream>

#include "TcpConnection.h"
#include "../ServerApplication.h"

using namespace boost::asio;

void server::network::ServerNetworkManager::Initialize(ServerApplication* application)
{
	Application = application;
}

void server::network::ServerNetworkManager::Stop()
{
	post(IOService, [&]
	{
		IOService.stop();
	});

	for(auto&& thread : WorkerThreads)
	{
		thread.join();
	}
}

void server::network::ServerNetworkManager::Send(const CommandResult& commandResult, uint32_t connectionId)
{
	if (!IsInitialized) return;

	const auto connection = GetConnection(connectionId);
	if (!connection)
	{
		// TODO: error handling
		return;
	}

	connection->Send(commandResult);
}

void server::network::ServerNetworkManager::OnGetCommandReceived(const GetCommand& command) const
{
	assert(IsInitialized);

	Application->ProcessGetCommand(command);
}

void server::network::ServerNetworkManager::OnSetCommandReceived(const SetCommand& command) const
{
	assert(IsInitialized);

	Application->ProcessSetCommand(command);
}

void server::network::ServerNetworkManager::CloseConnection(const std::shared_ptr<TcpConnection>& connection)
{
	Connections.erase(connection->GetId());
	connection->Close();
}

void server::network::ServerNetworkManager::RunAsioServer()
{
	try
	{
		IOService.run();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what();
	}
}

void server::network::ServerNetworkManager::Run()
{
	IsInitialized = true;

	const int32_t port = 7777;
	const ip::address addr = ip::address::from_string("127.0.0.1");

	const auto ep = ip::tcp::endpoint(addr, port);

	Acceptor = std::make_unique<ip::tcp::acceptor>(IOService, ep);

	StartAccept();

	for (size_t i = 0; i < ThreadsNum; i++)
	{
		WorkerThreads.emplace_back(&ServerNetworkManager::RunAsioServer, this);
	}

}

void server::network::ServerNetworkManager::StartAccept()
{
	const auto conn = TcpConnection::Create(IOService, GenerateConnectionId(), this);

	Acceptor->async_accept(conn->Socket, [=](auto ErrorCode)
	{
		OnAccept(conn);
	});
}

void server::network::ServerNetworkManager::OnAccept(const std::shared_ptr<TcpConnection>& conn)
{
	// It would be better to have some kind of connection factory

	std::cout << "Accepted a new connection\n";

	Connections[conn->GetId()] = conn;
	conn->Open();

	StartAccept();
}

std::shared_ptr<server::network::TcpConnection> server::network::ServerNetworkManager::GetConnection(ConnectionId id) const
{
	const auto result = Connections.find(id);

	return result != end(Connections) ? result->second : nullptr;
}

server::network::ConnectionId server::network::ServerNetworkManager::GenerateConnectionId()
{
	static ConnectionId nextId = 0;

	return nextId++;
}
