#ifndef __SIMULATIONCONTROLLER_H__
#define __SIMULATIONCONTROLLER_H__

#include <list>
#include <string>

#include <zmq.hpp>

using namespace std;

class SimulationController
{
	public:
		SimulationController(string hostFilePath);
		~SimulationController();

		void connect();
		void disconnect();
		void execute();

	private:
		bool connected;

		zmq::context_t* commandContext;
		list<zmq::socket_t*> commandSockets;
		list<string> hosts;
};

#endif