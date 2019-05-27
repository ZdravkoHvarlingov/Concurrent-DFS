#pragma once

#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
	std::mutex mutex;
	std::condition_variable condition;
	unsigned long count = 0;
	unsigned long currentlyWaiting = 0;

public:
	Semaphore()
	{}
	Semaphore(const Semaphore &other) = delete;
	Semaphore& operator=(const Semaphore &other) = delete;

	void notify(int amount = 1) {
		std::lock_guard<std::mutex> lock(mutex);
		count += amount;

		if (amount == 1)
		{
			condition.notify_one();
		}
		else
		{
			condition.notify_all();
		}
	}

	void wait() {
		std::unique_lock<std::mutex> lock(mutex);
		while (!count) // Handle spurious wake-ups.
		{
			++currentlyWaiting;
			condition.wait(lock);
			--currentlyWaiting;
		}
		--count;
	}

	//once count is equal to the size of the threads => count won't change
	int waiting_count() const
	{
		return currentlyWaiting;
	}
};