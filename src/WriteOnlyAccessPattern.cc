// C C++ includes
#include <stdlib.h>
#include <sstream>
// Unix OS includes
#include <time.h>
// KeyValueClusterPerf inlcudes
#include "logger.h"
#include "WriteOnlyAccessPattern.h"

WriteOnlyAccessPattern::WriteOnlyAccessPattern(Configuration* _config)
{
  // Read in range of key values to be used in base 10
  minKey = _config->accessPatternMinKey;
  maxKey = _config->accessPatternMaxKey;

  // initialise random seed, this could also be a configuration to make it deterministic
  srand(time(NULL));
}

WriteOnlyAccessPattern::~WriteOnlyAccessPattern()
{
  // Nothing to delete
}

SingleAccess WriteOnlyAccessPattern::getNext()
{
  // Determine the key to be written
  int key = rand() % (maxKey - minKey) + minKey;
  stringstream ss;
  ss << key;
  // Return the generated values in a struct
  SingleAccess nextSingleAccess = { false, ss.str(), valueDistribution->getNext() };
  return nextSingleAccess;
}

map<string, string> WriteOnlyAccessPattern::getInitialisationKeyValuePairs()
{
  map<string, string> keyValuePairs;
  // Initialise with strings and every possible key (can be improved in future)
  stringstream ss;
  for (int i = minKey; i <= maxKey; i++) {
    ss << i;
    keyValuePairs[ss.str()] = *valueDistribution->getNext();
    ss.str(string()); // clear stringstream
  }
  return keyValuePairs;
}
