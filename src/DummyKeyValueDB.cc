// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "DummyKeyValueDB.h"

DummyKeyValueDB::DummyKeyValueDB(map<string, string> configuration)
{
  // Dummy, do nothing
}

DummyKeyValueDB::~DummyKeyValueDB()
{
  // Dummy, do nothing
}

void DummyKeyValueDB::putValue(string key, string* value)
{
  // Dummy, do nothing
}

string DummyKeyValueDB::getValue(string key)
{
  // Dummy, do nothing, return empty string to prevent segmentation faults
  return "";
}

void DummyKeyValueDB::deleteValue(string key)
{
  // Dummy, do nothing
}