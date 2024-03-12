#pragma once

#include <memory>

#include "Network/GetCommand.h"
#include "Network/SetCommand.h"
#include "Network/CommandResult.h"
#include "Network/INetworkManager.h"
#include "Network/NetworkDefines.h"
#include "Services/Storage/IStorageService.h"


namespace server
{
	/**
	 * A coordination point of the system
	 * Handles the command processing
	 * TODO: data processing logic should be a part of the domain, but i put it for the sake of the simplicity
	 */
	class ServerApplication
	{
	public:

		explicit ServerApplication(std::unique_ptr<domain::services::IStorageService> storage, std::unique_ptr<network::INetworkManager> networkManager) noexcept;

		void Run();
		void Stop();

		void ProcessGetCommand(const network::GetCommand& command);
		void ProcessSetCommand(const network::SetCommand& command);

	private:

		/**
		 * Called when the operation is completed
		 * Process stats etc. and send the reply back to the callee
		 */
		void ProcessCommandResult(network::ConnectionId connectionId, const network::CommandResult& result);

	private:

		std::unique_ptr<domain::services::IStorageService> Storage;
		std::unique_ptr<network::INetworkManager> NetworkManager;

		bool IsStopRequested = false;
	}; 
}

