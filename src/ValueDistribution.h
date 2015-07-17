#ifndef __VALUEDISTRIBUTION_H__
#define __VALUEDISTRIBUTION_H__

#include <string>

using namespace std;

class ValueDistribution
{
	public:
		virtual ~ValueDistribution() {};

		virtual string* getNext() = 0;					// Get next value size in bytes
};

#endif