#include "ClientApplication.h"

#include <iostream>
#include <boost/asio/io_service.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>

using namespace boost::asio;

namespace
{
	constexpr auto delimiter = '\0';
}

client::ClientApplication::ClientApplication(config::ClientConfiguration configuration) noexcept:
Configuration(std::move(configuration))
{

}

void client::ClientApplication::Run()
{
	Sock = std::make_unique<ip::tcp::socket>(IOContext);
	auto DummyWork = std::make_shared<io_service::work>(IOContext);

	Connect();

	IOContext.run();
}

void client::ClientApplication::Stop()
{
	std::cout << "Client - shutdown\n";

	IOContext.stop();
}

void client::ClientApplication::Connect()
{
	const auto addr = ip::address::from_string(Configuration.ServerIpAddress);
	ip::tcp::endpoint ep(addr, Configuration.ServerPort);

	Sock->async_connect(ep, [&](const boost::system::error_code& ec)
		{
			if (ec)
			{
				std::cout << "An error occurred during the connection process, details: " << ec.what() << '\n';

				OnDisconnected();
			}
			else
			{
				std::cout << "Successfully connected to the server.\n";

				ReconnectTries = 0;

				SendCommandAsync();
			}
		});
}

void client::ClientApplication::OnDisconnected()
{
	Sock->close();

	Reconnect();
}

void client::ClientApplication::Reconnect()
{
	ReconnectTries++;
	if(ReconnectTries > Configuration.MaxReconnectTries)
	{
		Stop();
		return;
	}

	std::cout << "Trying to restore the connection " << ReconnectTries << '/' << Configuration.MaxReconnectTries << "...\n";

	Connect();
}

void client::ClientApplication::SendCommandAsync()
{
	const auto command = PrepareRandomCommand();

	async_write(*Sock, buffer(command + delimiter), [&](const boost::system::error_code& ec, size_t count)
		{
			if (ec)
			{
				OnDisconnected();

				// TODO: handle write errors
			}

			ReceiveResultAsync();
		});
}

void client::ClientApplication::ReceiveResultAsync()
{
	async_read_until(*Sock, ReceiveBuffer, delimiter, [=](const boost::system::error_code& ec, size_t count)
		{
			if (ec)
			{
				if (ec == error::eof || ec == error::connection_reset)
				{
					OnDisconnected();
				}

				// TODO: handle other read errors

				return;
			}

			// Parse the message and handle it
			std::istream stream(&ReceiveBuffer);

			std::string messageString;
			std::getline(stream, messageString, '\0');

			OnMessageReceived(messageString);

			ResultsReceived++;
			if(ResultsReceived < Configuration.RequestCount)
			{
				SendCommandAsync();
			}
			else
			{
				Stop();
			}
		});
}

void client::ClientApplication::OnMessageReceived(const std::string& message)
{
	// The standard output is really slow
	// I would prefer to use something else :)
	std::cout << '\n' << message << '\n';
}

std::string client::ClientApplication::PrepareRandomCommand() const
{
	std::uniform_int_distribution<size_t> distribution(0, 99);
	const bool shouldPrepareGetCommand = distribution(Random) != 0;

	if(shouldPrepareGetCommand)
	{
		return PrepareGetCommand();
	}

	return PrepareSetCommand();
}

std::string client::ClientApplication::PrepareGetCommand() const
{
	const auto& keys = Configuration.KeysList;
	// TODO: handle the empty list situation

	std::uniform_int_distribution<size_t> distribution(0, size(keys) - 1);

	const auto randomKey = keys[distribution(Random)];

	return "$get " + randomKey;
}

std::string client::ClientApplication::PrepareSetCommand() const
{
	// TODO: handle the empty lists situation
	
	const auto& keys = Configuration.KeysList;

	std::uniform_int_distribution<size_t> keysDistribution(0, size(keys) - 1);
	const std::string randomKey = keys[keysDistribution(Random)];

	const auto& values = Configuration.ValuesList;

	std::uniform_int_distribution<size_t> valuesDistribution(0, size(values) - 1);
	const std::string randomValue = Configuration.ValuesList[valuesDistribution(Random)];

	return "$set " + randomKey + '=' + randomValue;
}
