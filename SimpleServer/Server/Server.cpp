// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <iostream>

#include "../Domain/Application/ApplicationContext.h"
#include "../Domain/Services/Storage/DIY_ThreadSafeStorage.h"
#include "../Domain/Services/Storage/ConcurrentStorage.h"

int main()
{
	auto storage = std::make_unique<domain::services::ConcurrentStorage>();
	const auto app = domain::application::ApplicationContext(std::move(storage));

	constexpr size_t iterations = 100'000'000;
	auto startTime = std::chrono::steady_clock::now();

	for(size_t i = 0; i < iterations; i++)
	{
		auto data = app.FindData("tree");
	}

	std::vector<std::thread> th;

	for(int i = 0; i < 5; i++)
	{
		th.emplace_back([&] {
			for (size_t j = 0; j < iterations; j++)
		{
			auto data = app.FindData("tree");
		}});
	}

	for (auto& thread : th)
	{
		thread.join();
	}

	auto now = std::chrono::steady_clock::now();
	auto passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

	std::cout << "Time to perform " << iterations << " iterations: " << passed << "ms\n";


	int x;
	std::cin >> x;

	return 0;
}

