#pragma once
#include <memory>
#include <optional>
#include <string>

#include "../Models/DataEntry.h"
#include "../Services/Storage/IStorageService.h"


namespace domain::application
{
	class ApplicationContext
	{
	public:

		explicit ApplicationContext(std::unique_ptr<services::IStorageService> storageService) noexcept;

		void StoreData(const models::DataEntry& data) const;
		std::optional<models::DataEntry> FindData(const std::string& key) const;


	private:

		std::unique_ptr<services::IStorageService> StorageService;
	};
}

