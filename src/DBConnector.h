#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <list>
#include <map>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "AccessPattern.h"
#include "ConfigurationManager.h"
#include "KeyValueDB.h"
#include "MQ.h"
#include "ValueDistribution.h"

using namespace std;

/**
 * Class to perform the actual simulation and merge the results.
 * Workers use this class to simulate, controllers to merge
 */
class DBConnector {
public:
  /*! Constructor for worker node to pass the configuration */
  DBConnector(Configuration* _config);
  /*! Separate contructor for controller because it does not use any configuration */
  DBConnector();
  ~DBConnector();

  /*! start the simulation */
  void run();
  /*! Perform burn in or burn out, dous not count towards statistics */
  void burnInOut(int runs);
  /*! Get the statistics results from the last simulation */
  map<string, string> getResults();
  /*! Merge the results of multiple simulators */
  void mergeResults(list<map<string, string>> results, string csvFilePath);

private:
  /*! Count the number of loaded objects */
  unsigned int mLoadedObjects;
  /*! Count the total data size loaded in memory*/
  double mTotalDataSize;
  /*! Duration of the simulation */
  boost::posix_time::time_duration mDuration;

  double mBytesLimit = -1;

  /*! Pointer to the key value database to use in simulation */
  KeyValueDB* keyValueDB;
  /*! Pointer to access pattern to simulate */
  AccessPattern* accessPattern;
  /*! Pointer to valueDistribution to simulate */
  ValueDistribution* valueDistribution;
  /*! Contains results of last simulation */
  map<string, string> results;

  /*! Returns a char vector containing the binary payload of a ROOT file located at path path*/
  std::vector<char> load(const char* path);

  /*! Creates a ROOT object from a value binary array of size size */
  void create(std::vector<char> value);

  void putValue(std::string key, std::vector<char> value);
  std::vector<char> getValue(std::string key);

  /*! An MQ communicator object */
  MQ mq;
};

#endif
//__SIMULATOR_H__
