#include "GeojsonWriter.h"

#include <iomanip>
#include <iostream>

namespace avl {

	GeojsonWriter::GeojsonWriter(std::string& ofn):
		outFileName{ ofn } {
			openFile();
		}
	GeojsonWriter::GeojsonWriter(std::string&& ofn):
		outFileName{ ofn } {
			openFile();
		}
	GeojsonWriter::~GeojsonWriter() {
		if (outFile.is_open()) {
			outFile << std::endl << "    ]" << std::endl;
			outFile << "}" << std::endl;
			outFile.close();
			std::cout << "<GeojsonWriter> NUMBER OF FEATURES WRITTEN: " << writes << std::endl;
		}
	}

	void GeojsonWriter::openFile() {
		outFile.open(outFileName);
		if (outFile.is_open()) {
			outFile << std::setprecision(12);
			outFile << "{" << std::endl;
			outFile << "    \"type\": \"FeatureCollection\"," << std::endl;
			outFile << "    \"features\": [" << std::endl;
		}
		else {
			std::cout << "<GeojsonWriter> FAILED TO OPEN: " << outFileName << std::endl;
		}
	}

	GeojsonWriter& GeojsonWriter::operator()(const Node& node1, const Node& node2) {
		if (outFile.is_open()) {
			++writes;
			if (!featureWritten) {
				featureWritten = true;
			}
			else {
				outFile << "," << std::endl;
			}
			outFile << "        { \"type\": \"Feature\"," << std::endl;
			outFile << "            \"properties\": {" << std::endl;
			outFile << "                \"node1\": " << node1.id << "," << std::endl;
			outFile << "                \"node2\": " << node2.id << "," << std::endl;
			outFile << "                \"heightDiff\": " << (node2.height - node1.height) << std::endl;
			outFile << "            }, " << std::endl;
			outFile << "            \"geometry\": {" << std::endl;
			outFile << "                \"type\": \"LineString\"," << std::endl;
			outFile << "                \"coordinates\": [" << std::endl;
			outFile << "				    [" << node1.lng << ", " << node1.lat << "], " << std::endl;
			outFile << "				    [" << node2.lng << ", " << node2.lat << "]" << std::endl;
			outFile << "                ]" << std::endl;
			outFile << "            }" << std::endl;
			outFile << "        }";
		}
		return *this;
	}

}