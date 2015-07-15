#include "SimulationWorker.h"

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
		context = new zmq::context_t (1);
		socket = new zmq::socket_t (*context, ZMQ_REP);

		connectionOpen = true;
}

void SimulationWorker::closeConnection()
{
	delete socket;
	delete context;

	connectionOpen = false;
}

void SimulationWorker::listen()
{
	socket->bind("tcp://*:5555");
	/*while(true)
	{
		zmq::message_t request;

		socket->recv(&request);
		LOG_DEBUG("Received request");

		runSimulation();

		zmq::message_t reply;
		memcpy ((void *) reply.data (), "World", 5);
		socket->send(reply);
	}*/
	while((state!=EXIT)&&(state!=ERROR))
	{
		zmq::message_t request;
		socket->recv(&request);
		LOG_DEBUG("Received request");
		string requestStr = string(static_cast<char*>(request.data()),request.size());

		zmq::message_t* reply;

		if(requestStr.compare("GO")==0)
		{
			// request to start simulation received
			// Burn in
			initialiseSimulator();
			// TODO
			// Run simulation
			runSimulation();
			// Burn out
			// TODO
			
			// Simulation finished change state to result
			state=RESULTS;
			reply = new zmq::message_t(12);
			memcpy ((void *) reply->data (), "RESULTSREADY", 12);
		}
		else if(requestStr.compare("GETRESULTS")==0)
		{
			// Report back the results from the simulator
			// TODO
			LOG_DEBUG("Reporting results");
			deinitialiseSimulator();
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
		socket->send(*reply);
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