#include <vector>
#include <utility>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

struct Graph
{
	int amountNodes;
	std::vector<std::pair<int, int>> edges;
};

struct VertexCover
{
	int amount;
	std::vector<bool> nodeIsCover;

	int getCoverSize()
	{
		int sum = 0;
		for (auto nc : nodeIsCover)
		{
			if (nc)
			{
				sum++;
			}
		}
		return sum;
	}
};

Graph generateGraph(int nodes, int edges)
{
	Graph g;
	g.amountNodes = nodes;
	
	while (g.edges.size() < edges)
	{
		int n1 = rand() % nodes;
		int n2 = rand() % nodes;

		if (n1 != n2)
		{
			if (n1 > n2)
			{
				std::swap(n1, n2);
			}

			auto e = std::make_pair(n1, n2);

			if (std::find(g.edges.begin(), g.edges.end(), e) == g.edges.end())
			{
				g.edges.push_back(e);
			}
		}
	}
	
	return g;
}

bool isVertexCover(Graph g, VertexCover vc)
{
	std::vector<bool> nodeCovered;
	for (int i = 0; i < g.amountNodes; i++)
	{
		nodeCovered.push_back(false);
	}
	for (int i = 0; i < g.amountNodes; i++)
	{
		if (vc.nodeIsCover[i])
		{
			nodeCovered[i] = true;

			for (auto e : g.edges)
			{
				if (i == e.first)
				{
					nodeCovered[e.second] = true;
				}
				if (i == e.second)
				{
					nodeCovered[e.first] = true;
				}
			}
		}
	}
	bool allCovered = true;
	for (int i = 0; i < g.amountNodes; i++)
	{
		if (nodeCovered[i] == false)
		{
			allCovered = false;
		}
	}
	return allCovered;
}

void findAllCovers(Graph& g, VertexCover& vc, std::vector<VertexCover>& allVcs, int& minimum)
{
	if (vc.nodeIsCover.size() == g.amountNodes)
	{
		if (isVertexCover(g, vc))
		{
			int size = vc.getCoverSize();
			if (size == minimum)
			{
				allVcs.push_back(vc);
			}
			if (size < minimum)
			{
				minimum = size;
				allVcs.clear();
				allVcs.push_back(vc);
			}
		}
	}
	else
	{
		auto vc1 = vc;
		vc1.nodeIsCover.push_back(true);
		findAllCovers(g, vc1, allVcs, minimum);

		auto vc2 = vc;
		vc2.nodeIsCover.push_back(false);
		findAllCovers(g, vc2, allVcs, minimum);
	}
}

int main()
{
	int nodes = 7;
	int edges = 10;

	int maxMin = 0;

	std::string filename = "outputFile.csv";
	std::ofstream f(filename, std::ofstream::out);
	if (!f.is_open())
	{
		std::cout << "failed to open " << filename << std::endl;
		std::cin.get();

		return 0;
	}

	for (int i = 0; i < 100000; i++)
	{
		auto g = generateGraph(nodes, edges);

		f << g.amountNodes;
		for (int j = 0; j < g.amountNodes; j++)
		{
			f << "," << g.edges[j].first << "," << g.edges[j].second;
		}

		VertexCover prototype;
		prototype.amount = nodes;

		int minimum = nodes;
		std::vector<VertexCover> allVcs;

		findAllCovers(g, prototype, allVcs, minimum);

		f << "," << minimum;

		for (int j = 0; j < 30; j++)
		{
			for (int k = 0; k < g.amountNodes; k++)
			{
				if (j < allVcs.size())
				{
					f << "," << allVcs[j].nodeIsCover[k]?"1":"0";
				}
				else
				{
					f << "," << "2";
				}
			}
		}

		f << std::endl;

		if (allVcs.size() > maxMin)
		{
			maxMin = allVcs.size();
		}

		std::cout << "Minimum cover: " << minimum << std::endl;

		//std::cout << "Covers found:  " << allVcs.size() << std::endl << std::endl;
	}

	std::cout << "Max Min: " << maxMin << std::endl;

	f.close();

	std::cin.get();

	return 0;
}