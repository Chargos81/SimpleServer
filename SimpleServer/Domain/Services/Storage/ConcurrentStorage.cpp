#include "pch.h"
#include "ConcurrentStorage.h"
#include "../../Models/DataEntry.h"

domain::services::ConcurrentStorage::ConcurrentStorage(std::filesystem::path storageFilePath):
PersistentStorage(std::move(storageFilePath))
{
}

void domain::services::ConcurrentStorage::Run()
{
	PersistentStorage.Run();

	PopulateCache(PersistentStorage.GetData());
}

void domain::services::ConcurrentStorage::Stop()
{
	PersistentStorage.Stop();
}

void domain::services::ConcurrentStorage::Store(const models::DataEntry& data)
{
	// TODO: it would be better to not write to the cache before we have the data stored on the disc (or here should be a rollback mechanism)
	Cache[data.Key] = data.Value;

	PersistentStorage.Store(data);
}

std::optional<domain::models::DataEntry> domain::services::ConcurrentStorage::Find(const std::string& key)
{
	const auto result = Cache.find(key);
	if (result == end(Cache)) return std::nullopt;

	return models::DataEntry{ result->first, result->second };
}

void domain::services::ConcurrentStorage::PopulateCache(const std::vector<models::DataEntry>& dataContainer)
{
	for (auto&& [key, value] : dataContainer)
	{
		Cache[key] = value;
	}
}
