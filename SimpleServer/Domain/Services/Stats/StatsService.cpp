#include "pch.h"
#include "StatsService.h"

#include <iostream>

void domain::services::StatsService::Run()
{
	StatsThread = std::make_unique<std::thread>(&StatsService::PrintStats, this);
}

void domain::services::StatsService::Stop()
{
	{
		std::lock_guard lock(Mu);
		IsStopRequested = true;
		CV.notify_one();
	}

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

	while(!IsStopRequested)
	{
		std::unique_lock lock(Mu);

		CV.wait_for(lock, StatsPrintInterval, [&] {return ShouldPrintStats(); });

		LastStatsPrintTimestamp = std::chrono::steady_clock::now();


		const size_t recentReads = RecentReads;
		const size_t recentWrites = RecentWrites;

		RecentReads = 0;
		RecentWrites = 0;

		TotalReads += recentReads;
		TotalWrites += recentWrites;

		std::cout << "******\n";
		std::cout << "Total stats:\n\tReads: " << TotalReads << "\n\tWrites: " << TotalWrites << "\n";
		std::cout << "Recent stats (last " << StatsPrintInterval.count() << " seconds):\n\tReads: " << recentReads << "\n\tWrites: " << recentWrites << "\n";
	}
}

bool domain::services::StatsService::ShouldPrintStats() const
{
	const auto now = std::chrono::steady_clock::now();

	return std::chrono::duration_cast<std::chrono::seconds>(now - LastStatsPrintTimestamp) >= StatsPrintInterval;
}
