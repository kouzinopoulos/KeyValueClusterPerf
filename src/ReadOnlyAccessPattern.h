#ifndef __READONLYACCESSPATTERN_H__
#define __READONLYACCESSPATTERN_H__

#include <map>
#include <string>

#include "AccessPattern.h"
#include "ConfigurationManager.h"

using namespace std;

/**
 * Access pattern performing only reads
 */
class ReadOnlyAccessPattern : public AccessPattern {
public:
  ReadOnlyAccessPattern(Configuration* _config);
  ~ReadOnlyAccessPattern();

  SingleAccess getNext();
  map<string, string> getInitialisationKeyValuePairs();

private:
  /*! lowest integer value representing a valid key */
  int minKey;
  /*! highest integer value representing a valid key */
  int maxKey;
};

#endif
//__READONLYACCESSPATTERN_H__
