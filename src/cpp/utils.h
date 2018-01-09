#include <iostream>

namespace avl {

	template<class T>
	void log(T t) {
		std::cout << t << std::endl;
	}
	template<class T, class ...Args>
	void log(T t, Args ...args) {
		std::cout << t << " ";
		log(args...);
	}

	std::string join(std::string&, std::string&);
	template<class ...Args>
	std::string join(std::string& s1, Args... args) {
		return s1 + "/" + join(args...);
	}

}