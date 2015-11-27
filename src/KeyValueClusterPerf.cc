#include <map>
#include <string>

#include <unistd.h>
#include <time.h>

#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

inline bool parseControllerXML(const std::string& filename, Configuration* _config)
{

  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;

  // Load XML file and put the contents in property tree.
  read_xml(filename, pt);

  BOOST_FOREACH (ptree::value_type& v, pt.get_child("hosts")) {
    if (v.first == "host") {
      int firstCommandPort = v.second.get<int>("firstCommandPort");
      int firstDataPort = v.second.get<int>("firstDataPort");
      int numberOfNodes = v.second.get<int>("numberOfNodes");
      std::string nodeAddress = v.second.get<std::string>("nodeAddress");

      stringstream ss;

      // generate and store command and data address
      for (int i = 0; i < numberOfNodes; i++) {

        // Use hostLimit if available
        if (_config->hostLimit > 0 && i >= _config->hostLimit) {
          break;
        }

        ss << "tcp://" << nodeAddress << ":" << (firstCommandPort + i);
        _config->commandHosts.push_back(ss.str());

        ss.clear();
        ss.str(string());

        ss << "tcp://" << nodeAddress << ":" << (firstDataPort + i);

        _config->dataHosts.push_back(ss.str());

        ss.clear();
        ss.str(string());
      }
    }
  }
  return true;
}

inline bool parseWorkerXML(const std::string& filename, Configuration* _config)
{

  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;

  // Load XML file and put the contents in property tree.
  read_xml(filename, pt);

  _config->accessPatternType = pt.get<std::string>("worker.accessPattern.type");
  _config->accessPatternMinKey = pt.get<int>("worker.accessPattern.minKey");
  _config->accessPatternMaxKey = pt.get<int>("worker.accessPattern.maxKey");
  _config->accessPatternReadWriteRatio = pt.get<double>("worker.accessPattern.readWriteRatio");

  _config->databaseType = pt.get<std::string>("worker.database.type");
  _config->databaseSecurity = pt.get<std::string>("worker.database.security");
  _config->databaseBroker = pt.get<std::string>("worker.database.broker");

  _config->valueDistributionType = pt.get<std::string>("worker.valueDistribution.type");
  _config->valueDistributionSize = pt.get<int>("worker.valueDistribution.size");

  return true;
}

inline bool parse_cmd_line(int _argc, char* _argv[], Configuration* _config)
{
  if (_config == NULL)
    throw runtime_error("Internal error: options' container is empty.");

  namespace bpo = boost::program_options;
  bpo::options_description desc("Options");
  desc.add_options()("controller", "This instance is a controller node")("worker", "This instance is a worker node")("hostLimit", bpo::value<int>(),
                                                                         "Maximum number of worker nodes to connect")(
    "simulationIteration", boost::program_options::value<int>()->default_value(0), "Current simulation iteration")(
    "commandPort", bpo::value<int>(), "Command port number to connect")(
    "dataPort", bpo::value<int>(), "Data port number to connect")("help", "Print help messages");

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(_argc, _argv, desc), vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return false;
  }

  if (vm.count("controller") && vm.count("worker")) {
    cerr << "The node cannot be both a worker and a controller" << endl;
    return false;
  }

  if (!vm.count("controller") && !vm.count("worker")) {
    cerr << "Please specify if this node is a worker or a controller" << endl;
    return false;
  }

  bpo::notify(vm);

  if (vm.count("controller")) {
    _config->controller = true;
  }
  if (vm.count("worker")) {
    _config->worker = true;
  }
  if (vm.count("hostLimit")) {
    _config->hostLimit = vm["hostLimit"].as<int>();
  }
  if (vm.count("simulationIteration")) {
    _config->simulationIteration = vm["simulationIteration"].as<int>();
  }
  if (vm.count("worker")) {
    _config->worker = true;
  }
  if (vm.count("initialization")) {
    _config->initialization = vm["initialization"].as<bool>();
  }
  if (vm.count("commandPort")) {
    _config->commandPort = vm["commandPort"].as<int>();
  }
  if (vm.count("dataPort")) {
    _config->dataPort = vm["dataPort"].as<int>();
  }

  return true;
}

int main(int argc, char* argv[])
{
  // Container for the configuration options
  Configuration_t config;

  try {
    if (!parse_cmd_line(argc, argv, &config)) {
      return 0;
    }
  }
  catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  try {
    if (!parseControllerXML("controllerConfiguration.xml", &config)) {
      return 0;
    }
  }
  catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  try {
    if (!parseWorkerXML("workerConfiguration.xml", &config)) {
      return 0;
    }
  }
  catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // Start a controller or worker instance based on the program options
  if (config.controller) {
    SimulationController controller(&config);
    controller.connect();
    controller.execute();
  } else {
    SimulationWorker worker(&config);
    worker.listen(&config);
  }
}
