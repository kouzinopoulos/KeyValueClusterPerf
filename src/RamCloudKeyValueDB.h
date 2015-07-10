#include <map>
#include <string>

#include "RamCloud.h"

#include "KeyValueDB.h"                 // For instantiating a keyvalue database


using namespace std;

class RamCloudKeyValueDB : public KeyValueDB
{
	public:
		RamCloudKeyValueDB(map<string,string> configuration);
		~RamCloudKeyValueDB();

		void putValue(string key, string value);
		string getValue(string key);
		void deleteValue(string key);
	private:
		string communicationProtocol;
		string coordinatorHost;
		string coordinatorPort;

		RAMCloud::RamCloud* ramcloudDB;
		RAMCloud::Context* ramcloudContext;

		uint64_t tableID;
};