#include <map>
#include <string>

#include "KeyValueDB.h"
#include "logger.h"

void KeyValueDB::initialise(map<string, string> keyValuePairs)
{
  // Put every key-value pair in the database
  for (map<string, string>::iterator i = keyValuePairs.begin(); i != keyValuePairs.end(); i++) {
    putValue(i->first, &keyValuePairs[i->first]);
  }
}
