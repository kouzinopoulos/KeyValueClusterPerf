// C C++ includes
#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>
// Unix OS includes
#include <time.h>
#include <unistd.h>	// to get hostname
// KeyValueClusterPerf includes
#include "ConstantValueDistribution.h"
#include "DummyKeyValueDB.h"
#include "logger.h"
#include "RamCloudKeyValueDB.h"
#include "RandomAccessPattern.h"
#include "ReadOnlyAccessPattern.h"
#include "RiakJavaKeyValueDB.h"
#include "Simulator.h"
#include "ValueDistribution.h"
#include "WriteOnlyAccessPattern.h"

Simulator::Simulator(map<string,string> databaseConfiguration,
	map<string,string> accessPatternConfiguration,
	map<string,string> valueDistributionConfiguration,
	bool skipInitialisation)
{
	// Get the types to initialise from the configuration file
	string databaseType = databaseConfiguration["databaseType"];
	string accessPatternType = accessPatternConfiguration["accessPatternType"];
	string valueDistributionType = valueDistributionConfiguration["valueDistributionType"];
	// Create the requested database
	if(databaseType.compare("RamCloud") == 0)
	{
		keyValueDB = new RamCloudKeyValueDB(databaseConfiguration);
	}
	else if(databaseType.compare("RiakJava") == 0)
	{
		keyValueDB = new RiakJavaKeyValueDB(databaseConfiguration);
	}
	else if(databaseType.compare("Dummy") == 0)
	{
		keyValueDB = new DummyKeyValueDB(databaseConfiguration);
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
	else if(accessPatternType.compare("WriteOnly") == 0)
	{
		accessPattern = new WriteOnlyAccessPattern(accessPatternConfiguration);
	}
	else
	{
		// The requested accessPattern type does not exist
		// add exception handling
	}
	// Create the requested valueDistribution
	if(valueDistributionType.compare("Constant")==0)
	{
		LOG_DEBUG("Creating valueDistribution");
		valueDistribution = new ConstantValueDistribution(valueDistributionConfiguration);
		LOG_DEBUG("Done creating ValueDistribution");
	}
	else
	{
		// The requested valueDistributionType type does not exist
		// add exception handling
		LOG_DEBUG("Unknown Value Distribution");
	}
	accessPattern->setValueDistribution(valueDistribution);
	// Initialise the keyValueDatabase
	if(!skipInitialisation)
	{
		LOG_DEBUG("Initialise kvdb");
		keyValueDB->initialise(accessPattern->getInitialisationKeyValuePairs());
		LOG_DEBUG("Finished init kvdb");
	}
	// Initialise variables
	reads = 0;
	writes = 0;
}

Simulator::Simulator()
{
	accessPattern=NULL;
	keyValueDB=NULL;
	valueDistribution=NULL;
}

Simulator::~Simulator()
{
	if(accessPattern!=NULL)
	{
		delete accessPattern;
	}
	if(keyValueDB!=NULL)
	{
		delete keyValueDB;
	}
	if(valueDistribution!=NULL)
	{
		delete valueDistribution;
	}
}

void Simulator::simulate(int runs)
{
	// Initialise timers
	struct timespec timespecStart;
	struct timespec timespecStop;

	clock_gettime(idMonotonicRaw, &timespecStart);
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
	clock_gettime(idMonotonicRaw, &timespecStop);
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
	delete[] hostname;
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
}

double Simulator::calculateDurationMicroseconds(struct timespec start, struct timespec stop)
{
	uint64_t secondsDifference = stop.tv_sec - start.tv_sec;
	uint64_t nanosecondsDifference = stop.tv_nsec - start.tv_nsec;
	nanosecondsDifference += secondsDifference*1000000000;
	return nanosecondsDifference / 1000.0;
}
