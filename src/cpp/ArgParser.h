#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <unordered_map>

namespace avl {

	enum class ArgType { STRING_ARG, INT_ARG, DOUBLE_ARG };

	struct StringArg {
		ArgType type = STRING_ARG;
		char* value;
	};
	struct IntArg {
		ArgType type = INT_ARG;
		int value;
	};
	struct DoubleArg {
		ArgType type = DOUBLE_ARG;
		double value;
	};

	union Arg {
		ArgType type;
		StringArg stringArg;
		IntArg intArg;
		DoubleArg doubleArg;
	};

	class ArgParser {
	private:
		std::unordered_map<std::string, Arg> args;

	public:
		bool parseArgs(int, char**);
	};

}

#endif