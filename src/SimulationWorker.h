#ifndef __SIMULATIONWORKER_H__
#define __SIMULATIONWORKER_H__

#include <string>

#include <zmq.hpp>

#include "Simulator.h"

using namespace std;

enum SimulatorState{START, RESULTS, EXIT, ERROR, DONE};

class SimulationWorker
{
	public:
		SimulationWorker(string databaseCfgPath, string accessPatternCfgPath, string valueDistributionCfgPath);
		~SimulationWorker();

		void openConnection();			// Opens a port so that the controller can connect to it
		void closeConnection();			// Closes everything
		void listen();					// Listen for commands and execute

		void initialiseSimulator();		// Create a simulationobject
		void deinitialiseSimulator();	// Delete simulation object, only call when all hosts finished

	private:
		SimulatorState state;
		string databaseCfgPath;
		string accessPatternCfgPath;
		string valueDistributionCfgPath;
		bool connectionOpen;		// indicates if a connection is open

		zmq::context_t* commandContext;
		zmq::socket_t* commandSocket;

		Simulator* simulator;

		void runSimulation();
};

#endif