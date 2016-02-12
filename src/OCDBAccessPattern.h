#ifndef __OCDBACCESSPATTERN_H__
#define __OCDBACCESSPATTERN_H__

#include <map>
#include <string>

#include "AccessPattern.h"
#include "ConfigurationManager.h"

using namespace std;

/**
 * Access pattern representing randomness in the keys and the read-write cycles
 */
class OCDBAccessPattern : public AccessPattern {
public:
  OCDBAccessPattern(Configuration* _config);
  ~OCDBAccessPattern();

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
//__OCDBACCESSPATTERN_H__
