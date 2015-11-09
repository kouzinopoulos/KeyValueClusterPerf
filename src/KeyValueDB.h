#ifndef __KEYVALUEDB_H__
#define __KEYVALUEDB_H__

// C C++ includes
#include <string>
#include <map>

using namespace std;

/**
 * Abstract class representing all operations that a key value database should be able to perform
 * Each key value database should use this class as a base
 */
class KeyValueDB {
public:
  virtual ~KeyValueDB(){};

  virtual void putValue(string key, string* value) = 0;
  virtual string getValue(string key) = 0;
  virtual void deleteValue(string key) = 0;

  /*! Initialise the key value database with the keyvalue-pairs represented by the map*/
  void initialise(map<string, string> keyValuePairs);
};

#endif
//__KEYVALUEDB_H__