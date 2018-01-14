#include "ArgParser.h"

#include "./utils.h"

namespace avl {

	void ArgParser::addStringArg(const std::string& key, bool req, const std::string& def) {
		log("<ArgParser::addStringArg>", key);
		args.insert({ key, new StringArg{} });
	}
	void ArgParser::addIntArg(const std::string& key, bool req, int def) {
		log("<ArgParser::addIntArg>", key);
		args[key] = new IntArg{};
	}
	void ArgParser::addDoubleArg(const std::string& key, bool req, double def) {
		log("<ArgParser::addDoubleArg>", key);
	}
	void ArgParser::addBoolArg(const std::string& key, bool req, bool def) {
		log("<ArgParser::addBoolArg>", key);
	}

	bool ArgParser::parseArgs(int args, char* argv[]) {
		for (int i{ 1 }; i < args; ++i) {
			std::string arg{ argv[i] };
			log("<ArgParser::parseArgs>", arg);
		}
		return true;
	}

}