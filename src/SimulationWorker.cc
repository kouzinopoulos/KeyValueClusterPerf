#include "SimulationWorker.h"

#include <map>
#include <sstream>
#include <string>

#include <zmq.hpp>

#include "logger.h"
#include "ConfigurationManager.h"
#include "Simulator.h"

SimulationWorker::SimulationWorker(string db, string ap)
{
	connectionOpen = false;
	//databaseCfgPath = databaseCfgPath;
	//accessPatternCfgPath = accessPatternCfgPath;
	databaseCfgPath = "database.cfg";
	accessPatternCfgPath = "accessPattern.cfg";
	state=START;
}

SimulationWorker::~SimulationWorker()
{
	if(connectionOpen)
	{
		closeConnection();
	}

}

void SimulationWorker::openConnection()
{
		commandContext = new zmq::context_t (1);
		commandSocket = new zmq::socket_t (*commandContext, ZMQ_PAIR);

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
	commandSocket->bind("tcp://*:5555");

	while((state!=EXIT)&&(state!=ERROR))
	{
		zmq::message_t request;
		commandSocket->recv(&request);
		LOG_DEBUG("Received request");
		string requestStr = string(static_cast<char*>(request.data()),request.size());

		zmq::message_t* reply;

		if(requestStr.compare("GO")==0)
		{
			// request to start simulation received
			// Burn in
			initialiseSimulator();
			LOG_DEBUG("Burning in");
			simulator->burnInOut(10000);
			// Run simulation
			runSimulation();
			// Burn out
			LOG_DEBUG("Burning out");
			simulator->burnInOut(10000);
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
			int rc = zmq_bind(dataSocket, "tcp://*:5554");
			// Buffer to store data in
			char buffer[1024];
			// Generate the data to send
			//sprintf(buffer, "The data is getting through the pipe");
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
		}
		else if(requestStr.compare("RESTART")==0)
		{
			// Restarting the simulator state
			LOG_DEBUG("Restart simulator state");
			state=START;
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
	// Create the simulator
	simulator = new Simulator(databaseConfiguration, accessPatternConfiguration);
}

void SimulationWorker::deinitialiseSimulator()
{
	delete simulator;
}

void SimulationWorker::runSimulation()
{
	// create a configuration manager
	LOG_DEBUG("Running simulation");
	// Perform simulation
	simulator->simulate(10000);
	// Perform another simulation
}