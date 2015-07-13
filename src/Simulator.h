#include <list>
#include <map>
#include <time.h>

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
		map<string, string> getResults();
		void mergeResults(list<map<string, string>> results);	// Merge results from other other simulations

	private:
		KeyValueDB* keyValueDB;				// The keyValueDB to use
		AccessPattern* accessPattern;		// Access pattern to simulate
		map<string, string> results;		// Results from the simulation

		double calculateDurationMicroseconds(struct timespec start, struct timespec stop);
};