#include "Tile.h"

#include <cmath>
#include <cstdarg>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

namespace avl {

	template<class T>
	void log(T t) {
		std::cout << t << std::endl;
	}
	template<class T, class ...Args>
	void log(T t, Args ...args) {
		std::cout << t << ", ";
		log(args...);
	}

	Tile::Tile():
		z{ 0 }, x{ 0 }, y{ 0 }, file{}, nodes{} {}
	Tile::Tile(int _z, int _x, int _y, std::string& f):
		z{ _z }, x{ _x }, y{ _y }, file{ f }, nodes{} {}

	Node* Tile::getNode(int r, int c) {
		Node* ptr{ nullptr };
		if (nodes.size()) {
			int i = r * 256 + c;
			ptr = &nodes[i];
		}
		return ptr;
	}
	std::string Tile::getNodeId(int i) {
		int row = i / 256,
			column = i % 256;

		std::stringstream ss{};
		ss << "node-" << z << "-" << x << "-" << y << "-" << row << "-" << column;
		return ss.str();
	}

	constexpr double INVERSE_256 = 1.0 / 256.0;

	double color2height(double r, double g, float b) {
		return (r * 256.0 + g + b * INVERSE_256) - 32768.0;
	}

	/*
	def latLng(zoom, x3, y3):
		tiles = 2 ** zoom
		diameter = 2 * pi

		x2 = x3 * diameter / tiles - pi
		y2 = -(y3 * diameter / tiles - pi)

		x1 = x2
		y1 = 2 * (atan(exp(y2)) - 0.25 * pi)

		lng = x1 * 180 / pi
		lat = y1 * 180 / pi

		return zoom, lat, lng
	*/
constexpr double MY_PI = 3.14159265;
constexpr double todeg = 180.0 / MY_PI;

	double Tile::getLng(int zoom, double x3) {
		double tiles = std::pow(2.0, zoom),
			diameter = MY_PI * 2.0;

		double x2 = x3 * diameter / tiles - MY_PI;
		//double x1 = x2;

		double lng = x2 * todeg;

		return lng;
	}
	double Tile::getLat(int zoom, double y3) {
		double tiles = std::pow(2.0, zoom),
			diameter = MY_PI * 2.0;

		double y2 = MY_PI - y3 * diameter / tiles;
		double y1 = 2 * (std::atan(std::exp(y2)) - 0.25 * MY_PI);
		//double y1 = std::atan(0.5 * (std::exp(y2) - std::exp(-y2)));

		double lat = y1 * todeg;

		return lat;
/*
		var n=Math.PI-2*Math.PI*(y) /Math.pow(2,z);
    	return (180/Math.PI*Math.atan(0.5*(Math.exp(n)-Math.exp(-n))));
*/
	}

	std::string join(std::string& s1, std::string& s2) {
		return s1 + "/" + s2;
	}
	template<class ...Args>
	std::string join(std::string& s1, Args... args) {
		return s1 + "/" + join(args...);
	}

	bool Tile::loadNodes(std::string& dir) {
		std::string url = join(dir, file);
		SDL_Log("Attempting to load: %s", url.c_str());

		SDL_Surface* tile = IMG_Load(url.c_str());
		SDL_PixelFormat* format;

		if (tile) {
			tile = SDL_ConvertSurfaceFormat(tile, SDL_PIXELFORMAT_RGBA8888, 0);
			format = tile->format;

			for (int i{ 0 }; i < tile->w * tile->h; ++i) {
				Uint8 r, g, b;
				SDL_GetRGB(*((Uint32*)(tile->pixels) + i), format, &r, &g, &b);

				int row = i / 256,
					column = i % 256;

				double lat = getLat(z, static_cast<double>(y) + (static_cast<double>(row) + 0.5) * INVERSE_256),
					lng = getLng(z, static_cast<double>(x) + (static_cast<double>(column) + 0.5) * INVERSE_256),
					hght = color2height(r, g, b);

				nodes.push_back({ lat, lng, hght });
			}

			SDL_FreeSurface(tile);
		}
		else {
			SDL_Log("Could not load tile image: %s", IMG_GetError());
		}

		return static_cast<bool>(tile);
	}

}