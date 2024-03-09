// Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <chrono>
#include <iostream>

#include "../Domain/Application/ApplicationContext.h"
#include "../Domain/Services/Storage/ThreadSafeStorage.h"

int main()
{
	auto storage = std::make_unique<domain::services::ThreadSafeStorage>();
	const auto app = domain::application::ApplicationContext(std::move(storage));

	constexpr size_t iterations = 100'000'000;
	auto startTime = std::chrono::steady_clock::now();

	for(size_t i = 0; i < iterations; i++)
	{
		auto data = app.FindData("tree");
	}

	auto now = std::chrono::steady_clock::now();
	auto passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count();

	std::cout << "Time to perform " << iterations << " iterations: " << passed << "ms\n";


	int x;
	std::cin >> x;
	return 0;
}

