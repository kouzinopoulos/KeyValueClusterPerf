#ifndef __SIMULATIONCONTROLLER_H__
#define __SIMULATIONCONTROLLER_H__

#include <zmq.hpp>

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
		zmq::socket_t* socket;
};

#endif