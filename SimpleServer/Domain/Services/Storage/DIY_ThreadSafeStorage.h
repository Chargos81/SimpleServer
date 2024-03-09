#pragma once
#include "IStorageService.h"

#include <unordered_map>
#include <shared_mutex>

namespace domain::services
{
	/*
	* Naive approach - just use shared locks for read operations and the exclusive one for write operations
	* Simple and portable, takes 10 minutes to implement
	* Kinda slow even if we are using only one thread to operate and it also has terrible scaling - we are locking all the hash table as a whole
	*/
	class DIY_ThreadSafeStorage: public IStorageService
	{
	public:

		void Store(const models::DataEntry& data) override;
		std::optional<models::DataEntry> Find(const std::string& key) override;

	private:

		std::unordered_map<std::string, std::string> DataContainer;

		std::shared_mutex Mu;
	};
}
