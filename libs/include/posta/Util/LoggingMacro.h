#ifndef POSTAENGINE_LOGGINGMACRO_UTILITY_H
#define POSTAENGINE_LOGGINGMACRO_UTILITY_H
#include <posta/Util/Logging.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#elif __unix__
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#else
	#error "unknown OS for debug logging"
#endif

#define LOG(...) posta::log('[', __FILENAME__, ':', __LINE__, "]\t", __VA_ARGS__);

#endif // POSTAENGINE_LOGGINGMACRO_UTILITY_H
