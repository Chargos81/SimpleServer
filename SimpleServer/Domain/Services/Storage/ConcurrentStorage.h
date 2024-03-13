#pragma once
#include "IStorageService.h"

#include <optional>
#include <concurrent_unordered_map.h>

#include "PersistentStorage.h"

namespace domain::services
{
	/**
	 * The microsoft hashmap implementation is used, it performs well but as far as i remember it's MSVC specific thing :(
	 */
	class ConcurrentStorage : public IStorageService
	{
	public:

		ConcurrentStorage(std::filesystem::path storageFilePath);

		void Run() override;
		void Stop() override;

		void Store(const models::DataEntry& data) override;
		std::optional<models::DataEntry> Find(const std::string& key) override;

	private:

		void PopulateCache(const std::vector<models::DataEntry>& dataContainer);

	private:

		/**
		 * Represents the cache for fast read access
		 */
		Concurrency::concurrent_unordered_map<std::string, std::string> Cache;

		/**
		 * Represents the long term storage
		 */
		storage::PersistentStorage PersistentStorage;
	};

}
