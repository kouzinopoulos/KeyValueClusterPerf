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
	/*try
	{
		LOG_DEBUG("Application started");
		// Building configuration for Ramcloud database
		map<string,string> configuration;
		configuration["communicationProtocol"]="tcp";
		configuration["coordinatorHost"]="cernvmbl031";
		configuration["coordinatorPort"]="11101";
		// Generate test key value database
		KeyValueDB* testDB;
		testDB = new RamCloudKeyValueDB(configuration);
		// Put in a value and read back
		testDB->putValue("TestKey", "TestValue");
		string s = testDB->getValue("TestKey");
		LOG_DEBUG("Reading value:");
		LOG_DEBUG(s);
		// Delete value from database
		testDB->deleteValue("TestKey");
		// Test if value still there
		s = testDB->getValue("TestKey");
		LOG_DEBUG("Reading value:");
		LOG_DEBUG(s);
		// Clean up database
		delete testDB;
	}
	catch (RAMCloud::ClientException& e) {
    	fprintf(stderr, "RAMCloud client exception caught: %s\n", e.str().c_str());
    	return 1;
	} catch (RAMCloud::Exception& e) {
    	fprintf(stderr, "RAMCloud exception caught: %s\n", e.str().c_str());
    	return 1;
	}*/

    /*
	LOG_DEBUG("Testing RandomAccessPattern Class");
	// setup configuration
	map<string,string> configuration;
	configuration["minKey"]="10";
	configuration["maxKey"]="1000";
	configuration["readWriteRatio"]="2.0";
	// create access pattern
	AccessPattern* accessPattern;
	accessPattern = new RandomAccessPattern(configuration);
	// test the access pattern
	for(int i=0; i<20; i++)
	{
		SingleAccess test = accessPattern->getNext();
		LOG_DEBUG(test.key);
	}
	// remove access pattern
	delete accessPattern;
	*/
	// create a configuration manager
	ConfigurationManager cm;
	// Create database configuration
	map<string,string> databaseConfiguration;
	databaseConfiguration["databaseType"]="RamCloud";
	databaseConfiguration["communicationProtocol"]="tcp";
	databaseConfiguration["coordinatorHost"]="cernvmbl031";
	databaseConfiguration["coordinatorPort"]="11101";
	string temp = cm.writeString(databaseConfiguration);
	map<string, string> cfg = cm.readString(temp);
	LOG_RESULTS(cm.writeString(cfg));
	// Create accessPattern configuration
	map<string,string> accessPatternConfiguration;
	accessPatternConfiguration["accessPatternType"]="Random";
	accessPatternConfiguration["minKey"]="10";
	accessPatternConfiguration["maxKey"]="1000";
	accessPatternConfiguration["readWriteRatio"]="2.0";
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