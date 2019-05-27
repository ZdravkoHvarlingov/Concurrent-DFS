#include <iostream>
#include "ConcurrentDFS.h"
#include "SerialDFS.h"
#include "ParseArguments.h"
#include <string>
#include <fstream>

void GenerateRandomGraph(int n, int edges, std::vector<std::vector<bool>> &adjMatrix)
{
	adjMatrix.clear();

	adjMatrix.assign(n, std::vector<bool>());
	for (size_t i = 0; i < n; i++)
	{
		adjMatrix[i].assign(n, false);
	}

	srand(time(NULL));

	for (size_t i = 0; i < edges; i++)
	{
		int src = rand() % n;
		int dst = rand() % n;
		adjMatrix[src][dst] = true;
	}
}

void ReadGraph(std::vector<std::vector<bool>> &adjMatrix, std::string &inName, int &n)
{
	std::ifstream in(inName);
	in >> n;
	for (int i = 0; i < n; i++)
	{
		adjMatrix.push_back(std::vector<bool>());
		for (int j = 0; j < n; j++)
		{
			bool hasEdge;
			in >> hasEdge;

			adjMatrix[i].push_back(hasEdge);
		}
	}

	in.close();
}

int main(int argc, char *argv[])
{
	//number of threads including the controling main thread
	int numberOfThreads = 1;
	int n = 1000;
	int edges = 1000;
	bool quiet = false, inFile = false, outFile = false;
	std::string inName = "", outName = "";
	
	ParseArgs(argc, argv, quiet, numberOfThreads, inFile, inName, outFile, outName, n, edges);

	std::vector<std::vector<bool>> adjMatrix;
	std::vector<bool> visited;
	std::vector<int> res;
	std::vector<int> parent;

	if (!quiet)
	{
		std::cout << "Main thread " << std::this_thread::get_id() << " started working!" << std::endl;
	}

	if (inFile)
	{
		ReadGraph(adjMatrix, inName, n);
	}
	else
	{
		GenerateRandomGraph(n, edges, adjMatrix);
	}
	
	//Actual DFS execution
	auto begin = std::chrono::steady_clock::now();
	if (numberOfThreads == 1)
	{
		dfs::DFS_serial(adjMatrix, visited, parent, res, n);
	}
	else
	{
		//Number of threads without the main thread
		dfs::DFS_concurrent(adjMatrix, visited, parent, res, n, numberOfThreads - 1, quiet);
	}
	auto end = std::chrono::steady_clock::now();
	//Actual DFS execution

	if (outFile)
	{
		std::ofstream out(outName);
		for (int i = 0; i < n; i++)
		{
			if (parent[i] != -1)
			{
				out << "(" << parent[i] << ", " << i << ")" << std::endl;
			}
		}

		out.close();
	}

	double elapsed_secs = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	if (!quiet)
	{
		std::cout << "Main thread " << std::this_thread::get_id() << " stopped working! DFS execution time: " << elapsed_secs << std::endl;
	}
	
	return 0;
}

