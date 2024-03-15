#pragma once
#include <memory>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "NetworkDefines.h"

namespace server::network
{
	class ServerNetworkManager;
	class CommandResult;

	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:

		explicit TcpConnection(ServerNetworkManager* networkManager, boost::asio::io_context& ioContext, ConnectionId id);

		static std::shared_ptr<TcpConnection> Create(boost::asio::io_context& io_context, ConnectionId id, ServerNetworkManager* networkManager);

		void Open();
		void Close();

		ConnectionId GetId() const { return Id; }

		void Send(const CommandResult& commandResult);

	private:

		void AsyncRead();

		void Send(const std::string& messageString);

		void ProcessCommand(std::string_view buffer);
		void ProcessGetCommand(std::string_view buffer);
		void ProcessSetCommand(std::string_view buffer);

	private:

		friend class ServerNetworkManager;

		ServerNetworkManager* NetworkManager;

		boost::asio::streambuf ReceiveBuffer;
		boost::asio::streambuf SendBuffer;
		boost::asio::ip::tcp::socket Socket;

		ConnectionId Id;
	};
}
