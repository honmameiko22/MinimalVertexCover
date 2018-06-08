#include <vector>
#include <utility>
#include <array>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>

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
	for (auto e : g.edges)
	{
		if (!vc.nodeIsCover[e.first] && !vc.nodeIsCover[e.second])
		{
			return false;
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

float benchmark(int amount, int nodes, int edges, bool all=false)
{
	std::vector<Graph> gs;

	for (int i = 0; i < amount; i++)
	{
		gs.push_back(generateGraph(nodes, edges));
	}

	clock_t t = clock();

	for (int i = 0; i < amount; i++)
	{
		VertexCover prototype;
		int minimum = nodes;
		std::vector<VertexCover> allVcs;
		findAllCovers(gs[i], prototype, allVcs, minimum);
	}

	t = clock() - t;
	return ((float)t) / CLOCKS_PER_SEC;
}

void makeDataset(int amount, int nodes, int edges)
{
	int maxMin = 0;
	int fillUp = 75;
	int dropped = 0;

	std::string filename = "RawVertexCover-" + std::to_string(amount) + "-" + std::to_string(nodes) + "-" + std::to_string(edges) + ".csv";
	std::ofstream f(filename, std::ofstream::out);
	if (!f.is_open())
	{
		std::cout << "failed to open " << filename << std::endl;
		std::cin.get();

		return;
	}

	std::cout << amount << "-" << nodes << "-" << edges << ": ";

	for (int i = 0; i < amount; i++)
	{
		auto g = generateGraph(nodes, edges);

		while (!isFullyConnected(g))
		{
			g = generateGraph(nodes, edges);
		}

		

		VertexCover prototype;

		int minimum = nodes;
		std::vector<VertexCover> allVcs;

		findAllCovers(g, prototype, allVcs, minimum);

		if (allVcs.size() <= fillUp)
		{

			f << g.amountNodes;
			for (int j = 0; j < g.edges.size(); j++)
			{
				f << "," << g.edges[j].first << "," << g.edges[j].second;
			}

			f << "," << minimum;

			for (int j = 0; j < fillUp; j++)
			{
				for (int k = 0; k < g.amountNodes; k++)
				{
					if (j < allVcs.size())
					{
						f << "," << allVcs[j].nodeIsCover[k] ? "1" : "0";
					}
					else
					{
						f << "," << "10";
					}
				}
			}

			f << std::endl;
		}
		else
		{
			dropped++;
			i--;
		}

		if (allVcs.size() > maxMin)
		{
			maxMin = allVcs.size();
		}

		if (i % (int)(amount / 50) == 0 && i > 0)
		{
			std::cout << ".";
		}
	}
	std::cout << " Max amount: " << maxMin << " Dropped: " << dropped << std::endl;

	f.close();
}

int main()
{
	makeDataset(10000, 7, 10);
	makeDataset(10000, 9, 14);
	makeDataset(10000, 11, 18);
	makeDataset(10000, 13, 22);

	makeDataset(100000, 7, 10);
	makeDataset(100000, 9, 14);
	makeDataset(100000, 11, 18);
	makeDataset(100000, 13, 22);

	std::cin.get();

	return 0;
}

