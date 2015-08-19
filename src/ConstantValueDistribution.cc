// C C++ includes
#include <map>
#include <string.h>
// KeyValueClusterPerf includes
#include "ValueDistribution.h"
#include "ConstantValueDistribution.h"

ConstantValueDistribution::ConstantValueDistribution(map<string,string> configuration)
{
	// Read in the size of the values
	size = strtol(configuration["size"].c_str(), NULL, 10) + 1;
	// create a buffer to store the values and fill it
	valueBuffer = new char[size];
	memset(valueBuffer, 'X', size-1);
	// make the buffer 0-terminated
	valueBuffer[size-1]=0;
	// create a string of the buffer
	valueString = new string(valueBuffer);
}

ConstantValueDistribution::~ConstantValueDistribution()
{
	delete valueBuffer;
	delete valueString;
}

string* ConstantValueDistribution::getNext()
{
	return valueString;
}