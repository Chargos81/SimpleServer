// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ServerApplication.h"
#include "../Domain/Services/Storage/ConcurrentStorage.h"
#include "Network/ServerNetworkManager.h"
#include "Services/Stats/StatsService.h"

int main(int argc, char** argv)
{
	auto storageFilePath = std::filesystem::path(argv[0]).parent_path() / "Data\\config.txt";

	auto storage = std::make_unique<domain::services::ConcurrentStorage>(storageFilePath);
	auto networkManager = std::make_unique<server::network::ServerNetworkManager>();
	auto stats = std::make_unique<domain::services::StatsService>();

	auto server = server::ServerApplication(std::move(storage), std::move(networkManager), std::move(stats));

	server.Run();

	return EXIT_SUCCESS;
}

