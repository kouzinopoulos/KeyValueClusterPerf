#include <list>
#include <map>
#include <string>
#include <time.h>
#include <unistd.h>	// to get hostname
#include <iostream>
#include <fstream>

#include "logger.h"
#include "Simulator.h"
#include "RamCloudKeyValueDB.h"
#include "RiakKeyValueDB.h"
#include "RandomAccessPattern.h"
#include "ReadOnlyAccessPattern.h"

// for debugging
#include <sstream>

Simulator::Simulator(map<string,string> databaseConfiguration, 
	map<string,string> accessPatternConfiguration)
{
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
	else if(accessPatternType.compare("ReadOnly") == 0)
	{
		accessPattern = new ReadOnlyAccessPattern(accessPatternConfiguration);
	}
	else
	{
		// The requested accessPattern type does not exist
		// add exception handling
	}
	// Initialise the keyValueDatabase
	keyValueDB->initialise(accessPattern->getInitialisationKeyValuePairs());
	// Initialise variables
	reads = 0;
	writes = 0;
}

Simulator::Simulator()
{
	LOG_DEBUG("WARNING EMPTY CONSTRUCTOR CALLED");
	accessPattern=NULL;
	keyValueDB=NULL;
}

Simulator::~Simulator()
{
	LOG_DEBUG("destructor called");
	if(accessPattern!=NULL)
	{
		LOG_DEBUG("deleting");
		delete accessPattern;
	}
	if(keyValueDB!=NULL)
	{
		LOG_DEBUG("deleting2");
		delete keyValueDB;
	}
	LOG_DEBUG("destructor finished");
}

void Simulator::simulate(int runs)
{
	// Initialise timers
	struct timespec timespecStart;
	struct timespec timespecStop;
	const clockid_t id = CLOCK_MONOTONIC_RAW;
	clock_gettime(id, &timespecStart);
	// Do the actual simulation
	for(int i=0; i < runs; i++)
	{
		SingleAccess singleAccess = accessPattern->getNext();
		if(singleAccess.read == true)
		{
			reads++;
			keyValueDB->getValue(singleAccess.key);
		}
		else
		{
			writes++;
			keyValueDB->putValue(singleAccess.key, singleAccess.value);
		}
	}
	// Get final clock reading and print out difference
	clock_gettime(id, &timespecStop);
	double microsecondsTotal = calculateDurationMicroseconds(timespecStart, timespecStop);
	double microsecondsPerOperation = microsecondsTotal / runs;
	duration = microsecondsTotal;
	stringstream ss;
	ss << microsecondsPerOperation;
	LOG_RESULTS(ss.str());
}

// some code duplication to prevent simulation function from being slowed down
void Simulator::burnInOut(int runs)
{
	for(int i=0; i < runs; i++)
	{
		SingleAccess singleAccess = accessPattern->getNext();
		if(singleAccess.read == true)
		{
			keyValueDB->getValue(singleAccess.key);
		}
		else
		{
			keyValueDB->putValue(singleAccess.key, singleAccess.value);
		}
	}
}

map<string, string> Simulator::getResults()
{
	stringstream ss;
	map<string, string> results;
	// Add hostname so that origin can be identified
	char* hostname = new char[256];
	gethostname(hostname, 256);
	results["hostname"]=string(hostname);
	// Add number of read operations
	ss << reads;
	results["reads"]=ss.str();
	ss.str(string());
	// Add number of write operations
	ss << writes;
	results["writes"]=ss.str();
	ss.str(string());
	// Add total runtime of actual simulation
	ss << duration;
	results["duration"]=ss.str();
	ss.str(string());
	// Maybe also add timestamps later on
	return results;
}

void Simulator::mergeResults(list<map<string, string>> results, string csvFilePath)
{
	LOG_DEBUG("mergeResults called");
	ofstream csvFile(csvFilePath);
	if(csvFile.is_open())
	{
		// Add header
		csvFile << "HostName" << "," << "#reads" << "," << "#writes" << "," << "total_duration" << endl;
		// Add results
		for(list<map<string,string>>::iterator it = results.begin(); it != results.end(); it++)
		{
			map<string,string> hostResults = *it;
			csvFile << hostResults["hostname"] << "," << hostResults["reads"] << "," << hostResults["writes"] << "," << hostResults["duration"] << endl;
		}
		csvFile.close();
	}
	LOG_DEBUG("Merge finished");
}

double Simulator::calculateDurationMicroseconds(struct timespec start, struct timespec stop)
{
	uint64_t secondsDifference = stop.tv_sec - start.tv_sec;
	uint64_t nanosecondsDifference = stop.tv_nsec - start.tv_nsec;
	nanosecondsDifference += secondsDifference*1000000000;
	return nanosecondsDifference / 1000.0;
}