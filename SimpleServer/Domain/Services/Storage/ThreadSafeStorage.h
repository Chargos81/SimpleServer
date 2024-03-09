#pragma once
#include <unordered_map>

#include "IStorageService.h"

namespace domain::services
{
	class ThreadSafeStorage: public IStorageService
	{
	public:

		void Store(const models::DataEntry& data) override;
		std::optional<models::DataEntry> Find(const std::string& key) override;

	private:

		std::unordered_map<std::string, std::string> DataContainer;
	};
}
