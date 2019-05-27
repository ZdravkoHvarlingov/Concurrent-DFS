#pragma once
#include <string>
#include <string.h>

void ParseArgs(int argc, char *argv[], bool &quiet, int &numberOfThreads, bool &inFile, std::string &inName, bool &outFile, std::string &outName, int &n, int &numberOfEdges)
{
	n = 1000;
	numberOfThreads = 8;
	numberOfEdges = -1;
	inFile = false;
	quiet = false;
	outFile = false;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-n") == 0)
		{
			n = std::stoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-q") == 0)
		{
			quiet = true;
		}
		else if (strcmp(argv[i], "-i") == 0)
		{
			inFile = true;
			inName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			outFile = true;
			outName = argv[i + 1];
		}
		else if (strcmp(argv[i], "-t") == 0)
		{
			numberOfThreads = std::stoi(argv[i + 1]);
		}
		else if (strcmp(argv[i], "-e") == 0)
		{
			numberOfEdges = std::stoi(argv[i + 1]);
		}
	}

	if (numberOfEdges == -1)
	{
		numberOfEdges = n * 10;
	}
}
