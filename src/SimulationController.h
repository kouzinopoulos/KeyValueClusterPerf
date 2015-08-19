#ifndef __SIMULATIONCONTROLLER_H__
#define __SIMULATIONCONTROLLER_H__

// C C++ includes
#include <list>
#include <string>
// External library: ZMQ
#include <zmq.hpp>

using namespace std;

/**
 * Class handling the control of the entire simulation, 
 * instructing the worker nodes on the next task and merging the results.
 */
class SimulationController
{
	public:
		SimulationController(string hostFilePath, int hostLimit, int simIteration);
		~SimulationController();

		/*! Connect to all hosts specified in the host file */
		void connect();
		/*! Disconnect from the hosts */
		void disconnect();
		/*! Start the simulation by instructing the worker nodes */
		void execute();

	private:
		/*! value indicating if there are active connections */
		bool connected;

		/*! send a command to all connected host nodes */
		void sendAllNodes(string command);
		/*! receive an expected reply from all host nodes */
		void getAllNodes(string command);

		/*! ZMQ context to send commands over */
		zmq::context_t* commandContext;
		/*! ZMQ socket to send commands over */
		list<zmq::socket_t*> commandSockets;
		/*! list of all hosts with their command port */
		list<string> hosts;
		/*! list of all hosts with their data port */
		list<string> dataHosts;

		/*! Current iteration of the simulation */
		int simulationIteration;
};

#endif
//__SIMULATIONCONTROLLER_H__