#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include <time.h>
#include <unistd.h>

#include "boost/date_time/posix_time/posix_time.hpp"

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

Simulator::Simulator(Configuration* _config)
{
  cout << "Requested key/value database of type " << _config->databaseType << endl;

  // Create the requested database
  if (_config->databaseType.compare("RamCloud") == 0) {
    keyValueDB = new RamCloudKeyValueDB();
  } else if (_config->databaseType.compare("RiakJava") == 0) {
    keyValueDB = new RiakJavaKeyValueDB(_config);
  } else if (_config->databaseType.compare("Dummy") == 0) {
    keyValueDB = new DummyKeyValueDB();
  } else {
    // The requested database type does not exist
    // add exception handling
  }
  // Create the requested access pattern
  if (_config->accessPatternType.compare("Random") == 0) {
    accessPattern = new RandomAccessPattern(_config);
  } else if (_config->accessPatternType.compare("ReadOnly") == 0) {
    accessPattern = new ReadOnlyAccessPattern(_config);
  } else if (_config->accessPatternType.compare("WriteOnly") == 0) {
    accessPattern = new WriteOnlyAccessPattern(_config);
  } else {
    // The requested accessPattern type does not exist
    // add exception handling
  }

  // Create the requested valueDistribution
  if (_config->valueDistributionType.compare("Constant") == 0) {
    LOG_DEBUG("Creating valueDistribution");
    valueDistribution = new ConstantValueDistribution(_config);
    LOG_DEBUG("Done creating ValueDistribution");
  } else {
    // The requested valueDistributionType type does not exist
    // add exception handling
    LOG_DEBUG("Unknown Value Distribution");
  }

  accessPattern->setValueDistribution(valueDistribution);

  // Initialise the keyValueDatabase
  /*if (_config->initialization) {
    LOG_DEBUG("Initialise kvdb");
    keyValueDB->initialise(accessPattern->getInitialisationKeyValuePairs());
    LOG_DEBUG("Finished init kvdb");
  }*/
  // Initialize variables
  reads = 0;
  writes = 0;
}

Simulator::Simulator()
{
  accessPattern = NULL;
  keyValueDB = NULL;
  valueDistribution = NULL;
}

Simulator::~Simulator()
{
  if (accessPattern != NULL) {
    delete accessPattern;
  }
  if (keyValueDB != NULL) {
    delete keyValueDB;
  }
  if (valueDistribution != NULL) {
    delete valueDistribution;
  }
}

void Simulator::simulate(int runs)
{
  // Initialise timers
  struct timespec timespecStart;
  struct timespec timespecStop;

  boost::posix_time::ptime before = boost::posix_time::microsec_clock::local_time();
  clock_gettime(idMonotonicRaw, &timespecStart);
  // Do the actual simulation
  for (int i = 0; i < runs; i++) {
    SingleAccess singleAccess = accessPattern->getNext();
    if (singleAccess.read == true) {
      reads++;
      keyValueDB->getValue(singleAccess.key);
    } else {
      writes++;
      keyValueDB->putValue(singleAccess.key, singleAccess.value);
    }
  }

  // Get final clock reading and print out difference
  clock_gettime(idMonotonicRaw, &timespecStop);
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

  double microsecondsTotal = calculateDurationMicroseconds(timespecStart, timespecStop);
  double microsecondsPerOperation = microsecondsTotal / runs;
  duration = microsecondsTotal;
  stringstream ss;
  ss << microsecondsPerOperation;

  cout << "Time elapsed: " << microsecondsTotal << "μs. Time per run: " << microsecondsTotal / runs << "μs." << endl;
  cout << "Latency for " << runs << " runs: " << (now - before).total_microseconds() << "µs "
       << (now - before).total_milliseconds() << "ms " << (now - before).total_seconds()
       << "s. Latency per run: " << (now - before).total_microseconds() / runs << "µs "
       << (now - before).total_milliseconds() / runs << "ms " << (now - before).total_seconds() / runs << "s." << endl;
}

// some code duplication to prevent simulation function from being slowed down
void Simulator::burnInOut(int runs)
{
  for (int i = 0; i < runs; i++) {
    SingleAccess singleAccess = accessPattern->getNext();
    if (singleAccess.read == true) {
      keyValueDB->getValue(singleAccess.key);
    } else {
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
  results["hostname"] = string(hostname);
  delete[] hostname;
  // Add number of read operations
  ss << reads;
  results["reads"] = ss.str();
  ss.str(string());
  // Add number of write operations
  ss << writes;
  results["writes"] = ss.str();
  ss.str(string());
  // Add total runtime of actual simulation
  ss << duration;
  results["duration"] = ss.str();
  ss.str(string());
  // Maybe also add timestamps later on
  return results;
}

void Simulator::mergeResults(list<map<string, string>> results, string csvFilePath)
{
  ofstream csvFile(csvFilePath);
  if (csvFile.is_open()) {
    // Add header
    csvFile << "HostName"
            << ","
            << "#reads"
            << ","
            << "#writes"
            << ","
            << "total_duration" << endl;
    // Add results
    for (list<map<string, string>>::iterator it = results.begin(); it != results.end(); it++) {
      map<string, string> hostResults = *it;
      csvFile << hostResults["hostname"] << "," << hostResults["reads"] << "," << hostResults["writes"] << ","
              << hostResults["duration"] << endl;
    }
    csvFile.close();
  }
}

double Simulator::calculateDurationMicroseconds(struct timespec start, struct timespec stop)
{
  uint64_t secondsDifference = stop.tv_sec - start.tv_sec;
  uint64_t nanosecondsDifference = stop.tv_nsec - start.tv_nsec;
  nanosecondsDifference += secondsDifference * 1000000000;
  return nanosecondsDifference / 1000.0;
}
