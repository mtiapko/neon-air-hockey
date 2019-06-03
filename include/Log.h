#ifndef __LOG_H__
#define __LOG_H__

#include <iostream>

#define LOG(...)       std::clog << __VA_ARGS__ << '\n'
#define LOG_ERROR(...) std::cerr << __VA_ARGS__ << '\n'

#endif  //  !__LOG_H__
