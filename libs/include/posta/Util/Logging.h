#ifndef POSTAENGINE_LOGGING_UTILITY_H
#define POSTAENGINE_LOGGING_UTILITY_H
#include <iostream>
#include <cstring>

namespace posta {
	template <class T>
	void log(T t)
	{
		std::cout << t << std::endl;
	}

	template <class T, class... Ts>
	void log(T t, Ts... ts)
	{
		std::cout << t;
		log(ts...);
	}
}

#endif // POSTAENGINE_LOGGING_UTILITY_H
