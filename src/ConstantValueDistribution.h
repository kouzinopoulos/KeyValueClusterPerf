#ifndef __CONSTANTVALUEDISTRIBUTION_H__
#define __CONSTANTVALUEDISTRIBUTION_H__

#include <map>
#include <string>

#include "ConfigurationManager.h"
#include "ValueDistribution.h"

using namespace std;

/**
 * Represents a value distribution with a fixed lenght
 */
class ConstantValueDistribution : public ValueDistribution {
public:
  ConstantValueDistribution(Configuration* _config);
  ~ConstantValueDistribution();

  string* getNext();

private:
  /*! size of the generated values */
  int size;
  /*! char buffer containing data to be returned when the next value is requested */
  char* valueBuffer;
  /*! string representing the value to be returned */
  string* valueString;
};

#endif
//__CONSTANTVALUEDISTRIBUTION_H__
