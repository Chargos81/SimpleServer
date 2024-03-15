#pragma once

#include "boost/asio.hpp"

#include "INetworkManager.h"
#include "TcpConnection.h"
#include "../Config/ServerConfiguration.h"

namespace server::network
{
	class SetCommand;
	class GetCommand;
	class TcpConnection;

	/**
	 * A simple single threaded async server
	 * Runs in separate thread
	 * TODO: thread safe logging is required
	 */
	class ServerNetworkManager : public INetworkManager
	{
	public:

		explicit ServerNetworkManager(config::ServerConfiguration configuration) noexcept;

		void Initialize(ServerApplication* application) override;

		void Run() override;
		void Stop() override;

		void Send(const CommandResult& commandResult, ConnectionId connectionId) override;

		void OnGetCommandReceived(const GetCommand& command) const;
		void OnSetCommandReceived(const SetCommand& command) const;

		void CloseConnection(const std::shared_ptr<TcpConnection>& connection);

		void NotifyConnectionClosed(ConnectionId id);

	private:

		void RunAsioServer();

		void StartAccept();
		void OnAccept(const std::shared_ptr<TcpConnection>& conn);

		std::shared_ptr<TcpConnection> GetConnection(ConnectionId id) const;

		static ConnectionId GenerateConnectionId();

	private:

		config::ServerConfiguration Configuration;

		std::unordered_map<ConnectionId, std::shared_ptr<TcpConnection>> Connections;

		boost::asio::io_service IOService;

		std::unique_ptr<boost::asio::ip::tcp::acceptor> Acceptor;

		ServerApplication* Application = nullptr;

		std::vector<std::thread> WorkerThreads;
		size_t ThreadsNum = 4;

		bool IsInitialized = false;
	};
}

