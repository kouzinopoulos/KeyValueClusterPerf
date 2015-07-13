#include <map>
#include <string>

#include "KeyValueDB.h"                 // For instantiating a keyvalue database


using namespace std;

class RiakKeyValueDB : public KeyValueDB
{
	public:
		RiakKeyValueDB(map<string,string> configuration);
		~RiakKeyValueDB();

		void putValue(string key, string value);
		string getValue(string key);
		void deleteValue(string key);
};