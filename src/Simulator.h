#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

// C C++ includes
#include <list>
#include <map>
// Unix OS includes
#include <time.h>
// KeyValueClusterPerf includes
#include "KeyValueDB.h"
#include "AccessPattern.h"
#include "ValueDistribution.h"

using namespace std;

/**
 * Class to perform the actual simulation and merge the results.
 * Workers use this class to simulate, controllers to merge
 */
class Simulator {
public:
  /*! Constructor for worker node to pass the configuration */
  Simulator(Configuration* _config);
  /*! Separate contructor for controller because it does not use any configuration */
  Simulator();
  ~Simulator();

  /*! start the simulation */
  void simulate(int runs);
  /*! Perform burn in or burn out, dous not count towards statistics */
  void burnInOut(int runs);
  /*! Get the statistics results from the last simulation */
  map<string, string> getResults();
  /*! Merge the results of multiple simulators */
  void mergeResults(list<map<string, string>> results, string csvFilePath);

private:
  /*! Count the amount of reads during simulation */
  int reads;
  /*! Count the amount of writes during simulation */
  int writes;
  /*! Duration of the simulation */
  double duration;

  /*! Pointer to the key value database to use in simulation */
  KeyValueDB* keyValueDB;
  /*! Pointer to access pattern to simulate */
  AccessPattern* accessPattern;
  /*! Pointer to valueDistribution to simulate */
  ValueDistribution* valueDistribution;
  /*! Contains results of last simulation */
  map<string, string> results;

  double calculateDurationMicroseconds(struct timespec start, struct timespec stop);

  /*! Specify which system clock to use for counting */
  const static clockid_t idMonotonicRaw = CLOCK_MONOTONIC_RAW;
};

#endif
//__SIMULATOR_H__
