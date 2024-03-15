// Client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>

#include "ClientApplication.h"

int main()
{
    const auto start = std::chrono::steady_clock::now();

    auto config = client::config::ClientConfiguration();

    config.KeysList = { "tree", "house", "cat", "car", "chair" };
    config.ValuesList = { "blue", "red", "yellow", "green", "magenta", "white", "black" };
    config.ServerIpAddress = "127.0.0.1";
    config.ServerPort = 7777;
    config.RequestCount = 10'000;


    auto client = client::ClientApplication(config);

    client.Run();


    const auto end = std::chrono::steady_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Time passed: " << duration << "ms\n";

    int x;

    std::cin >> x;
}
