// C C++ includes
#include <map>
#include <list>
#include <sstream>
#include <string>
// Unix OS includes
#include "errno.h"
// External library: ZMQ
#include <zmq.hpp>
// KeyValueClusterPerf includes
#include "ConfigurationManager.h"
#include "logger.h"
#include "SimulationController.h"
#include "Simulator.h"

SimulationController::SimulationController(string hostFilePath, int hostLimit, int simIteration)
{
	// determine which iteration this is
	simulationIteration = simIteration;
	// initialise connected state
	connected = false;
	// read in host file data
	ConfigurationManager cm;
	list<string> hostNames = cm.readHostFile(hostFilePath);
	stringstream ss;

	// parse host file
	int numberOfHosts=0;
	for(list<string>::iterator it = hostNames.begin(); it != hostNames.end(); it++)
	{
		// if it starts with '#', it's a comment, ignore it
		if((*it).compare(0,1,"#")!=0)
		{
			// split on ':'
			size_t splitPosition;
			if((splitPosition = (*it).find(":")) != string::npos)
			{
				string host = (*it).substr(0, splitPosition);
				string ports = (*it).substr(splitPosition + 1, string::npos);
				// split on '|'
				if((splitPosition = ports.find("|")) != string::npos)
				{
					// parse a portMin|portMax|dataMin string = a|b|c
					string a = ports.substr(0, splitPosition);
					string bc = ports.substr(splitPosition + 1, string::npos);
					// split on '|'
					if((splitPosition = bc.find("|")) != string::npos)
					{
						string b = bc.substr(0, splitPosition);
						string c = bc.substr(splitPosition + 1, string::npos);

						// parse strings to int
						int minPort;
						int maxPort;
						int dataMinPort;

						ss.str(a);
						ss >> minPort;
						ss.clear();
						ss.str(b);
						ss >> maxPort;
						ss.clear();
						ss.str(c);
						ss >> dataMinPort;
						ss.clear();

						ss.str(string());
						// Generate host address and add it to the list
						for(int i=0; i<=(maxPort-minPort); i++)
						{
							if((hostLimit==-1)||(numberOfHosts<hostLimit))
							{
								// generate and store commandhost address
								ss << "tcp://" << host << ":" << (minPort + i);
								hosts.push_back(ss.str());
								LOG_DEBUG(ss.str());
								ss.clear();
								ss.str(string());
								// generate and store datahost address
								ss << "tcp://" << host << ":" << (dataMinPort + i);
								dataHosts.push_back(ss.str());
								ss.clear();

								ss.str(string());

								numberOfHosts++;
							}
						}
					}
				}
			}
		}
	}

}

SimulationController::~SimulationController()
{
	// if connected when destroying the controller, call the disconnect function to safely disconnect everything
	if(connected)
	{
		disconnect();
	}
}

void SimulationController::connect()
{
		// Connect to all commmand sockets
		commandContext = new zmq::context_t(1);
		for(list<string>::iterator it = hosts.begin(); it != hosts.end(); it++)
		{
			zmq::socket_t* socket = new zmq::socket_t(*commandContext, ZMQ_PAIR);
			socket->connect(*it);
			commandSockets.push_back(socket);
		}
}

void SimulationController::disconnect()
{
	// Disconnect from all command sockets
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
	// Let all Nodes initialise the simulator
	LOG_DEBUG("INIT");
	sendAllNodes("INIT");
	// All nodes should reply with INITDONE
	getAllNodes("INITDONE");
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
		string hostlocation = dataHosts.front();
		LOG_DEBUG(hostlocation);
		zmq_connect(dataSocket, hostlocation.c_str());
		dataHosts.pop_front();
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
		// close the socket
		zmq_close(dataSocket);
		zmq_ctx_destroy(dataContext);
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
	if(simulationIteration==-1)
	{
		sim.mergeResults(allResults, "results.csv");
	}
	else
	{
		stringstream ss;
		ss << "results" << simulationIteration << ".csv";
		LOG_DEBUG(ss.str());
		sim.mergeResults(allResults, ss.str());
	}
	LOG_DEBUG("COMPLETE");
}

void SimulationController::sendAllNodes(string command)
{
	// send a command string to all command nodes
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
	// retrieve a reply string form all nodes
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