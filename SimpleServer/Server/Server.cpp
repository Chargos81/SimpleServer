// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ServerApplication.h"
#include "../Domain/Services/Storage/ConcurrentStorage.h"
#include "Network/AsioNetworkManager.h"

int main()
{
	auto storage = std::make_unique<domain::services::ConcurrentStorage>();
	auto networkManager = std::make_unique<server::network::AsioNetworkManager>();

	auto server = server::ServerApplication(std::move(storage), std::move(networkManager));

	server.Run();

	return 0;
}

