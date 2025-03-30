#include <iostream>
#include <fstream>
#include <time.h>
#include <ctime>
#include <string>

template <typename T>
struct DynamicArray {
	T* data;
	int size;
	int capacity;

	DynamicArray(int initialCapacity = 10) : size(0), capacity(initialCapacity) {
		data = new T[capacity];
	}

	void push_back(const T& value) {
		if (size == capacity) {
			capacity *= 2;
			T* newData = new T[capacity];
			for (int i = 0; i < size; ++i) {
				newData[i] = data[i];
			}
			delete[] data;
			data = newData;
		}
		data[size++] = value;
	}

	~DynamicArray() {
		delete[] data;
	}

	T& operator[](int index) {
		if (index < 0 || index >= size) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}

	const T& operator[](int index) const {
		if (index < 0 || index >= size) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}

	void quicksort(int left, int right) {
		if (left < right) {
			int pivotIndex = partition(left, right);
			quicksort(left, pivotIndex - 1);
			quicksort(pivotIndex + 1, right);
		}
	}

	int partition(int left, int right) {
		T pivot = data[right];
		int i = left - 1;
		for (int j = left; j < right; ++j) {
			if (data[j] < pivot) {
				i++;
				T temp = data[i];
				data[i] = data[j];
				data[j] = temp;
			}
		}
		T temp = data[i + 1];
		data[i + 1] = data[right];
		data[right] = temp;
		return i + 1;
	}
};

struct Node {
	int id;
	double x, y;
};

struct Edge {
	int id1, id2;
	double weight;

	bool operator<(const Edge& other) const {
		return weight < other.weight;
	}
};

struct Graph {
	int nodes;
	DynamicArray<Node> nodeList;
	DynamicArray<Edge> edges;
};

int findCnt = 0;

class UnionFind {
private:
	DynamicArray<int> parentsID;
	DynamicArray<int> ranks;

public:
	UnionFind(int n)
		: parentsID(n), ranks(n) {
		for (int i = 0; i < n; i++) {
			parentsID.push_back(i);
			ranks.push_back(0);
		}
	}

	int find(int x, bool pathCompression) {
	
		if (pathCompression) {
			if (parentsID[x] != x) {
				parentsID[x] = find(parentsID[x], pathCompression);
				findCnt++;
			}
			return parentsID[x];
		}
		else {
			while (parentsID[x] != x) {
				x = parentsID[x];
				findCnt++;
			}
			return x;
		}
	}

	void unite(int x, int y, bool unionByranks, bool pathCompression) {
		int reprX = x;
		int reprY = y;

		if (reprX != reprY) {
			if (unionByranks) {
				if (ranks[reprX] < ranks[reprY]) {
					parentsID[reprX] = reprY;
				}
				else if (ranks[reprX] > ranks[reprY]) {
					parentsID[reprY] = reprX;
				}
				else {
					parentsID[reprY] = reprX;
					ranks[reprX]++;
				}
			}
			else {
				parentsID[reprX] = reprY;
			}
		}
	}
};

Graph readFile(const std::string& filename) {
	Graph graph;
	std::ifstream inputStream(filename);

	inputStream >> graph.nodes;
	for (int i = 0; i < graph.nodes; i++) {
		Node node;
		node.id = i;
		inputStream >> node.x >> node.y;
		graph.nodeList.push_back(node);
	}

	int edgeCNT;
	inputStream >> edgeCNT;
	for (int i = 0; i < edgeCNT; i++) {
		Edge edge;
		inputStream >> edge.id1 >> edge.id2 >> edge.weight;
		graph.edges.push_back(edge);
	}

	return graph;
}
clock_t t1, t2, t4, t5;
double t3, t6;
int kruskalFindCnt = 0;
DynamicArray<Edge> kruskal(Graph& graph, bool unionByranks, bool pathCompression) {
	DynamicArray<Edge> tree;
	t4 = clock();
	graph.edges.quicksort(0, graph.edges.size - 1);
	t5 = clock();
	t6 = (t5 - t4) / (double)CLOCKS_PER_SEC;

	UnionFind uf(graph.nodes);
	t1 = clock();
	for (int i = 0; i < graph.edges.size; i++) {
		Edge edge = graph.edges[i];
		kruskalFindCnt += 2;
		int p1 = uf.find(edge.id1, pathCompression);
		int p2 = uf.find(edge.id2, pathCompression);
		if (p1 != p2) {
			uf.unite(p1, p2, unionByranks, pathCompression);
			tree.push_back(edge);
			if (tree.size ==  graph.nodes-1)
			{
				break;
			}
		}
	}
	t2 = clock();
	t3 = (t2 - t1) / (double)CLOCKS_PER_SEC;

	return tree;
}



int main1(std::string fname) {
	std::cout << "////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	Graph graph = readFile(fname);
	DynamicArray<Edge> tree = kruskal(graph, true, true);

	double totalWeight = 0.0;
	for (int i = 0; i < tree.size; i++) {
		totalWeight += tree[i].weight;
	}
	std::cout << "union by rank: true, path compression: true" << std::endl;
	std::cout << "Total number of edges in tree: " << tree.size << std::endl;
	std::cout << "Total weight of tree: " << totalWeight << std::endl;
	std::cout << "Number of find operations: " << findCnt << std::endl;
	std::cout << "Number of kruskal find operations: " << kruskalFindCnt << std::endl;
	std::cout << "Main Kruskal loop execution time: " << t3 << "s" << std::endl;
	std::cout << "Time of sorting the edges: " << t6 << "s" << std::endl;
	double out = (double)findCnt / (double)kruskalFindCnt;
	std::cout << "Find finds / kruskal finds: " << out << std::endl;
	findCnt = 0;
	kruskalFindCnt = 0;
	return 0;
}

int main2(std::string fname) {
	std::cout << "////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	Graph graph = readFile(fname);
	DynamicArray<Edge> tree = kruskal(graph, true, false);

	double totalWeight = 0.0;
	for (int i = 0; i < tree.size; i++) {
		totalWeight += tree[i].weight;
	}
	std::cout << "union by rank: true, path compression: false" << std::endl;
	std::cout << "Total number of edges in tree: " << tree.size << std::endl;
	std::cout << "Total weight of tree: " << totalWeight << std::endl;
	std::cout << "Number of find operations: " << findCnt << std::endl;
	std::cout << "Number of kruskal find operations: " << kruskalFindCnt << std::endl;
	std::cout << "Main Kruskal loop execution time: " << t3 << "s" << std::endl;
	std::cout << "Time of sorting the edges: " << t6 << "s" << std::endl;
	double out = (double)findCnt / (double)kruskalFindCnt;
	std::cout << "Find finds / kruskal finds: " << out << std::endl;
	findCnt = 0;
	kruskalFindCnt = 0;
	return 0;
}

int main3(std::string fname) {
	std::cout << "////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	Graph graph = readFile(fname);
	DynamicArray<Edge> tree = kruskal(graph, false, true);

	double totalWeight = 0.0;
	for (int i = 0; i < tree.size; i++) {
		totalWeight += tree[i].weight;
	}
	std::cout << "union by rank: false, path compression: true" << std::endl;
	std::cout << "Total number of edges in tree: " << tree.size << std::endl;
	std::cout << "Total weight of tree: " << totalWeight << std::endl;
	std::cout << "Number of find operations: " << findCnt << std::endl;
	std::cout << "Number of kruskal find operations: " << kruskalFindCnt << std::endl;
	std::cout << "Main Kruskal loop execution time: " << t3 << "s" << std::endl;
	std::cout << "Time of sorting the edges: " << t6 << "s" << std::endl;
	double out = (double)findCnt / (double)kruskalFindCnt;
	std::cout << "Find finds / kruskal finds: " << out  << std::endl;
	findCnt = 0;
	kruskalFindCnt = 0;
	return 0;

}

int main4(std::string fname) {
	std::cout << "////////////////////////////////////////////////////////////////////////////////////" << std::endl;
	Graph graph = readFile(fname);
	DynamicArray<Edge> tree = kruskal(graph, false, false);

	double totalWeight = 0.0;
	for (int i = 0; i < tree.size; i++) {
		totalWeight += tree[i].weight;
	}
	std::cout << "union by rank: false, path compression: false" << std::endl;
	std::cout << "Total number of edges in tree: " << tree.size << std::endl;
	std::cout << "Total weight of tree: " << totalWeight << std::endl;
	std::cout << "Number of find operations: " << findCnt << std::endl;
	std::cout << "Number of kruskal find operations: " << kruskalFindCnt << std::endl;
	std::cout << "Main Kruskal loop execution time: " << t3 << "s" << std::endl;
	std::cout << "Time of sorting the edges: " << t6 << "s" << std::endl;
	double out = (double)findCnt / (double)kruskalFindCnt;
	std::cout << "Find finds / kruskal finds: " << out << std::endl;
	findCnt = 0;
	kruskalFindCnt = 0;
	return 0;
}

int main() {

	std::string fname = "g3.txt";
	main1(fname);
	main2(fname);
	main3(fname);
	main4(fname);
	return 0;
}