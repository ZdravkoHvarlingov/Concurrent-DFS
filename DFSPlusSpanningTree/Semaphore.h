#pragma once

#include <condition_variable>
#include <mutex>

class Semaphore
{
private:
	std::mutex mutex;
	std::condition_variable condition;
	std::mutex allSleepMutex;
	std::condition_variable allSleep;
	bool notifyAllSleep = true;
	unsigned long count = 0;
	unsigned long currentlyWaiting = 0;
	unsigned long numberOfThreads = 0;

public:
	Semaphore()
	{}
	Semaphore(const Semaphore &other) = delete;
	Semaphore& operator=(const Semaphore &other) = delete;

	void notify(int amount = 1) {
		std::lock_guard<std::mutex> lock(mutex);
		count += amount;
		notifyAllSleep = false;

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
			if (currentlyWaiting == numberOfThreads)
			{
				notifyAllSleep = true;
				allSleep.notify_all();
			}
			condition.wait(lock);
			--currentlyWaiting;
		}
		--count;
	}

	void set_number_of_threads(int numberOfThreads)
	{
		this->numberOfThreads = numberOfThreads;
	}

	void wait_for_all_to_sleep()
	{
		std::unique_lock<std::mutex> allSleepLock(allSleepMutex);
		while (currentlyWaiting != numberOfThreads || !notifyAllSleep)
		{
			allSleep.wait(allSleepLock);
		}
	}

	//once count is equal to the size of the threads => count won't change
	int waiting_count() const
	{
		return currentlyWaiting;
	}
};