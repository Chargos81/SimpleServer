#include "pch.h"
#include "ConcurrentStorage.h"
#include "../../Models/DataEntry.h"

void domain::services::ConcurrentStorage::Store(const models::DataEntry& data)
{
}

std::optional<domain::models::DataEntry> domain::services::ConcurrentStorage::Find(const std::string& key)
{
	const auto result = DataContainer.find(key);
	if (result == end(DataContainer)) return std::nullopt;

	return models::DataEntry{ result->first, result->second };

}
