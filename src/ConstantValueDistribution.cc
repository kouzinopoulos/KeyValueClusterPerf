#include <map>
#include <string.h>

#include "ValueDistribution.h"
#include "ConstantValueDistribution.h"

ConstantValueDistribution::ConstantValueDistribution(map<string,string> configuration)
{
	// Read in the size of the values
	size = strtol(configuration["size"].c_str(), NULL, 10) + 1;
	valueBuffer = new char[size];
	memset(valueBuffer, 'X', size-1);
	valueBuffer[size-1]=0;
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