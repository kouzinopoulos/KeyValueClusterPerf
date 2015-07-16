#include <map>
#include <string>

#include "ValueDistribution.h"

using namespace std;

class ConstantValueDistribution : public ValueDistribution
{
	public:
		ConstantValueDistribution(map<string,string> configuration);
		~ConstantValueDistribution();

		string getNext();

	private:
		int size;
		char* valueBuffer;
};