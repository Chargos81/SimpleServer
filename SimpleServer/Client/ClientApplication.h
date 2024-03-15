#pragma once
#include <memory>
#include <random>

#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "Config/ClientConfiguration.h"

namespace client
{
	/**
	 * TODO: deadline timers and "alive" messages
	 */
	class ClientApplication
	{
	public:

		explicit ClientApplication(config::ClientConfiguration configuration) noexcept;

		void Run();
		void Stop();

	private:

		void Connect();

		void OnDisconnected();
		void Reconnect();

		void SendCommandAsync();

		void ReceiveResultAsync();
		void OnMessageReceived(const std::string& message);

		std::string PrepareRandomCommand() const;
		std::string PrepareGetCommand() const;
		std::string PrepareSetCommand() const;

	private:

		/**
		 * Used to generate random commands
		 */
		static inline std::mt19937 Random = std::mt19937(std::random_device()());

		config::ClientConfiguration Configuration;

		boost::asio::io_context IOContext;
		std::unique_ptr<boost::asio::ip::tcp::socket> Sock;
		boost::asio::streambuf ReceiveBuffer;

		size_t ResultsReceived = 0;
		size_t ReconnectTries = 0;
	};
}

