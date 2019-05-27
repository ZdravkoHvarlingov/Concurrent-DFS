#pragma once
#include <stack>
#include <vector>
#include "Pair.h"

namespace dfs
{
	void DFS_serial(std::vector<std::vector<bool>> &adjMatrix, std::vector<bool> &visited, std::vector<int> &parent, std::vector<int> &res, int n)
	{
		visited.clear();
		res.clear();
		parent.clear();

		std::stack<Pair> dfsStack;
		for (int i = 0; i < n; i++)
		{
			dfsStack.push({ i, -1 });
			visited.push_back(false);
			parent.push_back(-1);
		}

		while (!dfsStack.empty())
		{
			Pair crr = dfsStack.top(); dfsStack.pop();
			if (!visited[crr.node])
			{
				visited[crr.node] = true;
				parent[crr.node] = crr.parent;
				res.push_back(crr.node);

				for (int i = 0; i < n; i++)
				{
					if (adjMatrix[crr.node][i])
					{
						dfsStack.push({ i, crr.node });
					}
				}
			}
		}
	}
}