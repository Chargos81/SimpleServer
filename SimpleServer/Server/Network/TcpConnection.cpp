#include "TcpConnection.h"

#include <iostream>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/regex.hpp>

#include "ServerNetworkManager.h"
#include "CommandResult.h"
#include "GetCommand.h"
#include "SetCommand.h"

using namespace boost::asio;

namespace
{
	using namespace server::network;

	constexpr auto delimiter = '\0';

	void SerializeResultToStream(std::ostream& stream, const CommandResult& result)
	{
		stream << result.Key << '=' << result.Value << "\nreads=" << result.Reads << "\nwrites=" << result.Writes;
	}
}

TcpConnection::TcpConnection(ServerNetworkManager* networkManager, io_context& ioContext, ConnectionId id):
NetworkManager(networkManager),
Socket(ioContext),
Id(id)
{
}

void TcpConnection::Open()
{
	AsyncRead();
}

void TcpConnection::Close()
{
	std::cout << "Connection closed\n";

	if(Socket.is_open())
	{
		Socket.close();
	}
}

std::shared_ptr<TcpConnection> TcpConnection::Create(
	io_context& io_context, ConnectionId id, ServerNetworkManager* networkManager)
{
	return std::make_shared<TcpConnection>(networkManager, io_context, id);
}

void TcpConnection::Send(const CommandResult& commandResult)
{
	std::ostream stream(&SendBuffer);

	SerializeResultToStream(stream, commandResult);

	stream << delimiter;

	async_write(Socket, SendBuffer, [&](const boost::system::error_code& ec, size_t count)
		{
			if (ec)
			{
				// TODO: handle write errors
			}

			// ... 
		});
}

void TcpConnection::AsyncRead()
{
	// Read until we meet the delimiter and then parse the command
	// TODO: it would be better to move the parsing logic into a separate class
	// TODO: read timeouts
	async_read_until(Socket, ReceiveBuffer, delimiter, [&](const boost::system::error_code& ec, size_t count)
		{
			if (ec)
			{
				if (ec == error::eof || ec == error::connection_reset)
				{
					NetworkManager->CloseConnection(shared_from_this());
				}

				return;
			}

			std::istream stream(&ReceiveBuffer);

			std::string messageString;
			std::getline(stream, messageString, delimiter);

			ProcessCommand(std::string_view(messageString));

			AsyncRead();
		});
}

void TcpConnection::ProcessCommand(std::string_view buffer)
{
	// TODO: it should be a set of message handlers to process different message types
	// Get
	if (const auto pos = buffer.find("$get "); pos != std::string::npos)
	{
		ProcessGetCommand(buffer.substr(pos + 5));
		return;
	}

	// Set
	if (const auto pos = buffer.find("$set "); pos != std::string::npos)
	{
		ProcessSetCommand(buffer.substr(pos + 5));
		return;
	}

	// TODO: treat the message as an invalid one
}

void TcpConnection::ProcessGetCommand(std::string_view buffer)
{
	assert(size(buffer) > 0);

	const GetCommand cmd{ GetId(), std::string(buffer) };

	NetworkManager->OnGetCommandReceived(cmd);
}

void TcpConnection::ProcessSetCommand(std::string_view buffer)
{
	assert(size(buffer) > 0);

	if (const auto delimPos = buffer.find('='); delimPos != std::string::npos)
	{
		std::string Key;
		std::string Value;

		Key = buffer.substr(0, delimPos);
		Value = buffer.substr(delimPos + 1);

		const SetCommand cmd(GetId(), Key, Value);
		
		NetworkManager->OnSetCommandReceived(cmd);
		return;
	}

	// TODO: treat the message as invalid one
}
