#include <map>
#include <string>
#include <unistd.h>

#include "boost/program_options.hpp" 

#include "ConfigurationManager.h"
#include "logger.h"
#include "KeyValueDB.h"
#include "RamCloudKeyValueDB.h"
#include "AccessPattern.h"
#include "RandomAccessPattern.h"
#include "Simulator.h"
#include "SimulationController.h"
#include "SimulationWorker.h"

#include "ValueDistribution.h"
#include "ConstantValueDistribution.h"
#include <time.h>

using namespace std;

int main(int argc, char *argv[])
{
	bool controller = false;
	string databaseCfgPath = "database.cfg";
	string accessPatternCfgPath = "accessPattern.cfg";
	string valueDistributionCfgPath = "valueDistribution.cfg";
	string hostFilePath = "hostFile.cfg";

	try 
	{ 
		/** Define and parse the program options 
	     */ 
	    boost::program_options::options_description desc("Options"); 
	    desc.add_options() 
	      ("help", "Request help message") 
	      ("controller", "Indicate this instance is the controller node") 
	      ("databasecfg", boost::program_options::value<string>(&databaseCfgPath), "Path to the database configuration file")
	      ("accesspatterncfg", boost::program_options::value<string>(&accessPatternCfgPath), "Path to the accessPattern configuraiton file")
	      ("hostfile", boost::program_options::value<string>(&hostFilePath), "Path to the hostfile");
	    boost::program_options::variables_map vm; 
	    try 
	    { 
	      boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	      if ( vm.count("help")  ) 
	      { 
	        std::cout << "Performance measurement of key value stores" << std::endl << desc << std::endl; 
	        return 0; 
	      }  
	      boost::program_options::notify(vm);
	    } 
	    catch(boost::program_options::error& e) 
	    { 
	      std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
	      std::cerr << desc << std::endl; 
	      return 1; 
	    }
	    // Parse input into settings
	    if(vm.count("controller"))
	    {
	    	controller = true;
	    }
	} 
	catch(std::exception& e) 
	{ 
	   std::cerr << "Unhandled Exception"<< e.what() << std::endl; 
	   return 1; 
	}

	if(controller)
	{
		SimulationController controller(hostFilePath);
		controller.connect();
		controller.execute();
	}
	else
	{
		SimulationWorker worker(databaseCfgPath, accessPatternCfgPath, valueDistributionCfgPath);
		worker.openConnection();
		worker.listen();
	}
}
