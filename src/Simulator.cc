#include <map>
#include <string>
#include <time.h>

#include "logger.h"
#include "Simulator.h"
#include "RamCloudKeyValueDB.h"
#include "RiakKeyValueDB.h"
#include "RandomAccessPattern.h"

// for debugging
#include <sstream>

Simulator::Simulator(map<string,string> databaseConfiguration, 
	map<string,string> accessPatternConfiguration)
{
	LOG_DEBUG("Simulator created");
	// Get the types to initialise from the configuration file
	string databaseType = databaseConfiguration["databaseType"];
	string accessPatternType = accessPatternConfiguration["accessPatternType"];
	// Create the requested database
	if(databaseType.compare("RamCloud") == 0)
	{
		keyValueDB = new RamCloudKeyValueDB(databaseConfiguration);
	}
	else if(databaseType.compare("Riak") == 0)
	{
		keyValueDB = new RiakKeyValueDB(databaseConfiguration);
	}
	else
	{
		// The requested database type does not exist
		// add exception handling
	}
	// Create the requested access pattern
	if(accessPatternType.compare("Random") == 0)
	{
		accessPattern = new RandomAccessPattern(accessPatternConfiguration);
	}
	else
	{
		// The requested accessPattern type does not exist
		// add exception handling
	}
}

Simulator::~Simulator()
{
	delete accessPattern;
	delete keyValueDB;
	LOG_DEBUG("Simulator destroyed");
}

void Simulator::simulate(int runs)
{
	LOG_DEBUG("Simulation started");
	// Initialise the keyValueDatabase
	keyValueDB->initialise(accessPattern->getInitialisationKeys());
	// Initialise timers
	struct timespec ts_start;
	struct timespec ts_stop;
	const clockid_t id = CLOCK_MONOTONIC_RAW;
	clock_gettime(id, &ts_start);
	// Do the actual simulation
	for(int i=0; i < runs; i++)
	{
		SingleAccess singleAccess = accessPattern->getNext();
		if(singleAccess.read == true)
		{
			keyValueDB->getValue(singleAccess.key);
		}
		else
		{
			keyValueDB->getValue(singleAccess.key);
			//keyValueDB->putValue(singleAccess.key, singleAccess.value);
		}
	}
	// Get final clock reading and print out difference
	clock_gettime(id, &ts_stop);
	/*stringstream ss;
	LOG_RESULTS("OUTPUT:");
	ss << ts_start.tv_sec << "." << ts_start.tv_nsec;
	LOG_RESULTS(ss.str());
	ss.str(string());
	ss << ts_stop.tv_sec << "." << ts_stop.tv_nsec;
	LOG_RESULTS(ss.str());*/
	// Calculate duration
	int seconds_difference = ts_stop.tv_sec - ts_start.tv_sec;
	int nanoseconds_difference = ts_stop.tv_nsec - ts_start.tv_nsec;
	nanoseconds_difference += seconds_difference*1000000000;
	double microsecondsPerOperation = ((double) nanoseconds_difference) / (1000.0 * runs);
	LOG_RESULTS("OUTPUT:");
	stringstream ss;
	ss << microsecondsPerOperation;
	LOG_RESULTS(ss.str());
	LOG_DEBUG("Simulation stopped");
}