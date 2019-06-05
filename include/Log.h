#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define LOG_IMPL(out, title, ...) out << title << " " __FILE__ "::" << __func__ \
	<< " (" << __LINE__ << ") - " << __VA_ARGS__ << '\n'

#define LOG(...)       LOG_IMPL(std::clog, "[ LOG ]", ##__VA_ARGS__)
#define LOG_ERROR(...) LOG_IMPL(std::cerr, "[ERROR]", ##__VA_ARGS__)

#endif  //  !__LOG_H__
