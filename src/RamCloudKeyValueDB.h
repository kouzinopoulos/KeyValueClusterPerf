#include "KeyValueDB.h"                 // For instantiating a keyvalue database

class RamCloudKeyValueDB : public KeyValueDB
{
	public:
		RamCloudKeyValueDB();
		~RamCloudKeyValueDB();

		void putValue(std::string key, std::string value);
		std::string getValue(std::string key);
		void deleteValue(std::string key);
};