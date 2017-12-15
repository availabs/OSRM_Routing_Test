#include <dirent.h>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "src/Tile.h"
#include "src/SimpleXmlWriter.h"

int runTest(std::string& test) {
	std::transform(test.begin(), test.end(), test.begin(), ::toupper);
	if (test == "XML") {
		std::cout << "RUNNING TEST: " << test << std::endl;
		avl::SimpleXmlWriter("TEST.xml")
			.openTag("osm", "version", 0.6, "generator", "JOSM")
				.addTag("node", "id", 1, "lat", 123.45, "lon", 123.45)
				.addTag("node", "id", 2, "lat", 456.78, "lon", 456.78)
				.openTag("way", "id", 3, "visible", "true")
					.addTag("nd", "ref", 1)
					.addTag("nd", "ref", 2)
				.closeTag()
				.openTag("way", "id", 4, "visible", "false")
					.addTag("nd", "ref", 1)
					.addTag("nd", "ref", 2)
				.closeTag()
			.closeTag();
		avl::SimpleXmlWriter("TEST2.xml")
			.openTag("osm", "version", 0.6, "generator", "JOSM")
				.addTag("node", "id", 1, "lat", 123.45, "lon", 123.45)
				.addTag("node", "id", 2, "lat", 456.78, "lon", 456.78)
				.openTag("way", "id", 3, "visible", "true")
					.addTag("nd", "ref", 1)
					.addTag("nd", "ref", 2)
				.closeTag()
				.openTag("way", "id", 4, "visible", "false")
					.addTag("nd", "ref", 1)
					.addTag("nd", "ref", 2);
	}
	else {
		return 1;
	}
	return 0;
}

void writeWay(avl::SimpleXmlWriter& writer, int wayId, int nodeId1, int nodeId2) {
	writer.openTag("way", "id", wayId, "visible", "true", "version", 1)
		.addTag("nd", "ref", nodeId1)
		.addTag("nd", "ref", nodeId2)
		.addTag("tag", "k", "oneway", "v", "no")
		.addTag("tag", "k", "duration", "v", "PT5M")
		.addTag("tag", "k", "highway", "v", "path");
	writer.closeTag();
}

int main(int args, char* argv[]) {

	if (args < 2) {
		std::cout << "You must supply a directory argument." << std::endl;
		return 1;
	}

	std::string arg2{ argv[1] };
	if (arg2.substr(0, 4) == "test") {
		std::string test{ arg2.substr(5) };
		return runTest(test);
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
		SDL_Log("Unable to initialize SDL_image: %s", IMG_GetError());
		return 1;
	}

	std::string directory{ argv[1] };

	DIR* dir;
	struct dirent* ent;

	if ((dir = opendir(directory.c_str())) != NULL) {
		std::cout << "Opened directory: " << argv[1] << "." << std::endl;

		std::regex rgx{ "^tile-([[:digit:]]+)-([[:digit:]]+)-([[:digit:]]+).png$" };
		std::smatch match{};

		std::vector<avl::Tile> tiles{};

		int zoom, minX{ -1 }, minY{ -1 }, maxX{ 0 }, maxY{ 0 };

		while ((ent = readdir(dir)) != NULL) {
			std::string file{ ent->d_name };

			if (std::regex_match(file, match, rgx)) {
				int z = std::stoi(match[1]),
					x = std::stoi(match[2]),
					y = std::stoi(match[3]);

				zoom = z;

				minX = minX == -1 ? x : std::min(minX, x);
				minY = minY == -1 ? y : std::min(minY, y);

				maxX = std::max(maxX, x);
				maxY = std::max(maxY, y);

				tiles.push_back({ z, x, y, file });
			}
		}
		std::cout << "Generated: " << tiles.size() << " tiles." << std::endl;

		avl::SimpleXmlWriter writer{ "test_1.osm" };
		writer.openTag("osm", "version", "0.6");

		double minLat{ avl::Tile::getLat(zoom, maxY + 1) },
			minLng{ avl::Tile::getLng(zoom, minX) },
			maxLat{ avl::Tile::getLat(zoom, minY) },
			maxLng{ avl::Tile::getLng(zoom, maxX + 1) };

		writer.addTag("bounds", "minlat", minLat, "minlon", minLng, "maxlat", maxLat, "maxlon", maxLng);

		std::unordered_map<std::string, int> nodeIdMap{};

		for (auto& tile : tiles) {
			if (tile.loadNodes(directory)) {
				int i{ 0 },
					objectId{ 0 };
				for (avl::Node& node : tile.nodes) {
					std::string nodeId{ tile.getNodeId(i) };
					nodeIdMap[nodeId] = ++objectId;
					writer.addTag("node", "id", objectId, "lat", node.lat, "lon", node.lng, "version", 1);
					++i;
				}
				i = 0;
				for (avl::Node& node : tile.nodes) {
					int r = i / 256,
						c = i % 256;

					std::string strId{ tile.getNodeId(i) };
					int nodeId{ nodeIdMap[strId] };

					// east
					if (c < 255) {
						int wayId{ ++objectId };
						std::string refStrId{ tile.getNodeId(i + 1) };
						int refNodeId{ nodeIdMap[refStrId] };
						writeWay(writer, wayId, nodeId, refNodeId);
					}

					// south-east
					// if (r < 255 && c < 255) {
					// 	int wayId{ ++objectId };
					// 	std::string refStrId{ tile.getNodeId(i + 257) };
					// 	int refNodeId{ nodeIdMap[refStrId] };
					// 	writeWay(writer, wayId, nodeId, refNodeId);
					// }

					// south
					if (r < 255) {
						int wayId{ ++objectId };
						std::string refStrId{ tile.getNodeId(i + 256) };
						int refNodeId{ nodeIdMap[refStrId] };
						writeWay(writer, wayId, nodeId, refNodeId);
					}

					// south-west
					// if (r < 255 && c > 0) {
					// 	int wayId{ ++objectId };
					// 	std::string refStrId{ tile.getNodeId(i + 255) };
					// 	int refNodeId{ nodeIdMap[refStrId] };
					// 	writeWay(writer, wayId, nodeId, refNodeId);
					// }
					++i;
				}
			}
		}

		closedir(dir);
	}

	return 0;
}