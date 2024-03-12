#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

namespace server
{
	class WorkerPool
	{
	public:

		void AddJob(std::function<void()> job);

		void Start(size_t workers);
		void Stop();

	private:

		void WorkCycle();

	private:

		std::vector<std::thread> Workers;
		std::atomic<bool> IsStopRequested = false;

		std::queue < std::function<void()>> JobQueue;
		std::condition_variable CV;
		std::mutex Mu;
	};
	
}
