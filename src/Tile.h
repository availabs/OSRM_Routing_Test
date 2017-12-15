#include <vector>
#include <string>

namespace avl {

	struct Node {
		double lat;
		double lng;
		double height;
	};

	struct Tile {
		int z;
		int x;
		int y;

		std::string file;

		std::vector<Node> nodes;

		Tile();
		Tile(int, int, int, std::string&);

		Node* getNode(int, int);
		std::string getNodeId(int);

		bool loadNodes(std::string&);

		static double getLng(int, double);
		static double getLat(int, double);
	};

}