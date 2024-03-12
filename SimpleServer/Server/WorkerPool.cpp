#include "WorkerPool.h"

#include <cassert>
#include <shared_mutex>

void server::WorkerPool::AddJob(std::function<void()> job)
{
	{
		std::unique_lock lock(Mu);
		JobQueue.push(job);
	}

	CV.notify_one();
}

void server::WorkerPool::Start(size_t workers)
{
	assert(workers > 0);

	for(size_t i = 0; i < workers; i++)
	{
		Workers.emplace_back(&WorkerPool::WorkCycle, this);
	}
}

void server::WorkerPool::Stop()
{
	IsStopRequested.store(true);

	CV.notify_all();

	for (auto& worker : Workers)
	{
		worker.join();
	}
}

void server::WorkerPool::WorkCycle()
{
	while(!IsStopRequested.load() == true)
	{
		std::function<void()> job;
		{
			std::unique_lock lock(Mu);
			while(JobQueue.empty())
			{
				CV.wait(lock, [&] {return !JobQueue.empty(); });
			}

			job = JobQueue.front();
			JobQueue.pop();
		}

		job();
	}
}
