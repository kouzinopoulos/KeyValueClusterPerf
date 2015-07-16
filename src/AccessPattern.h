#ifndef __ACCESSPATTERN_H__
#define __ACCESSPATTERN_H__

#include <map>
#include <string>

#include "ValueDistribution.h"

using namespace std;

// Define a single access in a struct for speed and for correct clearing of memory
struct SingleAccess {
	bool read;
	string key;
	string value;
};

class AccessPattern
{
	public:
		virtual ~AccessPattern() {};

		virtual SingleAccess getNext() = 0;		// Get next access
		virtual map<string,string> getInitialisationKeyValuePairs() = 0;
		void setValueDistribution(ValueDistribution* valueDistribution);
	protected:
		ValueDistribution* valueDistribution;
};

#endif