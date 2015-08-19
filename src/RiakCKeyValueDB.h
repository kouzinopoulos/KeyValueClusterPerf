// FUNCTIONS TEMPORARILY COMMENTED OUT DUE TO LIBRARY CONFLICT WITH NEWER RIAK !!!
#ifndef __RIAKCKEYVALUEDB_H__
#define __RIAKCKEYVALUEDB_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "KeyValueDB.h"
// External library: Riak
//#include "riak.h"

using namespace std;

/**
 * Class containing all functionality to communicate with a Riak cluster using C
 */
class RiakCKeyValueDB : public KeyValueDB
{
	public:
		RiakCKeyValueDB(map<string,string> configuration);
		~RiakCKeyValueDB();

		void putValue(string key, string* value);
		string getValue(string key);
		void deleteValue(string key);

		void initialise(map<string, string> keyValuePairs);

	private:
		// a riak_config serves as your per-thread state to interact with Riak
    	/*	riak_config *cfg;
    	// riak connection
    	riak_connection  *cxn;
    	// error handling
    	riak_error err;
    	// Printing
    	char output[10240];
    	riak_print_state print_state;*/
};

#endif