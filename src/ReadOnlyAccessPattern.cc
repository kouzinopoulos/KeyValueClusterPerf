#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "logger.h"
#include "ReadOnlyAccessPattern.h"                 



ReadOnlyAccessPattern::ReadOnlyAccessPattern(map<string,string> configuration)
{
	// Read in range of key values to be used in base 10
	minKey = strtol(configuration["minKey"].c_str(), NULL, 10);
	maxKey = strtol(configuration["maxKey"].c_str(), NULL, 10);
	// initialise random seed, this could also be a configuration to make it deterministic
	srand(time(NULL));
}	

ReadOnlyAccessPattern::~ReadOnlyAccessPattern()
{
	// Nothing to delete
}

SingleAccess ReadOnlyAccessPattern::getNext()
{
	// Determine the key to be written
	int key = rand() % (maxKey - minKey) + minKey;
	stringstream ss;
	ss << key;
	// Return the generated values in a struct
	SingleAccess nextSingleAccess = {true, ss.str(), valueDistribution->getNext()};
	return nextSingleAccess;
}

map<string,string> ReadOnlyAccessPattern::getInitialisationKeyValuePairs()
{
	map<string, string> keyValuePairs;
	// Initialise with empty strings and every possible key (can be improved in future)
	stringstream ss;
	for(int i=minKey; i <= maxKey; i++)
	{
		ss << i;
		keyValuePairs[ss.str()] = *valueDistribution->getNext();
		ss.str(string());		// clear stringstream
	}
	return keyValuePairs;
}