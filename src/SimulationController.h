#ifndef __SIMULATIONCONTROLLER_H__
#define __SIMULATIONCONTROLLER_H__

#include <list>

#include <zmq.hpp>

using namespace std;

class SimulationController
{
	public:
		SimulationController();
		~SimulationController();

		void connect();
		void disconnect();
		void execute();

	private:
		bool connected;

		zmq::context_t* context;
		list<zmq::socket_t*> sockets;
};

#endif