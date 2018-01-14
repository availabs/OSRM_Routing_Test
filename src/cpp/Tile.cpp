#include "Tile.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <unordered_set>

#include "./utils.h"
#include "./PNG_Image.h"

namespace avl {

	constexpr double INVERSE_256 = 1.0 / 256.0;
	constexpr double TO_DEG = 180.0 / M_PI;
	constexpr double TO_RAD = M_PI / 180.0;
	constexpr double EARTH_MEAN_RADIUS = 6371000.0;

	const int MAX_SQUARE_SIZE = 32;

	Node::Node():
		lat{ 0.0 }, lng{ 0.0 }, height{ 0.0 }, id{ -1 }, simplified{ false }, active{ false } {}
	Node::Node(double lt, double lg, double h, int i, bool s, bool a):
		lat{ lt }, lng{ lg }, height{ h }, id{ i }, simplified{ s }, active{ a } {}

	Tile::Tile():
		z{ 0 }, x{ 0 }, y{ 0 }, id{}, file{}, nodes{} {}
	Tile::Tile(int _z, int _x, int _y, std::string& f):
		z{ _z }, x{ _x }, y{ _y },
		id{ tileId(_z, _x, _y) }, file{ f }, nodes{} {}

	int Tile::numNodes() {
		auto accum = [=] (int a, const Node& n) {
			return a + static_cast<int>(n.active);
		};
		return std::accumulate(nodes.begin(), nodes.end(), 0.0, accum);
	}
	int Tile::numEdges() {
		auto accum = [=] (int a, const std::pair<int, std::vector<int>>& p) {
			if (!nodes[p.first].active) return a;
			int edges{ 0 };
			for (auto i : edgeMap[p.first]) {
				edges += static_cast<int>(nodes[i].active);
			}
			return a + edges;
		};
		return std::accumulate(edgeMap.begin(), edgeMap.end(), 0.0, accum);
	}

	bool Tile::loadNodes(std::string& dir, int& nodeId) {
		std::string url = join(dir, file);

		log("<avl::Tile::loadNodes> [Tile: " + id + "] START");

		PNG_Image img{};

		if (img.load(url)) {
			for (int row{ 0 }; row < 256; ++row) {
				for (int column{ 0 }; column < 256; ++column) {

					Pixel pxl{ img.getPixel(row, column) };

					double lat = getLat(z, static_cast<double>(y) + (static_cast<double>(row) + 0.5) * INVERSE_256),
						lng = getLng(z, static_cast<double>(x) + (static_cast<double>(column) + 0.5) * INVERSE_256),
						hght = color2height(pxl.r, pxl.g, pxl.b);

					int index = row * 256 + column;
					nodes[index] = std::move(Node{ lat, lng, hght, ++nodeId, false, true });
				}
			}
			return true;
		}

		log("<avl::Tile::loadNodes> [Tile: " + id + "] Failed to load image file: " + url + ".");
		return false;
	}
	int Tile::makeEdges() {
		int num{ 0 };
		for (int r{ 0 }; r < 256; ++r) {
			for (int c{ 0 }; c < 256; ++c) {
				int i = r * 256 + c;
				edgeMap[i] = std::vector<int>{};

				// south
				if (r < 255) {
					edgeMap[i].push_back(i + 256);
					++num;
				}

				// east
				if (c < 255) {
					edgeMap[i].push_back(i + 1);
					++num;
				}

				// south-east
				if (r < 255 && c < 255) {
					edgeMap[i].push_back(i + 256 + 1);
					++num;
				}
				
				// south-west
				if (r < 255 && c > 0) {
					edgeMap[i].push_back(i + 256 - 1);
					++num;
				}
			}
		}
		return num;
	}
	int Tile::simplify() {
		if (!edgeMap.size()) {
			log("<Tile::simplify> [Tile: " + id + "] You must run <Tile::makeEdges> before attempting to simplify.");
			return false;
		}
		log("<Tile::simplify> [Tile: " + id + "] START.");

		std::vector<int> square{}, temp{};

		for (auto size : { 64, 32, 16, 8, 4 }) {

			int stagger{ 0 };

			for (int r{ 0 }; r < 256; r += (size / 2)) {
				for (int c{ stagger % 2 ? size : 0 }; c < 256; c += (size * 2)) {
					// int size{ 4 },
					// 	successfulSize{ 0 };

					while (size <= MAX_SQUARE_SIZE && getNodesOfSquare(r, c, size, temp)) {

						if (!isSquareValid(size, temp)) break;

						std::vector<int> inside{ getNodesInsideSquare(temp) };

						double avgHeight{ getAverageHeight(inside) };

						if (!okToSimplify(avgHeight, inside)) break;

						square = temp;
						// successfulSize = size;
						// ++size;
					}

					// if (successfulSize) {
					if (square.size()) {
						nodesRemoved += condenseNodesInsideSquare(square);

						// c += successfulSize - 2;
					}

					square.clear();
					temp.clear();
				}
				++stagger;
			}

		}
		log("<Tile::simplify> [Tile: " + id + "] Removed", nodesRemoved, "nodes.");
		return nodesRemoved;
	}

	bool Tile::getNodesOfSquare(int row, int column, int size, std::vector<int>& vec) {
		std::vector<int> n{};

		int prevSize = sqrt(vec.size());

		for (int r{ 0 }; r < size; ++r) {
			for (int c{ 0 }; c < size; ++c) {

				if (prevSize && (r != (size - 1)) && (c != (size - 1))) continue;

				int i{ (row + r) * 256 + column + c };
				if (((row + r) < 256) && ((column + c) < 256)) {
					n.push_back(i);
				}
				else {
					return false;
				}
			}
		}
		vec.insert(vec.end(), n.begin(), n.end());
		return true;
	}
	bool Tile::isSquareValid(int size, std::vector<int>& vec) {
		if (size * size != static_cast<int>(vec.size())) return false;
		for (auto i : vec) {
			if (nodes[i].simplified || !nodes[i].active) return false;
		}
		return true;
	}
	std::vector<int> Tile::getNodesInsideSquare(std::vector<int>& vec) {
		int size = std::sqrt(vec.size());
		std::vector<int> in{};
		std::sort(vec.begin(), vec.end());
		for (int r{ 0 }; r < size; ++r) {
			for (int c{ 0 }; c < size; ++c) {
				if (r == 0 || c == 0 || r == (size - 1) || c == (size - 1)) continue;
				int i{ r * size + c };
				in.push_back(vec[i]);
			}
		}
		return in;
	}
	double Tile::getAverageHeight(std::vector<int>& vec) {
		double avgHeight{ 0.0 };
		for (auto index : vec) {
			avgHeight += nodes[index].height;
		}
		avgHeight /= static_cast<double>(vec.size());
		return avgHeight;
	}
	bool Tile::okToSimplify(double avg, std::vector<int>& vec) {
		double inv = 1.0 / avg;
		for (auto index : vec) {
			Node& node{ nodes[index] };
			if (std::abs((node.height - avg) * inv) > 0.0075) {
				return false;
			}
		}
		return true;
	}
	int Tile::condenseNodesInsideSquare(std::vector<int>& vec) {
		std::unordered_set<int> edgeSet(vec.begin(), vec.end());
		std::vector<int> inside{ getNodesInsideSquare(vec) };
		std::sort(inside.begin(), inside.end());

		int index{ inside[0] };
		Node& node{ nodes[index] };

		double minLat{ node.lat }, maxLat{ node.lat }, minLng{ node.lng }, maxLng{ node.lng };

		for (auto ndx : inside) {
			Node& nd{ nodes[ndx] };
			nd.active = false;
			nd.simplified = true;

			minLat = std::min(minLat, nd.lat);
			maxLat = std::max(maxLat, nd.lat);
			minLng = std::min(minLng, nd.lng);
			maxLng = std::max(maxLng, nd.lng);

			nd.lat = 0.0;
			nd.lng = 0.0;
			nd.height = 0.0;

			edgeSet.erase(ndx);
			edgeMap.erase(ndx);
		}

		node.active = true;
		node.lat = (minLat + maxLat) * 0.5;
		node.lng = (minLng + maxLng) * 0.5;

		for (auto i : { index - 1, index - 256, index - 256 - 1, index - 256 + 1 }) {
			edgeSet.erase(i);
		}
		edgeMap[index] = std::vector<int>{};
		edgeMap[index].insert(edgeMap[index].begin(), edgeSet.begin(), edgeSet.end());

		return inside.size() - 1;
	}

	double Tile::color2height(int r, int g, int b) {
		double _r = r,
			_g = g,
			_b = b;
		return (_r * 256.0 + _g + _b * INVERSE_256) - 32768.0;
	}
	double Tile::getLng(int zoom, double x3) {
		double tiles = std::pow(2.0, zoom),
			diameter = M_PI * 2.0;

		double x2 = x3 * diameter / tiles - M_PI;

		double lng = x2 * TO_DEG;

		return lng;
	}
	double Tile::getLat(int zoom, double y3) {
		double tiles = std::pow(2.0, zoom),
			diameter = M_PI * 2.0;

		double y2 = M_PI - y3 * diameter / tiles;
		double y1 = 2 * (std::atan(std::exp(y2)) - 0.25 * M_PI);

		double lat = y1 * TO_DEG;

		return lat;
	}
	double Tile::distance(double lat1, double lng1, double lat2, double lng2) {
		double theta1 = lat1 * TO_RAD,
			theta2 = lat2 * TO_RAD,
			deltaTheta = (lat2 - lat1) * TO_RAD,
			delatLambda = (lng2 - lng1) * TO_RAD,

			a = std::sin(deltaTheta * 0.5) * std::sin(deltaTheta * 0.5) +
				std::cos(theta1) * std::cos(theta2) *
				std::sin(delatLambda * 0.5) * std::sin(delatLambda * 0.5),

			c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a)),

			d = EARTH_MEAN_RADIUS * c;

		return d;
	}
	std::string Tile::tileId(int z, int x, int y) {
		std::stringstream ss{};
		ss << z << '-' << x << '-' << y;
		return ss.str();
	}

}