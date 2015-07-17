#include <map>
#include <string>

#include "DummyKeyValueDB.h"

DummyKeyValueDB::DummyKeyValueDB(map<string,string> configuration)
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
	// Dummy, do nothing
	return "";
}

void DummyKeyValueDB::deleteValue(string key)
{
	// Dummy, do nothing
}