#ifndef __READONLYACCESSPATTERN_H__
#define __READONLYACCESSPATTERN_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "AccessPattern.h"

using namespace std;

/**
 * Access pattern performing only reads
 */
class ReadOnlyAccessPattern : public AccessPattern
{
	public:
		ReadOnlyAccessPattern(map<string,string> configuration);
		~ReadOnlyAccessPattern();

		SingleAccess getNext();
		map<string,string> getInitialisationKeyValuePairs();

	private:
		/*! lowest integer value representing a valid key */
		int minKey;
		/*! highest integer value representing a valid key */
		int maxKey;
};

#endif
//__READONLYACCESSPATTERN_H__