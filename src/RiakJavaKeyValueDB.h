#ifndef __RIAKJAVAKEYVALUEDB_H__
#define __RIAKJAVAKEYVALUEDB_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "KeyValueDB.h"
#include "MessageSender.h"

using namespace std;

/**
 * Class containing functionality to perform key value operations on a Riak cluster using a Java client
 */
class RiakJavaKeyValueDB : public KeyValueDB
{
	public:
		RiakJavaKeyValueDB(map<string, string> configuration);
		~RiakJavaKeyValueDB();

		void putValue(string key, string* value);
		string getValue(string key);
		void deleteValue(string key);

		void initialise(map<string, string> keyValuePairs);

	private:
		/*! Class responsible for handling the communication aspects */
		MessageSender* messageSender;
};

#endif
//__RIAKJAVAKEYVALUEDB_H__