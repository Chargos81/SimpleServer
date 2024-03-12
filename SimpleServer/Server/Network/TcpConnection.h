#pragma once
#include <memory>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace server::network
{
	using ConnectionId = uint32_t;

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

	private:

		friend class AsioNetworkManager;

		boost::asio::streambuf Buffer;

		boost::asio::ip::tcp::socket Socket;

		ConnectionId Id;
	};
}
