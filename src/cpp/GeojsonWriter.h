#ifndef GEOJSON_WRITER_H
#define GEOJSON_WRITER_H

#include <fstream>
#include <stack>
#include <string>

#include "Tile.h"

namespace avl {

	class GeojsonWriter {
	private:
		std::string outFileName;
		std::ofstream outFile;

		void openFile();

		bool featureWritten = false;

		int writes = 0;

	public:
		GeojsonWriter(std::string&);
		GeojsonWriter(std::string&&);
		~GeojsonWriter();

		GeojsonWriter& operator()(const Node&, const Node&);
	};

}

#endif