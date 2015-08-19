#ifndef __LOGGER_H__
#define __LOGGER_H__

#define LOG_DEBUG_OUT		// Turn debug output on
#define LOG_RESULTS_OUT		// Turn results log on

// C C++ includes
#include <iostream>

using namespace std;

#ifdef LOG_DEBUG_OUT
#define LOG_DEBUG(msg)    cout << "DEBUG: " << msg << endl
#else
#define LOG_DEBUG(msg)
#endif

#ifdef LOG_RESULTS_OUT
#define LOG_RESULTS(msg)    cout << msg << endl
#else
#define LOG_RESULTS(msg)
#endif

#endif