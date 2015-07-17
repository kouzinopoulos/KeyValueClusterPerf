#include "logger.h"                     // For logging throughout application
#include "KeyValueDB.h"                 // For instantiating a keyvalue database
#include "RiakKeyValueDB.h"         // For instantiating a Ramcloud database

RiakKeyValueDB::RiakKeyValueDB(map<string,string> configuration)
{
	LOG_DEBUG("INSTANCE CREATED, Not yet implemented");
}

RiakKeyValueDB::~RiakKeyValueDB()
{
	LOG_DEBUG("INSTANCE DELETED, Not yet implemente");
}

void RiakKeyValueDB::putValue(std::string key, std::string* value)
{
	LOG_DEBUG("PUT CALLED, Not yet implemente");
}

string RiakKeyValueDB::getValue(std::string key)
{
	LOG_DEBUG("GET CALLED, Not yet implemente");
}

void RiakKeyValueDB::deleteValue(std::string key)
{
	LOG_DEBUG("DELETE CALLED, Not yet implemente");
}