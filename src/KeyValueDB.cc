#include <map>
#include <string>

#include "KeyValueDB.h"

void KeyValueDB::initialise(map<string, string> keyValuePairs)
{
	for(map<string, string>::iterator i = keyValuePairs.begin(); i != keyValuePairs.end(); i++)
	{
		putValue(i->first, keyValuePairs[i->first]);
	}
}