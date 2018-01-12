#ifndef SIMPLE_XML_WRITER_H
#define SIMPLE_XML_WRITER_H

#include <fstream>
#include <stack>
#include <string>

namespace avl {

	struct OpenTag {
		int indent;
		std::string tag;
	};

	class SimpleXmlWriter {
	private:
		std::string outFileName;
		std::ofstream outFile;

		int currentIndent = 0;
		std::stack<OpenTag> openTags;
		bool tagOpen = false;

		void openFile();

		template<class T, class ...Args>
		SimpleXmlWriter& write(bool, std::string&&, T&&, Args...);
		template<class T>
		SimpleXmlWriter& write(bool, std::string&&, T&&);
		SimpleXmlWriter& write(bool);

		void writeIndent(int=-1);

	public:
		SimpleXmlWriter(std::string&);
		SimpleXmlWriter(std::string&&);
		~SimpleXmlWriter();

		template<class ...Args>
		SimpleXmlWriter& openTag(std::string&&, Args...);

		template<class ...Args>
		SimpleXmlWriter& addTag(std::string&&, Args...);

		SimpleXmlWriter& closeTag();
	};

	template<class ...Args>
	SimpleXmlWriter& SimpleXmlWriter::openTag(std::string&& tag, Args... args) {
		if (tagOpen) {
			outFile << ">" << std::endl;
		}
		openTags.push({ currentIndent, tag });
		writeIndent();
		outFile << "<" << tag;
		++currentIndent;
		return write(false, args...);
	}
	template<class ...Args>
	SimpleXmlWriter& SimpleXmlWriter::addTag(std::string&& tag, Args... args) {
		if (tagOpen) {
			outFile << ">" << std::endl;
		}
		writeIndent();
		outFile << "<" << tag;
		return write(true, args...);
	}
	template<class T, class ...Args>
	SimpleXmlWriter& SimpleXmlWriter::write(bool singleLine, std::string&& key, T&& value, Args ...args) {
		outFile << " " << key << "=" << "'" << value << "'";
		return write(singleLine, args...);
	}
	template<class T>
	SimpleXmlWriter& SimpleXmlWriter::write(bool singleLine, std::string&& key, T&& value) {
		outFile << " " << key << "=" << "'" << value << "'";
		if (singleLine) {
			outFile << " />" << std::endl;
		}
		else {
			outFile << ">" << std::endl;
		}
		return *this;
	}

}

#endif