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
	std::vector<bool> nodeIsCover;
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
	for (int i = 0; i < g.amountNodes; i++)
	{
		if (!vc.nodeIsCover[i])
		{
			bool found = false;

			for (auto e : g.edges)
			{
				if (i == e.first)
				{
					if (vc.nodeIsCover[e.second])
					{
						found = true;
					}
				}
				if (i == e.second)
				{
					if (vc.nodeIsCover[e.first])
					{
						found = true;
					}
				}
			}

			if (!found)
			{
				return false;
			}
		}
	}
	return true;
}

bool isFullyConnected(const Graph& g)
{
	std::vector<int> explored;

	std::vector<int> toExplore;
	toExplore.push_back(0);

	while (toExplore.size() > 0)
	{
		int i = toExplore.back();
		toExplore.pop_back();
		explored.push_back(i);

		for (auto e : g.edges)
		{
			if (i == e.first)
			{
				int other = e.second;
				if (std::find(explored.begin(), explored.end(), other) == explored.end())
				{
					if (std::find(toExplore.begin(), toExplore.end(), other) == toExplore.end())
					{
						toExplore.push_back(other);
					}
				}
			}
			else if (i == e.second)
			{
				int other = e.first;
				if (std::find(explored.begin(), explored.end(), other) == explored.end())
				{
					if (std::find(toExplore.begin(), toExplore.end(), other) == toExplore.end())
					{
						toExplore.push_back(other);
					}
				}
			}
		}
	}

	return explored.size() == g.amountNodes;
}

void findAllCovers(Graph& g, VertexCover& vc, std::vector<VertexCover>& allVcs, int& minimum, int size=0)
{
	if (size > minimum)
	{
		return;
	}
	if (vc.nodeIsCover.size() == g.amountNodes)
	{
		if (isVertexCover(g, vc))
		{
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
		vc1.nodeIsCover.push_back(false);
		findAllCovers(g, vc1, allVcs, minimum, size);

		auto vc2 = vc;
		vc2.nodeIsCover.push_back(true);
		findAllCovers(g, vc2, allVcs, minimum, size + 1);
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

	for (int i = 0; i < 1000000; i++)
	{
		auto g = generateGraph(nodes, edges);

		while (!isFullyConnected(g))
		{
			g = generateGraph(nodes, edges);
		}

		f << g.amountNodes;
		for (int j = 0; j < g.edges.size(); j++)
		{
			f << "," << g.edges[j].first << "," << g.edges[j].second;
		}

		VertexCover prototype;

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
					f << "," << "10";
				}
			}
		}

		f << std::endl;

		if (allVcs.size() > maxMin)
		{
			maxMin = allVcs.size();
		}

		if (i % 100 == 0 && i > 0)
		{
			std::cout << ".";
		}
		if (i % 10000 == 0 && i > 0)
		{
			std::cout << i << std::endl;
		}


		//std::cout << "Minimum cover: " << minimum << std::endl;

		//std::cout << "Covers found:  " << allVcs.size() << std::endl << std::endl;
	}

	std::cout << "Max Min: " << maxMin << std::endl;

	f.close();

	std::cin.get();

	return 0;
}