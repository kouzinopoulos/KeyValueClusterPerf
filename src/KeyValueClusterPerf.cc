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

typedef struct DeviceOptions {
  // Controller node configuration options
  bool controller = false;

  int hostLimit = 0;
  int simulationIteration = 0;

  string distributionType;
  string accessPattern;

  int minKey;
  int maxKey;

  float readWriteRatio;

  int objectSize;

  // Worker node configuration options
  bool worker = false;
  bool initialization = false;

  int commandPort = 0;
  int dataPort = 0;

} DeviceOptions_t;

void parseXML(const std::string& filename, Configuration* _config, int hostLimit)
{

  // Create empty property tree object
  using boost::property_tree::ptree;
  ptree pt;

  // Load XML file and put its contents in property tree.
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
        if (hostLimit > 0 && i >= hostLimit) {
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

  cout << "Addedd " << _config->dataHosts.size() << " hosts from the XML file" << endl;
}

inline bool parse_cmd_line(int _argc, char* _argv[], DeviceOptions* _options)
{
  if (_options == NULL)
    throw runtime_error("Internal error: options' container is empty.");

  namespace bpo = boost::program_options;
  bpo::options_description desc("Options");
  desc.add_options()("controller", "This instance is a controller node")("hostLimit", bpo::value<int>(),
                                                                         "Maximum number of worker nodes to connect")(
    "simulationIteration", boost::program_options::value<int>()->default_value(0), "Current simulation iteration")(
    "distributionType", bpo::value<string>()->default_value("Constant"), "Data distribution type")(
    "accessPattern", bpo::value<string>()->default_value("Random"), "Access pattern")(
    "minKey", bpo::value<int>()->default_value(1), "Minimum key")("maxKey", bpo::value<int>()->default_value(1000),
                                                                  "Maximum key")(
    "readWriteRatio", bpo::value<float>()->default_value(3.33), "Default read to write ratio")(
    "objectSize", bpo::value<int>()->default_value(1024), "Key/value size")("worker", "This instance is a worker node")(
    "initialization", bpo::value<bool>()->default_value(false), "Perform initialization of the DB?")(
    "commandPort", bpo::value<int>()->default_value(0), "Command port number to connect")(
    "dataPort", bpo::value<int>()->default_value(0), "Data port number to connect")("help", "Print help messages");

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(_argc, _argv, desc), vm);

  if (vm.count("help")) {
    cout << desc << endl;
    return false;
  }

  if (vm.count("controller") && vm.count("worker")) {
    cerr << "The node cannot be both a worker and a controller" << endl;
  }

  if (!vm.count("controller") && !vm.count("worker")) {
    cerr << "Please specify if this node is a worker or a controller" << endl;
  }

  if (vm.count("worker")) {
    cout << "worker" << endl;
  }

  bpo::notify(vm);

  if (vm.count("controller")) {
    _options->controller = true;
  }
  if (vm.count("hostLimit")) {
    _options->hostLimit = vm["hostLimit"].as<int>();
  }
  if (vm.count("simulationIteration")) {
    _options->simulationIteration = vm["simulationIteration"].as<int>();
  }
  if (vm.count("distributionType")) {
    _options->distributionType = vm["distributionType"].as<string>();
  }
  if (vm.count("accessPattern")) {
    _options->accessPattern = vm["accessPattern"].as<string>();
  }
  if (vm.count("minKey")) {
    _options->minKey = vm["minKey"].as<int>();
  }
  if (vm.count("maxKey")) {
    _options->maxKey = vm["maxKey"].as<int>();
  }
  if (vm.count("readWriteRatio")) {
    _options->readWriteRatio = vm["readWriteRatio"].as<float>();
  }
  if (vm.count("objectSize")) {
    _options->objectSize = vm["objectSize"].as<int>();
  }

  if (vm.count("worker")) {
    _options->worker = true;
  }
  if (vm.count("initialization")) {
    _options->initialization = vm["initialization"].as<bool>();
  }
  if (vm.count("commandPort")) {
    _options->commandPort = vm["commandPort"].as<int>();
  }
  if (vm.count("dataPort")) {
    _options->dataPort = vm["dataPort"].as<int>();
  }

  return true;
}

int main(int argc, char* argv[])
{
  // Container for the command line options
  DeviceOptions_t options;

  try {
    if (!parse_cmd_line(argc, argv, &options)) {
      return 0;
    }
  }
  catch (exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // Container for the XML configuration file
  Configuration_t config;

  parseXML("hostFile.xml", &config , options.hostLimit);

  // Start a controller or worker instance dependend on the program options
  if (options.controller) {
    SimulationController controller(options.simulationIteration, options.distributionType,
                                    options.accessPattern, options.minKey, options.maxKey, options.readWriteRatio,
                                    options.objectSize, &config);
    controller.connect();
    controller.execute();
  } else {
    SimulationWorker worker(options.initialization, options.commandPort, options.dataPort);
    worker.listen();
  }
}
