#ifndef __ACCESSPATTERN_H__
#define __ACCESSPATTERN_H__

#include <map>
#include <string>

#include "ValueDistribution.h"

using namespace std;

/**
 *	A single access to the database is defined as a struct to increase speed and prevent memory errors
 *  It shows if it is a read or write operation, what the key is and a pointer to the value
 *  This value should not be cleared as it is owned and reused by the valueDistribution
 */
struct SingleAccess {
  bool read;
  string key;
  string* value;
};

/**
 *  Abstract class AccessPattern represents a set of functions to be implemented by each accessPattern
 *  This class defines the accesses to the database and generates SingleAccess returns
 */
class AccessPattern {
public:
  virtual ~AccessPattern(){};

  /*! generate the next access information to the database */
  virtual SingleAccess getNext() = 0;
  /*! get key-value pairs to initialise database before simulation. This prevents access to non-existend items */
  virtual map<string, string> getInitialisationKeyValuePairs() = 0;
  /*! Assign the accessPattern class which valueDistribution to use during generation of the next access */
  void setValueDistribution(ValueDistribution* valueDistribution);

protected:
  ValueDistribution* valueDistribution;
};

#endif
//__ACCESSPATTERN_H__
