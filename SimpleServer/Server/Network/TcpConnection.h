#pragma once
#include <memory>
#include <boost/asio/ip/tcp.hpp>

namespace server::network
{
	class CommandResult;

	class TcpConnection : public std::enable_shared_from_this<TcpConnection>
	{
	private:


	public:

		explicit TcpConnection(boost::asio::io_context& ioContext, uint32_t id);

		void Open();
		void Close();

		static std::shared_ptr<TcpConnection> Create(boost::asio::io_context& io_context, uint32_t id);

		uint32_t GetId() const { return Id; }

		void Send(const CommandResult& commandResult);

	private:

		void Send(const std::string& messageString);

	private:

		friend class AsioNetworkManager;

		std::vector<uint8_t> Buffer;

		boost::asio::ip::tcp::socket Socket;

		uint32_t Id;
	};
}
