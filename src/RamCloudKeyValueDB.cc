// FUNCTIONS TEMPORARILY COMMENTED OUT DUE TO LIBRARY CONFLICT WITH NEWER RIAK
// !!!

// C C++ includes
#include <sstream>
// External library: RamCLoud
//#include "RamCloud.h"
// KeyValueClusterPerf includes
#include "logger.h"
#include "KeyValueDB.h"
#include "RamCloudKeyValueDB.h"

RamCloudKeyValueDB::RamCloudKeyValueDB()
{
  /*	// Generate coordinatorLocator string
    string coordinatorLocator;
    coordinatorLocator = configuration["communicationProtocol"];	// add
    protocol
    coordinatorLocator += ":host=";
    coordinatorLocator += configuration["coordinatorHost"];			// add
    hostname
    coordinatorLocator += ",port=";
    coordinatorLocator += configuration["coordinatorPort"];			// add
    port

    // Setup the cluster connector
    ramcloudContext = new RAMCloud::Context(false);
    ramcloudDB = new RAMCloud::RamCloud(ramcloudContext,
    coordinatorLocator.c_str(), "");

    // Create a table in the cluster
    ramcloudDB->createTable("KeyValueClusterPerf");
    tableID = ramcloudDB->getTableId("KeyValueClusterPerf");*/
}

RamCloudKeyValueDB::~RamCloudKeyValueDB()
{
  // Tear down created table
  /*ramcloudDB->dropTable("KeyValueClusterPerf");

  // Remove cluster connection
  delete ramcloudDB;
  delete ramcloudContext;*/
}

void RamCloudKeyValueDB::putValue(std::string key, std::string* value)
{
  // table_id, key, keylength, buffer, bufferlength
  /* ramcloudDB->write(tableID, key.c_str(), key.size(), value->c_str(),
   * value->size());*/
}

string RamCloudKeyValueDB::getValue(std::string key)
{
  // Prepare buffer and read value
  /*RAMCloud::Buffer buffer;
    ramcloudDB->read(tableID, key.c_str(), key.size(), &buffer);
    int length = buffer.size();
    string s(static_cast<const char*>(buffer.getRange(0, length)), length );
    return s;*/
  return "";
}

void RamCloudKeyValueDB::deleteValue(std::string key)
{
  /*ramcloudDB->remove(tableID, key.c_str(), key.size(), NULL, NULL);*/
}
