#ifndef __CONSTANTVALUEDISTRIBUTION_H__
#define __CONSTANTVALUEDISTRIBUTION_H__

// C C++ includes
#include <map>
#include <string>
// KeyValueClusterPerf includes
#include "ValueDistribution.h"

using namespace std;

/**
 * Represents a value distribution with a fixed lenght
 */
class ConstantValueDistribution : public ValueDistribution
{
	public:
		ConstantValueDistribution(map<string,string> configuration);
		~ConstantValueDistribution();

		string* getNext();

	private:
		/*! size of the generated values */
		int size;
		/*! char buffer containing data to be returned when the next value is requested */
		char* valueBuffer;
		/*! string representing the value to be returned */
		string* valueString;
};

#endif
//__CONSTANTVALUEDISTRIBUTION_H__