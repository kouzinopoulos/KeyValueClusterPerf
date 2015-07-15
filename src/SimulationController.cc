#include "SimulationController.h"

#include <list>
#include <sstream>

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
		zmq::socket_t* socket = new zmq::socket_t(*context, ZMQ_REQ);
		zmq::socket_t* socket2 = new zmq::socket_t(*context, ZMQ_REQ);
		connected = true;

		LOG_DEBUG("connecting to worker 003");
		socket->connect("tcp://cernvmbl003:5555");

		LOG_DEBUG("connecting to worker 005");
		socket2->connect("tcp://cernvmbl005:5555");

		sockets.push_back(socket);
		sockets.push_back(socket2);
}

void SimulationController::disconnect()
{
	delete context;
	for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		delete socket;
	}
	connected = false;
}

void SimulationController::execute()
{
	/*for(int request_nbr = 0; request_nbr != 1; request_nbr++)
	{
		int socketnum = 1;
		for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
		{
			zmq::socket_t* socket = *it;
			zmq::message_t request;
			memcpy ((void *) request.data (), "Hello", 5);
			LOG_DEBUG("Socket");
			stringstream ss;
			ss << socketnum;
			LOG_DEBUG(ss.str());
			LOG_DEBUG("Sending Hello");
			socket->send (request);
			socketnum++;
		}
		for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
		{
			zmq::socket_t* socket = *it;
			zmq::message_t reply;
			socket->recv(&reply);
			LOG_DEBUG("received world");
		}
	}*/
	// All nodes start in the start state;
	// Send GO command to all nodes
	int socketnum=1;
	for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
	{
		zmq::socket_t* socket = *it;

		zmq::message_t request(2);
		memcpy ((void *) request.data (), "GO", 2);

		stringstream ss;
		ss << "Sending GO to: " << socketnum;
		LOG_DEBUG(ss.str());

		socket->send (request);
		socketnum++;
	}
	// All nodes should reply with RESULTSREADY
	socketnum=1;
	for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		zmq::message_t reply;
		socket->recv(&reply);
		string replyStr = string(static_cast<char*>(reply.data()),reply.size());

		stringstream ss;
		ss << "Received reply: " << replyStr << ", from: " << socketnum;
		LOG_DEBUG(ss.str());
		socketnum++;
	}
	// Shut down the nodes
	socketnum=1;
	for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
	{
		zmq::socket_t* socket = *it;

		zmq::message_t request(4);
		memcpy ((void *) request.data (), "EXIT", 4);

		stringstream ss;
		ss << "Sending EXIT to: " << socketnum;
		LOG_DEBUG(ss.str());

		socket->send (request);
		socketnum++;
	}
		// All nodes should reply with EXIT
	socketnum=1;
	for(list<zmq::socket_t*>::iterator it = sockets.begin(); it != sockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		zmq::message_t reply;
		socket->recv(&reply);
		string replyStr = string(static_cast<char*>(reply.data()),reply.size());

		stringstream ss;
		ss << "Received reply: " << replyStr << ", from: " << socketnum;
		LOG_DEBUG(ss.str());
		socketnum++;
	}
}