#include <stdlib.h>
#include <sstream>
#include <time.h>

#include "logger.h"
#include "RandomAccessPattern.h"                 



RandomAccessPattern::RandomAccessPattern(map<string,string> configuration)
{
	LOG_DEBUG("Random Pattern Created");
	// Read in range of key values to be used in base 10
	minKey = strtol(configuration["minKey"].c_str(), NULL, 10);
	maxKey = strtol(configuration["maxKey"].c_str(), NULL, 10);
	// Read in read to write ratio
	int readWriteRatio = strtod(configuration["readWriteRatio"].c_str(), NULL);
	readWriteBorder = readWriteRatio / ( 1.0 + readWriteRatio);
	// initialise random seed, this could also be a configuration to make it deterministic
	srand(time(NULL));
}	

RandomAccessPattern::~RandomAccessPattern()
{
	LOG_DEBUG("Random Pattern Removed");
}

SingleAccess RandomAccessPattern::getNext()
{
	//LOG_DEBUG("Get next called");
	// First determine if it's a read or a write
	double readOrWrite=(double) rand() / RAND_MAX;
	bool read = readOrWrite > readWriteBorder ? false : true;
	// Now determine the key to be written
	int key = rand() % (maxKey - minKey) + minKey;
	stringstream ss;
	ss << key;
	// Return the generated values in a struct
	SingleAccess nextSingleAccess = {read, ss.str(), "testValue"};
	return nextSingleAccess;
}
