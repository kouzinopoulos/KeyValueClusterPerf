#include <map>

#include "logger.h"
#include "KeyValueDB.h"
#include "RamCloudKeyValueDB.h"

using namespace std;

int main(int argc, char *argv[])
{
	try
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
	}
}