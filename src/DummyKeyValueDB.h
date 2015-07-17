#ifndef __DUMMYKEYVALUEDB_H__
#define __DUMMYKEYVALUEDB_H__

#include <map>
#include <string>

#include "KeyValueDB.h"                 // For instantiating a keyvalue database


using namespace std;

class DummyKeyValueDB : public KeyValueDB
{
	public:
		DummyKeyValueDB(map<string,string> configuration);
		~DummyKeyValueDB();

		void putValue(string key, string* value);
		string getValue(string key);
		void deleteValue(string key);
};

#endif