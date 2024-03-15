#include "pch.h"
#include "PersistentStorage.h"

#include <fstream>
#include <iostream>

#include "boost/json.hpp"
#include "boost/exception/diagnostic_information.hpp"

using namespace boost::json;

domain::services::storage::PersistentStorage::PersistentStorage(std::filesystem::path filePath) noexcept:
FilePath(std::move(filePath)){}

void domain::services::storage::PersistentStorage::Run()
{
	if (IsWorking) return;

	LoadData();

	IsWorking = true;

	WorkingThread = std::make_unique<std::thread>(&PersistentStorage::ProcessData, this);
}

void domain::services::storage::PersistentStorage::Stop()
{
	if (!IsWorking) return;


	{
		std::lock_guard lock(Mu);
		IsWorking = false;
		CV.notify_one();
	}

	if(WorkingThread && WorkingThread->joinable())
	{
		WorkingThread->join();
	}
}

void domain::services::storage::PersistentStorage::Store(const models::DataEntry& data)
{
	std::lock_guard lock(Mu);
	PendingData.push(data);
	CV.notify_one();
}

std::vector<domain::models::DataEntry> domain::services::storage::PersistentStorage::GetData()
{
	// Looks terrible and here are a lot of allocations under the mutex but we expect it to run only on startup
	std::lock_guard lock(Mu);
	
	std::vector<models::DataEntry> outData;

	auto& arr = Data.as_object();
	outData.resize(std::size(arr));
	std::transform(std::begin(arr), std::end(arr), std::begin(outData), [](const auto& keyAndValue)
	{
		const auto key = keyAndValue.key();
		const auto value = keyAndValue.value().as_string();

		return models::DataEntry{ key, {std::begin(value), std::end(value)}};
	});

	return outData;
}

void domain::services::storage::PersistentStorage::ProcessData()
{
	while (IsWorking)
	{
		models::DataEntry data;

		{
			std::unique_lock lock(Mu);

			CV.wait(lock, [&] {return !IsWorking || !PendingData.empty(); });

			if (PendingData.empty()) continue;

			data = PendingData.front();
			PendingData.pop();
		}

		// Assuming that the write operations are quite seldom
		// TODO: handle the situation when we have several pending data updates
		try
		{
			std::fstream fs;
			fs.open(FilePath, std::fstream::out);

			// Find the data entry in the json and update it
			auto& obj = Data.as_object();
			obj[data.Key] =  data.Value;

			const auto jsonString = serialize(Data);
			fs << jsonString;

			fs.flush();
			fs.close();
		}
		catch (...)
		{
			// TODO: handle the writing exceptions
		}
	}
}

void domain::services::storage::PersistentStorage::LoadData()
{
	try
	{
		if(!exists(FilePath))
		{
			std::cout << "File does not exist";
		}

		// Assuming that the write operations are quite seldom
		std::fstream fs;
		fs.open(FilePath, std::fstream::in);

		// Load the data if any
		if (fs.peek() != std::ifstream::traits_type::eof())
		{
			Data = parse(fs);
		}
		else
		{
			Data = object();
		}

		fs.close();
	}
	catch (const boost::system::system_error& e)
	{
		// TODO: handle the json parsing and file opening errors
		std::cout << "The data file provided is an invalid json. Please, provide the correct one.\n";

		throw(e);
	}
}
