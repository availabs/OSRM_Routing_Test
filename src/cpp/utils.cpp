#include "utils.h"

namespace avl {

	static std::ostream* LOG_STREAM{ nullptr };

	std::ostream& logStream() {
		return LOG_STREAM ? *LOG_STREAM : std::cout;
	}

	void logStream(std::ostream& os) {
		LOG_STREAM = &os;
	}
	void setLogPrecision(int p) {
		logStream() << std::setprecision(p);
	}

	void log() {
		logStream() << std::endl;
	}
	// template<class T, class ...Args>
	// void log(T t, Args ...args) {
	// 	logStream() << t << " ";
	// 	log(args...);
	// }


	std::string join(std::string& s1, std::string& s2) {
		return s1 + "/" + s2;
	}
	// template<class ...Args>
	// std::string join(std::string& s1, Args... args) {
	// 	return s1 + "/" + join(args...);
	// }

}