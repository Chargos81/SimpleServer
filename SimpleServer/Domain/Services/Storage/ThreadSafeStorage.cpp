#include "pch.h"
#include "ThreadSafeStorage.h"
#include "../../Models/DataEntry.h"

void domain::services::ThreadSafeStorage::Store(const models::DataEntry& data)
{
	if (data.Key.empty()) return;

	DataContainer[data.Key] = data.Value;
}

std::optional<domain::models::DataEntry> domain::services::ThreadSafeStorage::Find(const std::string& key)
{
	if (key.empty()) return std::nullopt;

	const auto result = DataContainer.find(key);
	if (result == end(DataContainer)) return std::nullopt;

	return { {result->first, result->second } };
}
