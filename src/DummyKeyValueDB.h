#ifndef __DUMMYKEYVALUEDB_H__
#define __DUMMYKEYVALUEDB_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "KeyValueDB.h"


using namespace std;

/**
 * Represents a key value database without any implementation in the backend
 * This key value database is to be used when testing the performance of KeyValueClusterPerf itself
 */
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
//__DUMMYKEYVALUEDB_H__