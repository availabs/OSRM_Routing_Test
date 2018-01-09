#include "SimpleXmlWriter.h"

namespace avl {

	SimpleXmlWriter::SimpleXmlWriter(std::string& ofn):
		outFileName{ ofn } {
			openFile();
		}
	SimpleXmlWriter::SimpleXmlWriter(std::string&& ofn):
		outFileName{ ofn } {
			openFile();
		}

	SimpleXmlWriter::~SimpleXmlWriter() {
		if (outFile.is_open()) {
			while (openTags.size()) {
				closeTag();
			}
			outFile.close();
		}
	}

	void SimpleXmlWriter::openFile() {
		outFile.open(outFileName);
		outFile << std::setprecision(12);
		outFile << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
	}
	
	SimpleXmlWriter& SimpleXmlWriter::write(bool singleLine) {
		if (singleLine) {
			outFile << " />" << std::endl;
		}
		else {
			outFile << ">" << std::endl;
		}
		return *this;
	}

	SimpleXmlWriter& SimpleXmlWriter::closeTag() {
		if (openTags.size()) {
			OpenTag& openTag = openTags.top();
			writeIndent(openTag.indent);
			outFile << "</" << openTag.tag << ">" <<std::endl;
			openTags.pop();
			--currentIndent;
		}
		return *this;
	}

	void SimpleXmlWriter::writeIndent(int indent) {
		if (indent == -1) {
			indent = currentIndent;
		}
		for (int i = 0; i < indent; ++i) {
			outFile << "  ";
		}
	}

}