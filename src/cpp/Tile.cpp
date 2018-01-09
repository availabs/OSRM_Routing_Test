#include "Tile.h"

#include <cmath>
#include <sstream>

#include "./utils.h"
#include "./PNG_Image.h"

namespace avl {

	constexpr double INVERSE_256 = 1.0 / 256.0;
	constexpr double TO_DEG = 180.0 / M_PI;

		double lat,
			lng,
			height;
		int id;
		bool simplified,
			active;
	Node::Node():
		lat{ 0.0 }, lng{ 0.0 }, height{ 0.0 }, id{ -1 }, simplified{ false }, active{ false } {}
	Node::Node(double lt, double lg, double h, int i, bool s, bool a):
		lat{ lt }, lng{ lg }, height{ h }, id{ i }, simplified{ s }, active{ a } {}

	Tile::Tile():
		z{ 0 }, x{ 0 }, y{ 0 }, id{}, file{}, nodes{} {}
	Tile::Tile(int _z, int _x, int _y, std::string& f):
		z{ _z }, x{ _x }, y{ _y },
		id{ tileId(_z, _x, _y) }, file{ f }, nodes{} {}

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
					nodes[index] = std::move(Node{ lat, lng, hght, nodeId++, false, true });
				}
			}
			return true;
		}

		log("<avl::Tile::loadNodes> [Tile: " + id + "] Failed to load image file: " + url + ".");
		return false;
	}
	bool Tile::makeEdges() {
		for (int r{ 0 }; r < 256; ++r) {
			for (int c{ 0 }; c < 256; ++c) {
				int i = r * 256 + c;
				edgeMap[i] = std::vector<int>{};
				if (r < 255) {
					edgeMap[i].push_back(i + 256);
				}
				if (c < 255) {
					edgeMap[i].push_back(i + 1);
				}
				if (r < 255 && c < 255) {
					edgeMap[i].push_back(i + 256 + 1);
				}
				if (r < 255 && c > 0) {
					edgeMap[i].push_back(i + 256 - 1);
				}
			}
		}
	}
	bool Tile::simplify() {
		if (!edgeMap.size()) {
			log("<Tile::simplify> [Tile: " + id + "] You must run <Tile::makeEdges> before attempting to simplify.");
			return false;
		}
		log("<Tile::simplify> [Tile: " + id + "] START.");

		std::vector<int> nodesInsideSquare{}, temp{};

		for (int r{ 0 }; r < 256; ++r) {
			for (int c{ 0 }; c < 256; ++c) {
				int index{ r * 256 + c },
					size{ 4 };

				if (nodes[index].simplified) continue;

				while (getNodesInsideSquare(index, size, temp)) {

					double avgHeight{ getAverageHeight(temp) };

					if (!okToSimplify(temp, avgHeight)) {
						break;
					}

					nodesInsideSquare = temp;

					++size;
				}

				if (!nodesInsideSquare.empty()) {
					nodesRemoved += nodesInsideSquare.size();

					for (auto id : nodesInsideSquare) {
						nodes[id].simplified = true;
					}
				}

				nodesInsideSquare.clear();
				temp.clear();
			}
		}
		log("<Tile::simplify> [Tile: " + id + "] Removed", nodesRemoved, "nodes.");
		return true;
	}

	bool Tile::getNodesInsideSquare(int index, int size, std::vector<int>& vec) {
		if (size < 4 || size > 64) return false;

		std::vector<int> n{};

		int prevSize = sqrt(vec.size()),
			innerSize = size - 2;

		for (int r{ 0 }; r < innerSize; ++r) {
			for (int c{ 0 }; c < innerSize; ++c) {

				if (prevSize && (r != (innerSize - 1)) && (c != (innerSize - 1))) continue;

				int i{ index + 256 + 1 + r * 256 + c };
				if (i < 256 * 256) {
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
	double Tile::getAverageHeight(std::vector<int>& vec) {
		double avgHeight{ 0.0 };
		for (auto id : vec) {
			avgHeight += nodes[id].height;
		}
		avgHeight /= static_cast<double>(vec.size());
		return avgHeight;
	}
	bool Tile::okToSimplify(std::vector<int>& vec, double avg) {
		for (auto id : vec) {
			Node& node{ nodes[id] };

			if (node.simplified) return false;

			if (std::abs(node.height - avg) >= 7.0) {
				return false;
			}
		}
		return true;
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
	std::string Tile::tileId(int z, int x, int y) {
		std::stringstream ss{};
		ss << z << '-' << x << '-' << y;
		return ss.str();
	}

}