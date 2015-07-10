#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOG_DEBUG_OUT

#include <iostream>

using namespace std;

#ifdef LOG_DEBUG_OUT
#define LOG_DEBUG(msg)    cout << msg << endl
#else
#define LOG_DEBUG(msg)
#endif

#endif