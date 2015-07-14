#ifndef __SIMULATIONWORKER_H__
#define __SIMULATIONWORKER_H__

#include <string>

#include <zmq.hpp>

using namespace std;

class SimulationWorker
{
	public:
		SimulationWorker(string databaseCfgPath, string accessPatternCfgPath);
		~SimulationWorker();

		void openConnection();			// Opens a port so that the controller can connect to it
		void closeConnection();			// Closes everything
		void listen();					// Listen for commands and execute

	private:
		string databaseCfgPath;
		string accessPatternCfgPath;
		bool connectionOpen;		// indicates if a connection is open

		zmq::context_t* context;
		zmq::socket_t* socket;

		void runSimulation();
};

#endif