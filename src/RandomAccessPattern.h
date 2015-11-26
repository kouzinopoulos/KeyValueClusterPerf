#ifndef __RANDOMACCESSPATTERN_H__
#define __RANDOMACCESSPATTERN_H__

#include <map>
#include <string>

#include "AccessPattern.h"
#include "ConfigurationManager.h"

using namespace std;

/**
 * Access pattern representing randomness in the keys and the read-write cycles
 */
class RandomAccessPattern : public AccessPattern {
public:
  RandomAccessPattern(Configuration* _config);
  ~RandomAccessPattern();

  SingleAccess getNext();
  map<string, string> getInitialisationKeyValuePairs();

private:
  /*! lowest integer value representing a valid key */
  int minKey;
  /*! highest integer value representing a valid key */
  int maxKey;
  /*! Contains a border between 0 and 1 indicating which part are reads and which parts are writes */
  double readWriteBorder;
};

#endif
//__RANDOMACCESSPATTERN_H__
