#include <map>
#include <string>

#include "AccessPattern.h"

using namespace std;

class RandomAccessPattern : public AccessPattern
{
	public:
		RandomAccessPattern(map<string,string> configuration);
		~RandomAccessPattern();

		SingleAccess getNext();
		map<string,string> getInitialisationKeys();

	private:
		int minKey;
		int maxKey;
		double readWriteBorder;		// Contains a border between 0 and 1 indicating which part are reads and which parts are writes
};