#pragma once

namespace domain
{
	namespace models
	{
		class DataEntry;
	}
}

namespace domain::services
{

	class IStorageService
	{
	public:

		virtual void Store(const models::DataEntry& data) = 0;
		virtual std::optional<models::DataEntry> Find(const std::string& key) = 0;

		virtual ~IStorageService() = default;
	};
}
