#pragma once
#include "IStatsService.h"

#include <concurrent_unordered_map.h>
#include <thread>

namespace domain::services
{
	class StatsService : public IStatsService
	{
	public:

		void Run() override;
		void Stop() override;

		models::DataEntryStats UpdateReads(const std::string& key) override;
		models::DataEntryStats UpdateWrites(const std::string& key) override;

	private:

		void PrintStats();

	private:

		Concurrency::concurrent_unordered_map<std::string, models::DataEntryStats> StatsPerKey;

		std::atomic<size_t> RecentReads;
		std::atomic<size_t> RecentWrites;

		size_t TotalReads = 0;
		size_t TotalWrites = 0;

		std::unique_ptr<std::thread> StatsThread;

		std::chrono::seconds StatsPrintInterval {5};
	};
}
