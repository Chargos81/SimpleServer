#pragma once

#include "boost/asio.hpp"

#include "INetworkManager.h"

namespace server::network
{
	class SetCommand;
	class GetCommand;
	class TcpConnection;

	/**
	 * A simple singlethreaded async server
	 * Runs in separate thread
	 */
	class AsioNetworkManager : public INetworkManager
	{
	public:

		void Initialize(ServerApplication* application) override;
		void Shutdown() override;

		void Send(const CommandResult& commandResult, uint32_t connectionId) override;

		void OnGetCommandReceived(const GetCommand& command) const;
		void OnSetCommandReceived(const SetCommand& command) const;

	private:

		void Run();

		void StartAccept();
		void OnAccept(const std::shared_ptr<TcpConnection>& conn);

		std::shared_ptr<TcpConnection> GetConnection(uint32_t id) const;

		static uint32_t GenerateConnectionId();

	private:

		/**
		 * Connections to their ids
		 */
		std::unordered_map<uint32_t, std::shared_ptr<TcpConnection>> Connections;

		std::unique_ptr<std::thread> WorkingThread;

		boost::asio::io_service IOService;
		boost::asio::ip::tcp::acceptor Acceptor;

		ServerApplication* Application = nullptr;

		bool IsInitialized = false;
	};
}

