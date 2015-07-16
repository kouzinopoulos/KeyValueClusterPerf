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
	// All nodes start in the start state;
	// Send GO command to all nodes
	LOG_DEBUG("GO");
	sendAllNodes("GO");
	// All nodes should reply with RESULTSREADY
	LOG_DEBUG("RESULTSREADY");
	getAllNodes("RESULTSREADY");
	// Get the results back	
	list<map<string,string>> allResults;
	int socketnum=1;
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		zmq::message_t request(10);
		memcpy ((void *) request.data (), "GETRESULTS", 10);
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
	LOG_DEBUG("DONERESULTS");
	getAllNodes("DONERESULTS");
	// Shut down the nodes
	LOG_DEBUG("EXIT");
	sendAllNodes("EXIT");
	// All nodes should reply with EXIT
	LOG_DEBUG("EXITING");
	getAllNodes("EXITING");
	// Merge the results and write out to file
	LOG_DEBUG("MERGING");
	Simulator sim;
	sim.mergeResults(allResults, "results.csv");
	LOG_DEBUG("COMPLETE");
}

void SimulationController::sendAllNodes(string command)
{
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
	{
		zmq::socket_t* socket = *it;

		zmq::message_t request(command.length());
		memcpy ((void *) request.data (), command.c_str(), command.length());
		socket->send (request);
	}
}

void SimulationController::getAllNodes(string command)
{
	for(list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++)
	{
		zmq::socket_t* socket = *it;
		zmq::message_t reply;
		socket->recv(&reply);
		string replyStr = string(static_cast<char*>(reply.data()),reply.size());
		if(replyStr.compare(command)!=0)
		{
			// Handle error
			LOG_DEBUG("WRONG REPLY");
		}
	}
}