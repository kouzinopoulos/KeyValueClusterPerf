// FUNCTIONS TEMPORARILY COMMENTED OUT DUE TO LIBRARY CONFLICT WITH NEWER RIAK !!!
#ifndef __RAMCLOUDKEYVALUEDB_H__
#define __RAMCLOUDKEYVALUEDB_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "KeyValueDB.h"
// External library: RamCloud
//#include "RamCloud.h"

using namespace std;

/**
 * Class handling all key value database operations on a RamCloud cluster
 */
class RamCloudKeyValueDB : public KeyValueDB
{
	public:
		RamCloudKeyValueDB(map<string,string> configuration);
		~RamCloudKeyValueDB();

		void putValue(string key, string* value);
		string getValue(string key);
		void deleteValue(string key);

	private:
		/*! Representation of the communication protocol used to communicate with RamCloud (tcp, udp, fast), see RamCloud documentation */
		string communicationProtocol;
		/*! String containing the address of the coordinator, see RamCloud documentation */
		string coordinatorHost;
		/*! String containing the port of the coordinator, see RamCloud documentation */
		string coordinatorPort;

		//RAMCloud::RamCloud* ramcloudDB;
		//RAMCloud::Context* ramcloudContext;

		/*! table id, see RamCloud documentation */
		uint64_t tableID;
};

#endif
//__RAMCLOUDKEYVALUEDB_H__