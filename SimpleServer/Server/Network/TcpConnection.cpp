#include "TcpConnection.h"

#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/regex.hpp>
#include <boost/asio/streambuf.hpp>

#include "CommandResult.h"
#include "GetCommand.h"
#include "SetCommand.h"

using namespace boost::asio;

namespace
{
	using namespace server::network;

	void ReadGetCommandBody(std::string_view buffer);
	void ReadSetCommandBody(std::string_view buffer);

	void ReadCommand(std::string_view buffer)
	{

		// Get
		if (const auto pos = buffer.find("$get "); pos != std::string::npos)
		{
			ReadGetCommandBody(buffer.substr(pos + 5));
			return;
		}

		// Set
		if (const auto pos = buffer.find("$set "); pos != std::string::npos)
		{
			ReadSetCommandBody(buffer.substr(pos + 5));
			return;
		}

		// TODO: treat the message as an invalid one
	}

	void ReadGetCommandBody(std::string_view buffer)
	{
		assert(size(buffer) > 0);
		GetCommand cmd{ std::string(buffer) };
		//...
	}

	void ReadSetCommandBody(std::string_view buffer)
	{
		assert(size(buffer) > 0);

		if (const auto delimPos = buffer.find('='); delimPos != std::string::npos)
		{
			std::string Key;
			std::string Value;

			Key = buffer.substr(0, delimPos);
			Value = buffer.substr(delimPos + 1);

			SetCommand cmd(Key, Value);
			// ...
			return;
		}

		// TODO: treat the message as invalid one
	}

	std::string ResultToString(const CommandResult& result)
	{
		return result.Key + '=' + result.Value;
	}
}

TcpConnection::TcpConnection(io_context& ioContext, ConnectionId id):
Socket(ioContext),
Id(id)
{
}

void TcpConnection::Open()
{
	constexpr auto delimiter = '\0';

	// Read until we meet the delimiter and then parse the command
	// TODO: it would be better to move the parsing logic into a separate class
	// TODO: read deadline
	async_read_until(Socket, Buffer, delimiter, [&](const boost::system::error_code& ec, size_t count)
	{
		if (ec)
		{
			// TODO: handle receive errors
			return;
		}

		std::string messageString;
		std::istream stream(&Buffer);

		stream >> messageString;

		ReadCommand(std::string_view(messageString).substr(0, size(messageString) - 1));
	});
}

void TcpConnection::Close()
{
	// TODO
}

std::shared_ptr<TcpConnection> TcpConnection::Create(
	io_context& io_context, ConnectionId id)
{
	return std::make_shared<TcpConnection>(io_context, id);
}

void TcpConnection::Send(const CommandResult& commandResult)
{
	Send(ResultToString(commandResult));
}

void TcpConnection::Send(const std::string& messageString)
{
	constexpr auto delimiter = '\0';

	streambuf buff(size(messageString) + sizeof delimiter);
	std::ostream stream(&buff);

	stream << messageString << delimiter;

	async_write(Socket, buff, [&](const boost::system::error_code& ec, size_t count)
	{
		if(ec)
		{
			// TODO: handle write errors
		}

		// ... 
	});
}
