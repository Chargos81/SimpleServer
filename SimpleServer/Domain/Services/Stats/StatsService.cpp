#include "pch.h"
#include "StatsService.h"

#include <iostream>

void domain::services::StatsService::Run()
{
	StatsThread = std::make_unique<std::thread>(&StatsService::PrintStats, this);
}

void domain::services::StatsService::Stop()
{
	if(StatsThread && StatsThread->joinable())
	{
		StatsThread->join();
	}
}

domain::models::DataEntryStats domain::services::StatsService::UpdateReads(const std::string& key)
{
	StatsPerKey[key].Reads++;

	++RecentReads;

	return StatsPerKey[key];
}

domain::models::DataEntryStats domain::services::StatsService::UpdateWrites(const std::string& key)
{
	StatsPerKey[key].Writes++;

	++RecentWrites;

	return StatsPerKey[key];
}

void domain::services::StatsService::PrintStats()
{
	size_t recentReads = 0;
	size_t recentWrites = 0;

	while(true)
	{
		std::this_thread::sleep_for(StatsPrintInterval);

		{
			recentReads = RecentReads;
			recentWrites = RecentWrites;

			RecentReads = 0;
			RecentWrites = 0;
		}

		TotalReads += recentReads;
		TotalWrites += recentWrites;

		std::cout << "******\n";
		std::cout << "Total stats:\n\tReads: " << TotalReads << "\n\tWrites: " << TotalWrites << "\n\0";
		std::cout << "Recent stats (last " << StatsPrintInterval.count() << " seconds):\n\tReads: " << recentReads << "\n\tWrites: " << recentWrites << "\n\0";
	}
}
