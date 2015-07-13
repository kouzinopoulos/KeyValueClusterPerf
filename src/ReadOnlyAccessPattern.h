#include <map>
#include <string>

#include "AccessPattern.h"

using namespace std;

class ReadOnlyAccessPattern : public AccessPattern
{
	public:
		ReadOnlyAccessPattern(map<string,string> configuration);
		~ReadOnlyAccessPattern();

		SingleAccess getNext();
		map<string,string> getInitialisationKeyValuePairs();

	private:
		int minKey;
		int maxKey;
};