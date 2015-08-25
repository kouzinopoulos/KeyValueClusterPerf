// C C++ includes
#include <map>
#include <sstream>
#include <string>
// external library: ZMQ
#include <zmq.hpp>
// KeyValueClusterPerf includes
#include "logger.h"
#include "ConfigurationManager.h"
#include "SimulationWorker.h"
#include "Simulator.h"

SimulationWorker::SimulationWorker(string db, string ap, string vd, bool skipInit)
{
	connectionOpen = false;
	//databaseCfgPath = db;
	//accessPatternCfgPath = ap;
	databaseCfgPath = "database.cfg";
	accessPatternCfgPath = "accessPattern.cfg";
	valueDistributionCfgPath = "valueDistribution.cfg";
	state=START;
	skipInitialisation=skipInit;
}

SimulationWorker::~SimulationWorker()
{
	// If there are open connections when destroying the worker, safely close them
	if(connectionOpen)
	{
		closeConnection();
	}

}

void SimulationWorker::openConnection(int portNum, int dataportNum)
{
		// Open a command socket
		commandContext = new zmq::context_t (1);
		commandSocket = new zmq::socket_t (*commandContext, ZMQ_PAIR);
		if(portNum==-1)
		{
			portNumber=5555;
		}
		else
		{
			portNumber=portNum;
		}
		if(dataportNum==-1)
		{
			dataportNumber=5554;
		}
		else
		{
			dataportNumber=dataportNum;
		}
		connectionOpen = true;
}

void SimulationWorker::closeConnection()
{
	delete commandSocket;
	delete commandContext;

	connectionOpen = false;
}

void SimulationWorker::listen()
{
	// bind the command socket
	stringstream ss;
	ss << "tcp://*:" << portNumber;
	LOG_DEBUG(ss.str());
	commandSocket->bind(ss.str());
	LOG_DEBUG("Socket bound");
	
	// listen for commands form the controller
	while((state!=EXIT)&&(state!=ERROR))
	{
		zmq::message_t request;
		commandSocket->recv(&request);
		LOG_DEBUG("Received request");
		string requestStr = string(static_cast<char*>(request.data()),request.size());

		zmq::message_t* reply;

		if (requestStr.compare("INIT") == 0)
		{
			initialiseSimulator();
			reply = new zmq::message_t(8);
			memcpy ((void *) reply->data (), "INITDONE", 8);
		}
		else if(requestStr.compare("GO")== 0)
		{
			// request to start simulation received
			// Burn in
			LOG_DEBUG("Burning in");
			simulator->burnInOut(75);
			// Run simulation
			LOG_DEBUG("Simulating");
			runSimulation();
			// Burn out
			LOG_DEBUG("Burning out");
			simulator->burnInOut(75);
			// Simulation finished change state to result
			state=RESULTS;
			reply = new zmq::message_t(12);
			memcpy ((void *) reply->data (), "RESULTSREADY", 12);
		}
		else if(requestStr.compare("GETRESULTS")==0)
		{
			// Report back the results from the simulator
			LOG_DEBUG("Reporting results");
			map<string, string> results = simulator->getResults();
			// Open up a data socket
			void *dataContext = zmq_ctx_new();
			void *dataSocket = zmq_socket(dataContext, ZMQ_PAIR);
			ss.clear();
			ss.str(string());
			ss << "tcp://*:" << dataportNumber;
			LOG_DEBUG(ss.str());
			int rc = zmq_bind(dataSocket, ss.str().c_str());
			// Buffer to store data in
			char buffer[1024];
			// Generate the data to send
			ConfigurationManager cm;
			string resultsData = cm.writeString(results);
			strcpy(buffer, resultsData.c_str());
			buffer[sizeof(buffer) - 1] = 0;
			// Send the data over the socket
			rc = zmq_send(dataSocket, buffer, 1024,0);
			// Deinitialise the simulator
			deinitialiseSimulator();
			// Reply that we are done sending results
			reply = new zmq::message_t(11);
			memcpy ((void *) reply->data (), "DONERESULTS", 11);
			zmq_close(dataSocket);
			zmq_ctx_destroy(dataContext);
		}
		else if(requestStr.compare("RESTART")==0)
		{
			// Restarting the simulator state
			LOG_DEBUG("Restart simulator state");
			state=START;
			reply = new zmq::message_t(10);
			memcpy ((void *) reply->data (), "RESTARTING", 10);
		}
		else if(requestStr.compare("EXIT")==0)
		{
			LOG_DEBUG("Exiting");
			state=EXIT;
			reply = new zmq::message_t(7);
			memcpy ((void *) reply->data (), "EXITING", 7);
		}

		// Send out the reply message
		commandSocket->send(*reply);
		delete reply;
	}
	if(state==ERROR)
	{
		LOG_DEBUG("Exited due to error");
	}
}

void SimulationWorker::initialiseSimulator()
{
	ConfigurationManager cm;
	// Create database configuration
	map<string,string> databaseConfiguration = cm.readFile(databaseCfgPath);
	// Create accessPattern configuration
	map<string,string> accessPatternConfiguration = cm.readFile(accessPatternCfgPath);
	// Create valueDistribution configuration
	map<string, string> valueDistribution = cm.readFile(valueDistributionCfgPath);
	// Create the simulator
	simulator = new Simulator(databaseConfiguration, accessPatternConfiguration, valueDistribution, skipInitialisation);
}

void SimulationWorker::deinitialiseSimulator()
{
	delete simulator;
}

void SimulationWorker::runSimulation()
{
	simulator->simulate(250);
}