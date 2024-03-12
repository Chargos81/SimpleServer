#pragma once

#include <variant>
#include <condition_variable>
#include <queue>

#include "WorkerPool.h"
#include "Network/GetCommand.h"
#include "Network/SetCommand.h"
#include "Network/CommandResult.h"
#include "Network/INetworkManager.h"
#include "Services/Storage/IStorageService.h"


namespace server
{
	/**
	 * A coordination point of the system
	 * Also handles message exchange between the components
	 * In more complicated solutions we should use dynamic message types or the type erasure idiom but this one is quite simple so i'll go with std::variant
	 */
	class ServerApplication
	{
		using ApplicationMessage = std::variant<network::GetCommand, network::CommandResult, network::SetCommand>;

	public:

		explicit ServerApplication(std::unique_ptr<domain::services::IStorageService> storage, std::unique_ptr<network::INetworkManager> networkManager) noexcept;

		void Run();
		void Stop();

		/**
		 * An entry point for incoming messages
		 */
		void ReceiveMessage(const ApplicationMessage& msg);

	private:

		/**
		 * The std::variant approach went as terrible as i expected...
		 */
		struct ApplicationMessageVisitor
		{
		public:

			ApplicationMessageVisitor(ServerApplication* app):
			App(app){}

			void operator()(const network::SetCommand& cmd) const
			{
				App->ProcessSetCommand(cmd);
			}

			void operator()(const network::GetCommand& cmd) const
			{
				App->ProcessGetCommand(cmd);
			}

			void operator()(const network::CommandResult& cmd) const
			{
				App->ProcessCommandResult(cmd);
			}

		private:

			ServerApplication* App = nullptr;
		};

		void ProcessCommandResult(const network::CommandResult& result);
		void ProcessGetCommand(const network::GetCommand& command);
		void ProcessSetCommand(const network::SetCommand& command);

	private:

		std::unique_ptr<domain::services::IStorageService> Storage;
		std::unique_ptr<network::INetworkManager> NetworkManager;

		WorkerPool WorkerPool;

		/**
		 * A simple blocking queue
		 */
		std::queue<ApplicationMessage> Messages;
		std::condition_variable CV;
		std::mutex Mu;

		bool IsStopRequested = false;
	}; 
}

