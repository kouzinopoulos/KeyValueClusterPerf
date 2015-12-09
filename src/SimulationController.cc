#include <map>
#include <list>
#include <sstream>
#include <string>

#include "errno.h"

#include "ConfigurationManager.h"
#include "logger.h"
#include "SimulationController.h"
#include "Simulator.h"

SimulationController::SimulationController(Configuration* _config)
{
  // Determine the iteration
  this->simulationIteration = simulationIteration;

  mConfiguration = _config;
}

SimulationController::~SimulationController()
{
}

void SimulationController::receiveDataFromWorker(char*& buffer)
{
  MQ mq;

  mq.openSocket(ZMQ_PAIR);

  stringstream sshost;
  string hostlocation = mConfiguration->dataHosts.front();

  cout << "Connected to host " << hostlocation << endl;

  mq.connect(hostlocation.c_str());

  mConfiguration->dataHosts.pop_front();

  // Receive the data
  mq.receive(buffer, 1024);

  buffer[1024] = '\0';

  // Close the open data connection
  mq.closeSocket();
}

void SimulationController::execute()
{
  // Connect to all worker nodes
  for (list<string>::iterator it = mConfiguration->commandHosts.begin(); it != mConfiguration->commandHosts.end();
       it++) {

    MQ mq;

    mq.openSocket(ZMQ_PAIR);
    mq.connect((*it).c_str());

    mCommandMQs.push_back(mq);
  }

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
  list<map<string, string>> allResults;

  for (list<MQ>::iterator it = mCommandMQs.begin(); it != mCommandMQs.end(); it++) {
    MQ mq = *it;

    mq.send((char*)"GETRESULTS", 10);

    // Connect to the datasocket (this is because of the setup of zeromq)
    char* buffer = NULL;
    buffer = new char[1024];

    receiveDataFromWorker(buffer);

    ConfigurationManager cm;
    map<string, string> results = cm.readString(string(buffer));
    allResults.push_back(results);

    delete[] buffer;
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

  if (simulationIteration == -1) {
    sim.mergeResults(allResults, "results.csv");
  } else {
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
  for (list<MQ>::iterator it = mCommandMQs.begin(); it != mCommandMQs.end(); it++) {
    MQ mq = *it;

    mq.send((char*)command.c_str(), command.length());
  }
}

void SimulationController::getAllNodes(string command)
{
  for (list<MQ>::iterator it = mCommandMQs.begin(); it != mCommandMQs.end(); it++) {
    MQ mq = *it;

    std::string replyStr = mq.receive();

    cout << "Received reply: " << replyStr << endl;

    if (replyStr.compare(command) != 0) {
      LOG_DEBUG("WRONG REPLY");
    }
  }
}
