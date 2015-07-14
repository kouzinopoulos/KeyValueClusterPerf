#include "SimulationController.h"

#include <zmq.hpp>

#include "logger.h"

SimulationController::SimulationController()
{
	connected = false;
}

SimulationController::~SimulationController()
{
	if(connected)
	{
		disconnect();
	}
}

void SimulationController::connect()
{
		context = new zmq::context_t(1);
		socket = new zmq::socket_t(*context, ZMQ_REQ);

		connected = true;

		LOG_DEBUG("connecting to worker");
		socket->connect("tcp://cernvmbl031:5555");
}

void SimulationController::disconnect()
{
	delete socket;
	delete context;

	connected = false;
}

void SimulationController::execute()
{
	for(int request_nbr = 0; request_nbr != 10; request_nbr++)
	{
		zmq::message_t request (6);
		memcpy ((void *) request.data (), "Hello", 5);
		LOG_DEBUG("Sending Hello");
		socket->send (request);

		zmq::message_t reply;
		socket->recv(&reply);
		LOG_DEBUG("received world");
	}
}