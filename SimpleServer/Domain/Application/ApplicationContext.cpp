#include "pch.h"
#include "ApplicationContext.h"

#include "../Services/Storage/IStorageService.h"

domain::application::ApplicationContext::ApplicationContext(std::unique_ptr<services::IStorageService> storageService) noexcept:
StorageService(std::move(storageService))
{
}

void domain::application::ApplicationContext::StoreData(const models::DataEntry& data) const
{
	StorageService->Store(data);
}

std::optional<domain::models::DataEntry> domain::application::ApplicationContext::FindData(const std::string& key) const
{
	return StorageService->Find(key);
}
