#ifndef __KEYVALUEDB_H__
#define __KEYVALUEDB_H__

#include <string>

class KeyValueDB
{
	public:
		virtual ~KeyValueDB() {};
		virtual void putValue(std::string key, std::string value) = 0;
		virtual std::string getValue(std::string key) = 0;
		virtual void deleteValue(std::string key) = 0;
};

#endif