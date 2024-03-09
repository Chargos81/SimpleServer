#pragma once
#include "IStorageService.h"

#include <optional>
#include <concurrent_unordered_map.h>

namespace domain::services
{
	/**
	 * The microsoft hashmap implementation is used, it performs well but as far as i remember it's MSVC specific thing :(
	 */
	class ConcurrentStorage : public IStorageService
	{
	public:

		void Store(const models::DataEntry& data) override;
		std::optional<models::DataEntry> Find(const std::string& key) override;

	private:
		
		Concurrency::concurrent_unordered_map<std::string, std::string> DataContainer;
	};

}
