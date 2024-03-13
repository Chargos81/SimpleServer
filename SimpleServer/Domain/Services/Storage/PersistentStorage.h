#pragma once
#include <filesystem>
#include <queue>
#include <thread>
#include <vector>

#include "boost/json.hpp"
#include "../../Models/DataEntry.h"

namespace domain::services::storage
{
	/**
	 * Represents a service that writes our data to the file
	 */
	class PersistentStorage
	{
	public:

		explicit PersistentStorage(std::filesystem::path filePath) noexcept;

		void Run();
		void Stop();

		void Store(const models::DataEntry& data);
		std::vector<models::DataEntry> GetData();

	private:

		void ProcessData();
		void LoadData();

	private:

		std::queue<models::DataEntry> PendingData;
		std::mutex Mu;
		std::condition_variable CV;
		std::unique_ptr<std::thread> WorkingThread;

		std::filesystem::path FilePath;
		boost::json::value Data;
		bool IsWorking = false;
	};
}
