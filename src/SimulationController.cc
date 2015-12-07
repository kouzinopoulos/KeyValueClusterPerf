#include <map>
#include <list>
#include <sstream>
#include <string>

#include "errno.h"

#include "ConfigurationManager.h"
#include "logger.h"
#include "MQ.h"
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
  // Close the open command connections
  for (list<void*>::iterator it = mCommandSockets.begin(); it != mCommandSockets.end(); it++) {
    void* commandSocket = *it;

    if (zmq_close(commandSocket) != 0) {
      cout << "Failed closing socket, reason: " << zmq_strerror(errno);
    }

    commandSocket = NULL;
  }

  if (zmq_ctx_destroy(mCommandContext) != 0) {
    cout << "Failed terminating context, reason: " << zmq_strerror(errno);
  }
}

void SimulationController::connect()
{
  // Connect to all commmand sockets
  mCommandContext = zmq_ctx_new();

  if (mCommandContext == NULL) {
    cout << "failed creating context, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  for (list<string>::iterator it = mConfiguration->commandHosts.begin(); it != mConfiguration->commandHosts.end();
       it++) {
    cout << "Connecting to host " << *it << endl;

    void* commandSocket = zmq_socket(mCommandContext, ZMQ_PAIR);

    if (commandSocket == NULL) {
      cout << "Failed creating socket, reason: " << zmq_strerror(errno);
      exit(-1);
    }

    if (zmq_connect(commandSocket, (*it).c_str()) != 0) {
      cout << "Failed connecting socket, reason: " << zmq_strerror(errno);
      exit(-1);
    }

    mCommandSockets.push_back(commandSocket);
  }
}

void SimulationController::receiveDataFromWorker(char*& buffer)
{
  void* mContext = zmq_ctx_new();

  if (mContext == NULL) {
    cout << "failed creating context, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  void* mSocket = zmq_socket(mContext, ZMQ_PAIR);

  if (mSocket == NULL) {
    cout << "Failed creating socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  stringstream sshost;
  string hostlocation = mConfiguration->dataHosts.front();

  cout << "Connected to host " << hostlocation << endl;

  if (zmq_connect(mSocket, hostlocation.c_str()) != 0) {
    cout << "Failed connecting socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  mConfiguration->dataHosts.pop_front();

  // Receive the data
  int nbytes = zmq_recv(mSocket, buffer, 1024, 0);

  if (nbytes < 0) {
    cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
  }

  buffer[nbytes] = '\0';

  // Close the socket and destroy the context
  if (zmq_close(mSocket) != 0) {
    cout << "Failed closing socket, reason: " << zmq_strerror(errno);
  }

  mSocket = NULL;

  if (zmq_ctx_destroy(mContext) != 0) {
    cout << "Failed terminating context, reason: " << zmq_strerror(errno);
  }
}

void SimulationController::execute()
{
  // Connect to worker nodes
  connect();

  // Initialize the communication layer
  MQ mq;

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

  for (list<void*>::iterator it = mCommandSockets.begin(); it != mCommandSockets.end(); it++) {
    void* mCommandSocket = *it;

    mq.send(mCommandSocket, (char*)"GETRESULTS", 10);

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
  MQ mq;
  // send a command string to all command nodes
  for (list<void*>::iterator it = mCommandSockets.begin(); it != mCommandSockets.end(); it++) {
    void* mCommandSocket = *it;

    mq.send(mCommandSocket, (char*)command.c_str(), command.length());
  }
}

void SimulationController::getAllNodes(string command)
{
  MQ mq;
  // retrieve a reply string form all nodes
  for (list<void*>::iterator it = mCommandSockets.begin(); it != mCommandSockets.end(); it++) {
    void* mCommandSocket = *it;

    std::string replyStr = mq.receive(mCommandSocket);

    cout << "Received reply: " << replyStr << endl;

    if (replyStr.compare(command) != 0) {
      LOG_DEBUG("WRONG REPLY");
    }
  }
}
