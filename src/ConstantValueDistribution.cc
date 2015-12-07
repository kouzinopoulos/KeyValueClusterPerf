#include <map>
#include <string.h>

#include "ValueDistribution.h"
#include "ConstantValueDistribution.h"

ConstantValueDistribution::ConstantValueDistribution(Configuration* _config)
{
  // Read in the size of the values
  size = _config->valueDistributionSize;
  // create a buffer to store the values and fill it
  valueBuffer = new char[size];
  memset(valueBuffer, 'X', size - 1);
  // make the buffer 0-terminated
  valueBuffer[size - 1] = 0;
  // create a string of the buffer
  valueString = new string(valueBuffer);
}

ConstantValueDistribution::~ConstantValueDistribution()
{
  delete[] valueBuffer;
  delete valueString;
}

string* ConstantValueDistribution::getNext() { return valueString; }
