#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <unordered_map>

namespace avl {

	enum class ArgType { STRING_ARG, INT_ARG, DOUBLE_ARG, BOOL_ARG };

	struct Arg {
		Arg() {};
		Arg(ArgType t, bool r = false):
			type{ t }, req{ r } {}
		ArgType type;
		bool req;
	};
	struct StringArg: Arg {
		StringArg():
			Arg{ ArgType::STRING_ARG } {}
		std::string value;
	};
	struct IntArg: Arg {
		IntArg():
			Arg{ ArgType::INT_ARG } {}
		int value;
	};
	struct DoubleArg: Arg {
		DoubleArg():
			Arg{ ArgType::DOUBLE_ARG } {}
		double value;
	};
	struct BoolArg: Arg {
		BoolArg():
			Arg{ ArgType::BOOL_ARG } {}
		bool value;
	};

	class ArgParser {
	private:
		std::unordered_map<std::string, Arg*> args;

	public:
		// ARGS: (key, required, default)
		void addStringArg(const std::string&, bool = false, const std::string& = { "foobar" });
		void addIntArg(const std::string&, bool = false, int = 0);
		void addDoubleArg(const std::string&, bool = false, double = 0.0);
		void addBoolArg(const std::string&, bool = false, bool = false);

		bool parseArgs(int, char*[]);
	};

}

#endif