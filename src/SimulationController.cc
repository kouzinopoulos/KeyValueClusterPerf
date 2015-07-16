#include "SimulationController.h"

#include <map>
#include <list>
#include <sstream>
#include <string>
#include "errno.h"

#include <zmq.hpp>

#include "ConfigurationManager.h"
#include "logger.h"
#include "Simulator.h"

SimulationController::SimulationController(string hostFilePath)
{
	connected = false;

	ConfigurationManager cm;
	list<string> hostNames = cm.readHostFile(hostFilePath);
	stringstream ss;
	for(list<string>::iterator it = hostNames.begin(); it != hostNames.end(); it++)
	{
		ss << "tcp://" << *it;
		hosts.push_back(ss.str());
		ss.str(string());
	}
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
		commandContext = new zmq::context_t(1);
		stringstream ss;
		for(list<string>::iterator it = hosts.begin(); it != hosts.end(); it++)
		{
			zmq::socket_t* socket = new zmq::socket_t(*commandContext, ZMQ_PAIR);
			ss << *it << ":5555";
			socket->connect(ss.str());
			LOG_DEBUG(ss.str());
			ss.str(string());
			commandSockets.push_back(socket);
		}
}

void SimulationController::disconnect()
{
	delete commandContext;
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		delete socket;
	}
	connected = false;
}

void SimulationController::execute()
{
	list<map<string,string>> allResults;
	// All nodes start in the start state;
	// Send GO command to all nodes
	int socketnum=1;
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
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
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
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

	// Get the results back	
	socketnum=1;
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
	{
		zmq::socket_t* socket = *it;

		zmq::message_t request(10);
		memcpy ((void *) request.data (), "GETRESULTS", 10);

		stringstream ss;
		ss << "Sending GETRESULTS to: " << socketnum;
		LOG_DEBUG(ss.str());

		socket->send (request);

		// Connect to the datasocket (this is because of the setup of zeromq)
		void* dataContext = zmq_ctx_new();
		void* dataSocket = zmq_socket(dataContext, ZMQ_PAIR);
		stringstream sshost;
		sshost << hosts.front() << ":5554";
		string hostlocation = sshost.str();
		int rc = zmq_connect(dataSocket, hostlocation.c_str());
		hosts.pop_front();
		// Receive the data
		char buffer[1024];
		int num=zmq_recv(dataSocket, buffer, 1024,0);
		// Check if we have received the data
		if(num>0)
		{
			buffer[num]='\0';
			printf("receiver (%s)\n", buffer);
		}
		else{
			LOG_DEBUG("call failed");
			int err = errno;
			printf("ERRNO: %d\n", err);
		}
		ConfigurationManager cm;
		map<string, string> results = cm.readString(string(buffer));
		allResults.push_back(results);
		socketnum++;
	}

	// All nodes should reply with RESULTSDONE
	socketnum=1;
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
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
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
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
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
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

	LOG_DEBUG("calling merge");
	Simulator sim;
	sim.mergeResults(allResults, "results.csv");
	LOG_DEBUG("Getting here");
}