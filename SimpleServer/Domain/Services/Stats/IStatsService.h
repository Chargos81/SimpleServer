#pragma once
#include <string>

#include "../../Models/DataEntryStats.h"

namespace domain::services
{
	class IStatsService
	{
	public:

		virtual void Run() = 0;
		virtual void Stop() = 0;

		virtual models::DataEntryStats UpdateReads(const std::string& key) = 0;
		virtual models::DataEntryStats UpdateWrites(const std::string& key) = 0;

		virtual ~IStatsService() = default;
	};
}
