#include <map>

#include "ConfigurationManager.h"
#include "logger.h"
#include "KeyValueDB.h"
#include "RamCloudKeyValueDB.h"
#include "AccessPattern.h"
#include "RandomAccessPattern.h"
#include "Simulator.h"

using namespace std;

int main(int argc, char *argv[])
{
	// create a configuration manager
	ConfigurationManager cm;
	// Create database configuration
	map<string,string> databaseConfiguration = cm.readFile("database.cfg");
	// Create accessPattern configuration
	map<string,string> accessPatternConfiguration = cm.readFile("accessPattern.cfg");
	// Create the simulator
	Simulator* simulator = new Simulator(databaseConfiguration, accessPatternConfiguration);
	// Perform simulation
	simulator->simulate(10000);
	// Perform another simulation
	delete simulator;
	accessPatternConfiguration["accessPatternType"]="ReadOnly";
	simulator = new Simulator(databaseConfiguration, accessPatternConfiguration);
	simulator->simulate(10000);
	// Destroy the simulator
	delete simulator;
}