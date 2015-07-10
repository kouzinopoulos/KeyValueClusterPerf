#include "logger.h"                     // For logging throughout application
#include "KeyValueDB.h"                 // For instantiating a keyvalue database
#include "RamCloudKeyValueDB.h"         // For instantiating a Ramcloud database

RamCloudKeyValueDB::RamCloudKeyValueDB()
{
	LOG("INSTANCE CREATED, Not yet implemented");
}

RamCloudKeyValueDB::~RamCloudKeyValueDB()
{
	LOG("INSTANCE DELETED, Not yet implemented");
}

void RamCloudKeyValueDB::putValue(std::string key, std::string value)
{
	LOG("PUT CALLED, Not yet implemented");
}

std::string RamCloudKeyValueDB::getValue(std::string key)
{
	LOG("GET CALLED, Not yet implemented");
}

void RamCloudKeyValueDB::deleteValue(std::string key)
{
	LOG("DELETE CALLED, Not yet implemented");
}