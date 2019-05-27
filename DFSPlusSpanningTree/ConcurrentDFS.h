#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "Semaphore.h"
#include "Pair.h"
#include <stack>
#include <iostream>

namespace dfs
{
	int visitedCount, visitedLocksCount;

	std::mutex stackMutex;
	std::mutex finalMutex;
	std::mutex writeMutex;
	std::mutex resultMutex;
	std::mutex visitedLocks[128];

	Semaphore stackCountSem;
	std::condition_variable finalEvent;

	std::stack<Pair> st;

	Pair concurrent_pop()
	{
		std::lock_guard<std::mutex> lockGuard(stackMutex);
		Pair top = st.top();
		st.pop();

		return top;
	}

	void concurrent_push(Pair pair)
	{
		std::lock_guard<std::mutex> lockGuard(stackMutex);
		st.push(pair);
	}

	void DFS_concurrent_visit(std::vector<std::vector<bool>> &adjMatrix, std::vector<bool> &visited, std::vector<int> &parent, std::vector<int> &res, int n, bool quiet)
	{
		if (!quiet)
		{
			std::unique_lock<std::mutex> writeLock(writeMutex);
			std::cout << "Thread " << std::this_thread::get_id() << " started working!" << std::endl;
		}
		
		auto begin = std::chrono::steady_clock::now();

		while (true)
		{
			//Waiting for an element in the stack
			stackCountSem.wait();
			//If all are visited
			if (visitedCount == n)
			{
				break;
			}
			Pair crr = concurrent_pop();

			int lockInd = crr.node % visitedLocksCount;
			bool willVisit = false;
			std::unique_lock<std::mutex> lock(visitedLocks[lockInd]);
			if (!visited[crr.node])
			{
				willVisit = true;
				visited[crr.node] = true;
				parent[crr.node] = crr.parent;
			}
			lock.unlock();

			if (willVisit)
			{
				std::unique_lock<std::mutex> resLock(resultMutex);
				res.push_back(crr.node);
				resLock.unlock();

				for (int i = n - 1; i >= 0; --i)
				{
					if (adjMatrix[crr.node][i])
					{
						concurrent_push({ i, crr.node });

						//Maybe count variable and notify when all done(at the end of the loop)
						stackCountSem.notify(1);
					}
				}

				/*if (visitedCount == n)
				{
					finalEvent.notify_one();
				}*/
			}
		}

		auto end = std::chrono::steady_clock::now();
		double elapsed_secs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

		if (!quiet)
		{
			std::unique_lock<std::mutex> writeLock(writeMutex);
			std::cout << "Thread " << std::this_thread::get_id() << " stopped working! Execution time in ms: " << elapsed_secs << std::endl;
		}
	}

	void DFS_concurrent(std::vector<std::vector<bool>> &adjMatrix, std::vector<bool> &visited, std::vector<int> &parent, std::vector<int> &res, int n, int numberOfThreads, bool quiet)
	{
		visited.clear();
		visited.assign(n, false);
		parent.clear();
		parent.assign(n, -1);
		
		visitedCount = 0;
		visitedLocksCount = numberOfThreads * 2;

		stackCountSem.set_number_of_threads(numberOfThreads);
		std::vector<std::thread> threads;
		for (size_t i = 0; i < numberOfThreads; i++)
		{
			threads.push_back(std::thread(DFS_concurrent_visit, std::ref(adjMatrix), std::ref(visited), std::ref(parent), std::ref(res), n, quiet));
		}

		for (int i = 0; i < n; i++)
		{
			if (!visited[i])
			{
				st.push({ i, -1 });
				stackCountSem.notify();

				stackCountSem.wait_for_all_to_sleep();
			}
		}
		visitedCount = n;
		stackCountSem.notify(numberOfThreads);

		for (size_t i = 0; i < numberOfThreads; i++)
		{
			threads[i].join();
		}
	}
}
