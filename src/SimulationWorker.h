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
		SimulationWorker(string databaseCfgPath, string accessPatternCfgPath);
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
		bool connectionOpen;		// indicates if a connection is open

		zmq::context_t* context;
		zmq::socket_t* socket;

		Simulator* simulator;

		void runSimulation();
};

#endif