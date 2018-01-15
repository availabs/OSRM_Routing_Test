#ifndef AVL_TILE_H
#define AVL_TILE_H

#include <array>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace avl {

	struct Node {
		Node();
		Node(double, double, double, int, bool, bool);
		double lat,
			lng,
			height;
		int id;
		bool simplified,
			active;
	};
	struct Edge {
		int node1,
			node2;
	};

	class Tile {
	private:
		int nodesRemoved = 0;

		bool getNodesOfSquare(int, int, int, std::vector<int>&);
		bool isSquareValid(int, std::vector<int>&);
		std::vector<int> getNodesInsideSquare(std::vector<int>&);
		double getAverageHeight(std::vector<int>&);
		bool okToSimplify(double, std::vector<int>&, double);
		int condenseNodesInsideSquare(std::vector<int>&);
		void connectCorners(std::unordered_set<int>&, int);

	public:
		int z;
		int x;
		int y;

		std::string id;

		std::string file;

		std::array<Node, 256 * 256> nodes;
		std::unordered_map<int, std::vector<int>> edgeMap;

		Tile();
		Tile(int, int, int, std::string&);

		int numNodes();
		int numEdges();

		bool loadNodes(std::string&, int&);
		int makeEdges();
		// first paramater is maximum square size
		// second parameter is percent in height change OK for simplification
		int simplify(int = 64, double = 0.025);

		static double color2height(int, int, int);
		static double getLng(int, double);
		static double getLat(int, double);
		static double distance(double, double, double, double);
		static std::string tileId(int, int, int);
	};

}

#endif