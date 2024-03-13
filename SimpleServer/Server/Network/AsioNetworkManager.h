#pragma once

#include "boost/asio.hpp"

#include "INetworkManager.h"
#include "TcpConnection.h"

namespace server::network
{
	class SetCommand;
	class GetCommand;
	class TcpConnection;

	/**
	 * A simple single threaded async server
	 * Runs in separate thread
	 */
	class AsioNetworkManager : public INetworkManager
	{
	public:

		void Initialize(ServerApplication* application) override;

		void Run() override;
		void Stop() override;

		void Send(const CommandResult& commandResult, ConnectionId connectionId) override;

		void OnGetCommandReceived(const GetCommand& command) const;
		void OnSetCommandReceived(const SetCommand& command) const;

	private:

		void StartAsioServer();

		void StartAccept();
		void OnAccept(const std::shared_ptr<TcpConnection>& conn);

		std::shared_ptr<TcpConnection> GetConnection(ConnectionId id) const;

		static ConnectionId GenerateConnectionId();

	private:


		std::unordered_map<ConnectionId, std::shared_ptr<TcpConnection>> Connections;

		std::unique_ptr<std::thread> WorkingThread;

		boost::asio::io_service IOService;

		std::unique_ptr<boost::asio::ip::tcp::acceptor> Acceptor;

		ServerApplication* Application = nullptr;

		bool IsInitialized = false;
	};
}

