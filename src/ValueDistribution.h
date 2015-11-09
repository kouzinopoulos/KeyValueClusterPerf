#ifndef __VALUEDISTRIBUTION_H__
#define __VALUEDISTRIBUTION_H__

// C C++ includes
#include <string>

using namespace std;

/**
 * Abstract class representing the distribution of values to be written to the database
 */
class ValueDistribution {
public:
  virtual ~ValueDistribution(){};

  virtual string* getNext() = 0;
};

#endif
//__VALUEDISTRIBUTION_H__