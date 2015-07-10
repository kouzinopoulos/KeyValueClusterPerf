#ifndef __KEYVALUEDB_H__
#define __KEYVALUEDB_H__

#include <string>

using namespace std;

class KeyValueDB
{
	public:
		virtual ~KeyValueDB() {};
		virtual void putValue(string key, string value) = 0;
		virtual string getValue(string key) = 0;
		virtual void deleteValue(string key) = 0;
};

#endif