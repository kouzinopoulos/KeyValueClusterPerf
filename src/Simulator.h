#include <map>

#include "KeyValueDB.h"
#include "AccessPattern.h"

using namespace std;

class Simulator
{
	public:
		Simulator(map<string,string> databaseConfiguration, 
			map<string,string> accessPatternConfiguration);
		~Simulator();

		void simulate(int runs);					// start the simulation

	private:
		KeyValueDB* keyValueDB;				// The keyValueDB to use
		AccessPattern* accessPattern;		// Access pattern to simulate
};