// C C++ includes
#include <map>
#include <string>
// Unix OS includes
#include <unistd.h>
#include <time.h>
// External library: boost program options
#include "boost/program_options.hpp"
// KeyValueClusterPerf includes
#include "AccessPattern.h"
#include "ConfigurationManager.h"
#include "ConstantValueDistribution.h"
#include "KeyValueDB.h"
#include "logger.h"
#include "RamCloudKeyValueDB.h"
#include "RandomAccessPattern.h"
#include "SimulationController.h"
#include "SimulationWorker.h"
#include "Simulator.h"
#include "ValueDistribution.h"

using namespace std;

/**
 * Main entry point to the program
 */
int main(int argc, char* argv[])
{
  // boolean variable indicating if this is a controller instance
  bool controller = false;
  // standard paths to configuration files
  string databaseCfgPath = "database.cfg";
  string accessPatternCfgPath = "accessPattern.cfg";
  string valueDistributionCfgPath = "valueDistribution.cfg";
  string hostFilePath = "hostFile.cfg";
  // initialise values to -1 to detect when no value has been read
  int portNumber = -1;
  int dataportNumber = -1;
  int hostLimit = -1;
  int simulationIteration = -1;
  // initialise the variables to be read in
  boost::program_options::variables_map vm;

  // read in the variables
  try {
    /** Define and parse the program options
       */
    boost::program_options::options_description desc("Options");
    desc.add_options()("help", "Request help message")("controller", "Indicate this instance is the controller node")(
      "databasecfg", boost::program_options::value<string>(&databaseCfgPath),
      "Path to the database configuration file")("accesspatterncfg",
                                                 boost::program_options::value<string>(&accessPatternCfgPath),
                                                 "Path to the accessPattern configuraiton file")(
      "port", boost::program_options::value<int>(&portNumber), "Port number if not using default port")(
      "dport", boost::program_options::value<int>(&dataportNumber), "Data port number if not using default port")(
      "hostlimit", boost::program_options::value<int>(&hostLimit), "Maximum number of hosts to simulate")(
      "simiteration", boost::program_options::value<int>(&simulationIteration), "Current simulation iteration")(
      "skipinitialisation", "Indicate wether to do initialisation procedure")(
      "hostfile", boost::program_options::value<string>(&hostFilePath), "Path to the hostfile");
    try {
      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
      if (vm.count("help")) {
        std::cout << "Performance measurement of key value stores" << std::endl << desc << std::endl;
        return 0;
      }
      boost::program_options::notify(vm);
    }
    catch (boost::program_options::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
      std::cerr << desc << std::endl;
      return 1;
    }
    // Parse input into settings
    if (vm.count("controller")) {
      controller = true;
    }
  }
  catch (std::exception& e) {
    std::cerr << "Unhandled Exception" << e.what() << std::endl;
    return 1;
  }
  // Start a controller or worker instance dependend on the program options
  if (controller) {
    SimulationController controller(hostFilePath, hostLimit, simulationIteration);
    controller.connect();
    controller.execute();
  } else {
    SimulationWorker worker(databaseCfgPath, accessPatternCfgPath, valueDistributionCfgPath,
                            vm.count("skipinitialisation"));
    worker.openConnection(portNumber, dataportNumber);
    worker.listen();
  }
}
