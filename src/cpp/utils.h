#include <iomanip>
#include <iostream>

namespace avl {

	void logStream(std::ostream&);
	std::ostream& logStream();

	void setLogPrecision(int);

	void log();
	template<class T, class ...Args>
	void log(T t, Args ...args) {
		logStream() << t << " ";
		log(args...);
	}

	std::string join(std::string&, std::string&);
	template<class ...Args>
	std::string join(std::string& s1, Args... args) {
		return s1 + "/" + join(args...);
	}

}