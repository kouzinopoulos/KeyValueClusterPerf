#ifndef __CONFIGURATIONMANAGER_H__
#define __CONFIGURATIONMANAGER_H__

// C C++ includes
#include <list>
#include <map>
#include <string>

using namespace std;

typedef struct Configuration {

  // Controller node configuration options
  bool controller = false;

  list<string> commandHosts;
  list<string> dataHosts;

  int hostLimit = -1;
  int simulationIteration = -1;


  // Worker node configuration options
  bool worker = false;

  bool initialization = false;

  int commandPort = 0;
  int dataPort = 0;

  std::string accessPatternType;
  int accessPatternMinKey;
  int accessPatternMaxKey;
  double accessPatternReadWriteRatio;

  std::string databaseType;
  std::string databaseSecurity;
  std::string databaseBroker;

  std::string valueDistributionType;
  int valueDistributionSize;
} Configuration_t;

/**
 * ConfigurationManager contains all functionality to read and write configuration files
 * used in AccessPattern, KeyValueDB and ValueDistribution as well as reading in the hostfile in SimulationController
 */
class ConfigurationManager {
public:
  /*! Read in a file containing 'key=value' lines and return the data in a map */
  map<string, string> readFile(string fileName);
  /*! Write out a map of key=value pairs to a file */
  void writeFile(string fileName, map<string, string> configuration);
  /*! Read in a string containing 'key=value' lines and return the data in a map */
  map<string, string> readString(string str);
  /*! Write out a map of key=value pairs to a string */
  string writeString(map<string, string> configuration);

  /*! Read in a host file containing the data to connect to worker instances of KeyValueClusterPerf */
  list<string> readHostFile(string fileName);
};

#endif
//__CONFIGURATIONMANAGER_H__
