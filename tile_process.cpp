#include <dirent.h>

#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "src/cpp/SimpleXmlWriter.h"
#include "src/cpp/GeojsonWriter.h"
#include "src/cpp/Tile.h"
#include "src/cpp/utils.h"

#include "src/cpp/ArgParser.h"

void writeWay(avl::SimpleXmlWriter& writer, int wayId, avl::Node& node1, avl::Node& node2) {
	double dist{ avl::Tile::distance(node1.lat, node1.lng, node2.lat, node2.lng) };
	writer.openTag("way", "id", wayId, "visible", "true", "version", 1)
		.addTag("nd", "ref", node1.id)
		.addTag("nd", "ref", node2.id)
		.addTag("tag", "k", "height_delta", "v", node2.height - node1.height)
		.addTag("tag", "k", "distance", "v", dist)
		.addTag("tag", "k", "oneway", "v", "no")
		.addTag("tag", "k", "duration", "v", "PT5M")
		.addTag("tag", "k", "highway", "v", "path");
	writer.closeTag();
}

int main(int args, char* argv[]) {

	if (args < 3) {
		avl::log("You must supply a directory and out file arguments.");
		return 1;
	}

	bool WRITE_GEOJSON{ false };
	std::string jsonFile{ "src/geojson.json" };

	int dirArg{ 1 },
		fileArg{ 2 };

	if (args == 4) {
		std::string arg{ argv[1] };
		if (arg.substr(0, 6) == "-json=") {
			WRITE_GEOJSON = true;
			jsonFile = arg.substr(6);
		}
		else {
			avl::log("optional -json= option must come before tile_directory.");
			avl::log("usage: tile_process [-json=json_file] tile_directory osm_file");
			return 1;
		}
		dirArg = 2;
		fileArg = 3;
	}

	std::string directory{ argv[dirArg] };
	int length = directory.length();
	if (directory[length - 1] == '/') {
		directory = directory.substr(0, length - 1);
	}
	std::string outFile{ argv[fileArg] };

	DIR* dir{ nullptr };
	struct dirent* ent;
	auto mainExit = [=](int v){ if (dir) closedir(dir); return v; };

	if ((dir = opendir(directory.c_str())) != nullptr) {
		avl::log("Opened directory:", argv[1]);

		std::regex rgx{ "^tile-([[:digit:]]+)-([[:digit:]]+)-([[:digit:]]+).png$" };
		std::smatch match{};

		//std::vector<avl::Tile> tiles{};
		std::unordered_map<std::string, avl::Tile> tiles{};

		int zoom, minX{ 123456789 }, minY{ 123456789 }, maxX{ 0 }, maxY{ 0 };

		while ((ent = readdir(dir)) != nullptr) {
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

		avl::SimpleXmlWriter writer{ outFile };
		writer.openTag("osm", "version", "0.6");

		avl::GeojsonWriter geoWriter{ jsonFile };

		double minLat{ avl::Tile::getLat(zoom, maxY + 1) },
			minLng{ avl::Tile::getLng(zoom, minX) },
			maxLat{ avl::Tile::getLat(zoom, minY) },
			maxLng{ avl::Tile::getLng(zoom, maxX + 1) };

		writer.addTag("bounds", "minlat", minLat, "minlon", minLng, "maxlat", maxLat, "maxlon", maxLng);

		int nodeId{ 0 },
			wayId{ 0 },

			totalNodes{ 0 },
			totalEdges{ 0 };

		for (auto& pair : tiles) {
			avl::Tile& tile{ pair.second };

			tile.loadNodes(directory, nodeId);
			tile.makeEdges();
			tile.simplify();

			totalNodes += tile.numNodes();
			totalEdges += tile.numEdges();

			for (auto& node : tile.nodes) {
				if (node.active) {
					writer.addTag("node", "id", node.id, "lat", node.lat, "lon", node.lng, "version", 1);
				}
			}
		}

		avl::log("<tile_process> Generated", totalNodes, "nodes, and", totalEdges, "edges.");

		for (auto& pair : tiles) {
			avl::Tile& tile{ pair.second };

			for (auto& pair : tile.edgeMap) {
				int index1{ pair.first };
				avl::Node& node1{ tile.nodes[index1] };

				if (!node1.active) continue;

				std::vector<int>& edges{ pair.second };

				for (auto index2 : edges) {
					avl::Node& node2{ tile.nodes[index2] };
					if (node2.active) {
						writeWay(writer, ++wayId, node1, node2);
					if (WRITE_GEOJSON) {
							geoWriter(node1, node2);
						}
					}
				}
			}

			std::string neighborId{};

			neighborId = avl::Tile::tileId(tile.z, tile.x - 1, tile.y);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };
				for (int i{ 0 }; i < 256 * 256; i += 256) {
					avl::Node tileNode{ tile.nodes[i] },
						neighborNode{ neighborTile.nodes[i + 255] };
					// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
					writeWay(writer, ++wayId, tileNode, neighborNode);
					if (WRITE_GEOJSON) {
						geoWriter(tileNode, neighborNode);
					}

					if (i > 0) {
						neighborNode = neighborTile.nodes[i + 255 - 256];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
						if (WRITE_GEOJSON) {
							geoWriter(tileNode, neighborNode);
						}
					}
					if (i < 255 * 256) {
						neighborNode = neighborTile.nodes[i + 255 + 256];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
						if (WRITE_GEOJSON) {
							geoWriter(tileNode, neighborNode);
						}
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
					if (WRITE_GEOJSON) {
						geoWriter(tileNode, neighborNode);
					}

					if (i > 0) {
						neighborNode = neighborTile.nodes[i + 256 * 255 - 1];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
						if (WRITE_GEOJSON) {
							geoWriter(tileNode, neighborNode);
						}
					}
					if (i < 255) {
						neighborNode = neighborTile.nodes[i + 256 * 255 + 1];
						// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
						writeWay(writer, ++wayId, tileNode, neighborNode);
						if (WRITE_GEOJSON) {
							geoWriter(tileNode, neighborNode);
						}
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
				if (WRITE_GEOJSON) {
					geoWriter(tileNode, neighborNode);
				}
			}

			neighborId = avl::Tile::tileId(tile.z, tile.x - 1, tile.y - 1);
			if (tiles.count(neighborId)) {
				avl::Tile& neighborTile{ tiles[neighborId] };

				avl::Node tileNode{ tile.nodes[0] },
					neighborNode{ neighborTile.nodes[256 * 256 - 1] };
				// writeWay(writer, ++wayId, tileNode.id, neighborNode.id);
				writeWay(writer, ++wayId, tileNode, neighborNode);
				if (WRITE_GEOJSON) {
					geoWriter(tileNode, neighborNode);
				}
			}
		}
	}
	mainExit(0);
}