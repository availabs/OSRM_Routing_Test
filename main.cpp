#include <dirent.h>

#include <algorithm>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "src/cpp/SimpleXmlWriter.h"
#include "src/cpp/Tile.h"
#include "src/cpp/utils.h"

void writeWay(avl::SimpleXmlWriter& writer, int wayId, int nodeId1, int nodeId2) {
	writer.openTag("way", "id", wayId, "visible", "true", "version", 1)
		.addTag("nd", "ref", nodeId1)
		.addTag("nd", "ref", nodeId2)
		.addTag("tag", "k", "oneway", "v", "no")
		.addTag("tag", "k", "duration", "v", "PT5M")
		.addTag("tag", "k", "highway", "v", "path");
	writer.closeTag();
}
void writeWay(avl::SimpleXmlWriter& writer, int wayId, avl::Node& node1, avl::Node& node2) {
	writer.openTag("way", "id", wayId, "visible", "true", "version", 1)
		.addTag("nd", "ref", node1.id)
		.addTag("nd", "ref", node2.id)
		.addTag("tag", "k", "slope", "v", node2.height - node1.height)
		.addTag("tag", "k", "oneway", "v", "no")
		.addTag("tag", "k", "duration", "v", "PT5M")
		.addTag("tag", "k", "highway", "v", "path");
	writer.closeTag();
}

int main(int args, char* argv[]) {

	if (args < 2) {
		avl::log("You must supply a directory argument.");
		return 1;
	}

	std::string directory{ argv[1] };

	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(directory.c_str())) != NULL) {
		avl::log("Opened directory:", argv[1]);

		std::regex rgx{ "^tile-([[:digit:]]+)-([[:digit:]]+)-([[:digit:]]+).png$" };
		std::smatch match{};

		//std::vector<avl::Tile> tiles{};
		std::unordered_map<std::string, avl::Tile> tiles{};

		int zoom, minX{ 123456789 }, minY{ 123456789 }, maxX{ 0 }, maxY{ 0 };

		while ((ent = readdir(dir)) != NULL) {
			std::string file{ ent->d_name };

			if (std::regex_match(file, match, rgx)) {
				int z = std::stoi(match[1]),
					x = std::stoi(match[2]),
					y = std::stoi(match[3]);

				zoom = z;

				minX = std::min(minX, x);
				minY = std::min(minY, y);

				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);

				avl::Tile tile{ z, x, y, file };
				tiles.insert({ tile.id, tile });
			}
		}
		avl::log("Generated:", tiles.size(), "tiles");

		avl::SimpleXmlWriter writer{ "test_1.osm" };
		writer.openTag("osm", "version", "0.6");

		double minLat{ avl::Tile::getLat(zoom, maxY + 1) },
			minLng{ avl::Tile::getLng(zoom, minX) },
			maxLat{ avl::Tile::getLat(zoom, minY) },
			maxLng{ avl::Tile::getLng(zoom, maxX + 1) };

		writer.addTag("bounds", "minlat", minLat, "minlon", minLng, "maxlat", maxLat, "maxlon", maxLng);

		int nodeId{ 0 },
			wayId{ 0 };

		for (auto& pair : tiles) {
			avl::Tile& tile{ pair.second };
			tile.loadNodes(directory, nodeId);
			tile.makeEdges();
			tile.simplify();
		}

		return 0;

		for (auto& pair : tiles) {
			avl::Tile& tile{ pair.second };

			if (tile.loadNodes(directory, nodeId)) {
				for (avl::Node& node : tile.nodes) {
					writer.addTag("node", "id", node.id, "lat", node.lat, "lon", node.lng, "version", 1);
				}
			}
		}

		std::string neighborId{};
		for (auto& pair : tiles) {
			avl::Tile& tile{ pair.second };

			int i{ 0 };
			for (avl::Node& node : tile.nodes) {
				int r = (i / 256),
					c = (i % 256);

				avl::Node& node1{ tile.nodes[i] };

				// east
				if (c + 1 < 256) {
					avl::Node& node2{ tile.nodes[i + 1] };
					// writeWay(writer, ++wayId, node1.id, node2.id);
					writeWay(writer, ++wayId, node1, node2);
				}
				// south-east
				if ((r + 1 < 256) && (c + 1 < 256)) {
					avl::Node& node2{ tile.nodes[i + 256 + 1] };
					// writeWay(writer, ++wayId, node1.id, node2.id);
					writeWay(writer, ++wayId, node1, node2);
				}
				//south
				if (r + 1 < 256) {
					avl::Node& node2{ tile.nodes[i + 256] };
					// writeWay(writer, ++wayId, node1.id, node2.id);
					writeWay(writer, ++wayId, node1, node2);
				}
				// south-west
				if ((r + 1 < 256) && (c - 1 > 0)) {
					avl::Node& node2{ tile.nodes[i + 256 - 1] };
					// writeWay(writer, ++wayId, node1.id, node2.id);
					writeWay(writer, ++wayId, node1, node2);
				}
				++i;
			}

			neighborId = avl::Tile::tileId(tile.z, tile.x - 1, tile.y);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };
				for (int i{ 0 }; i < 256 * 256; i += 256) {
					avl::Node tileNode{ tile.nodes[i] },
						neighborNode{ neighborTile.nodes[i + 255] };
					// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
					writeWay(writer, ++wayId, tileNode, neighborNode);

					if (i > 0) {
						neighborNode = neighborTile.nodes[i + 255 - 256];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
					}
					if (i < 255 * 256) {
						neighborNode = neighborTile.nodes[i + 255 + 256];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
					}
				}
			}

			neighborId = avl::Tile::tileId(tile.z, tile.x, tile.y - 1);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };
				for (int i{ 0 }; i < 256; ++i) {
					avl::Node tileNode{ tile.nodes[i] },
						neighborNode{ neighborTile.nodes[i + 256 * 255] };
					// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
					writeWay(writer, ++wayId, tileNode, neighborNode);
					if (i > 0) {
						neighborNode = neighborTile.nodes[i + 256 * 255 - 1];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
					}
					if (i < 255) {
						neighborNode = neighborTile.nodes[i + 256 * 255 + 1];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
					}
				}
			}

			neighborId = avl::Tile::tileId(tile.z, tile.x + 1, tile.y - 1);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };

				avl::Node tileNode{ tile.nodes[255] },
					neighborNode{ neighborTile.nodes[256 * 255] };
				// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
				writeWay(writer, ++wayId, tileNode, neighborNode);
			}

			neighborId = avl::Tile::tileId(tile.z, tile.x - 1, tile.y - 1);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };

				avl::Node tileNode{ tile.nodes[0] },
					neighborNode{ neighborTile.nodes[256 * 256 - 1] };
				// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
				writeWay(writer, ++wayId, tileNode, neighborNode);
			}
		}

		closedir(dir);
	}

	return 0;
}