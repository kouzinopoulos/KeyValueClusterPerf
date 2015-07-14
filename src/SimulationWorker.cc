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
	while(true)
	{
		zmq::message_t request;

		socket->recv(&request);
		LOG_DEBUG("Received HELLO");

		runSimulation();

		zmq::message_t reply(5);
		memcpy ((void *) reply.data (), "World", 5);
		socket->send(reply);
	}
}

void SimulationWorker::runSimulation()
{
	// create a configuration manager
	LOG_DEBUG("Running simulation");
	ConfigurationManager cm;
	// Create database configuration
	LOG_DEBUG("Read db config");
	map<string,string> databaseConfiguration = cm.readFile(databaseCfgPath);
	// Create accessPattern configuration
	LOG_DEBUG("Read ap config");
	map<string,string> accessPatternConfiguration = cm.readFile(accessPatternCfgPath);
	// Create the simulator
	LOG_DEBUG("Build simulator");
	Simulator* simulator = new Simulator(databaseConfiguration, accessPatternConfiguration);
	// Perform simulation
	LOG_DEBUG("simulate");
	simulator->simulate(10000);
	// Perform another simulation
	LOG_DEBUG("delete simulator");
	delete simulator;
	LOG_DEBUG("recreate configuration");
	accessPatternConfiguration["accessPatternType"]="ReadOnly";
	simulator = new Simulator(databaseConfiguration, accessPatternConfiguration);
	simulator->simulate(10000);
	// Destroy the simulator
	delete simulator;
	LOG_DEBUG("end of run");
}