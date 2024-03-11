#include "TcpConnection.h"

server::network::TcpConnection::TcpConnection(boost::asio::io_context& ioContext, uint32_t id):
Socket(ioContext),
Id(id)
{
	constexpr size_t bufferSize = 1024;
	Buffer.resize(bufferSize);
}

void server::network::TcpConnection::Open()
{
	Socket.async_receive(boost::asio::buffer(data(Buffer), size(Buffer)), [](boost::system::error_code ec, size_t count) {});
}

void server::network::TcpConnection::Close()
{
}

std::shared_ptr<server::network::TcpConnection> server::network::TcpConnection::Create(
	boost::asio::io_context& io_context, uint32_t id)
{
	return std::make_shared<TcpConnection>(io_context, id);
}

void server::network::TcpConnection::Send(const CommandResult& commandResult)
{

}

void server::network::TcpConnection::Send(const std::string& messageString)
{

}

