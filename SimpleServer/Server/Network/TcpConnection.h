#pragma once
#include <memory>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "NetworkDefines.h"

namespace server::network
{
	class CommandResult;

	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	public:

		explicit TcpConnection(boost::asio::io_context& ioContext, ConnectionId id);

		static std::shared_ptr<TcpConnection> Create(boost::asio::io_context& io_context, ConnectionId id);

		void Open();
		void Close();

		ConnectionId GetId() const { return Id; }

		void Send(const CommandResult& commandResult);

	private:

		void Send(const std::string& messageString);

		void ReadCommand(std::string_view buffer);
		void ReadGetCommandBody(std::string_view buffer);
		void ReadSetCommandBody(std::string_view buffer);

	private:

		friend class AsioNetworkManager;

		boost::asio::streambuf Buffer;
		boost::asio::ip::tcp::socket Socket;

		ConnectionId Id;
	};
}
