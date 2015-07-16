#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <list>
#include <map>
#include <time.h>

#include "KeyValueDB.h"
#include "AccessPattern.h"
#include "ValueDistribution.h"

using namespace std;

class Simulator
{
	public:
		Simulator(map<string,string> databaseConfiguration, 
			map<string,string> accessPatternConfiguration,
			map<string,string> valueDistributionConfiguration);
		Simulator();								// Only to be used by controller to call merge
		~Simulator();

		void simulate(int runs);					// start the simulation
		void burnInOut(int runs);
		map<string, string> getResults();
		void mergeResults(list<map<string, string>> results, string csvFilePath);	// Merge results from other other simulations

	private:
		int reads;							// Count the amount of reads and writes performed
		int writes;
		double duration;					// The duration of the simulation

		KeyValueDB* keyValueDB;				// The keyValueDB to use
		AccessPattern* accessPattern;		// Access pattern to simulate
		ValueDistribution* valueDistribution;	// Value distribution to simulate
		map<string, string> results;		// Results from the simulation

		double calculateDurationMicroseconds(struct timespec start, struct timespec stop);
};

#endif