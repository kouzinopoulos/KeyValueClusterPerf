#include <map>
#include <list>
#include <sstream>
#include <string>

#include "errno.h"

#include <zmq.hpp>

#include "ConfigurationManager.h"
#include "logger.h"
#include "SimulationController.h"
#include "Simulator.h"

SimulationController::SimulationController(Configuration* _config)
{
  // determine which iteration this is
  this->simulationIteration = simulationIteration;

  mConfiguration = _config;
}

SimulationController::~SimulationController()
{
  // if connected when destroying the controller, call the disconnect function to safely disconnect everything
  if (connected) {
    disconnect();
  }
}

void SimulationController::connect()
{
  // Connect to all commmand sockets
  commandContext = new zmq::context_t(1);
  for (list<string>::iterator it = mConfiguration->commandHosts.begin(); it != mConfiguration->commandHosts.end(); it++) {
    cout << "connecting to host..." << endl;
    zmq::socket_t* socket = new zmq::socket_t(*commandContext, ZMQ_PAIR);
    socket->connect(*it);
    commandSockets.push_back(socket);
  }
}

void SimulationController::disconnect()
{
  // Disconnect from all command sockets
  delete commandContext;
  for (list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++) {
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
  list<map<string, string>> allResults;
  int socketnum = 1;
  for (list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++) {
    zmq::socket_t* socket = *it;
    zmq::message_t request(10);
    memcpy((void*)request.data(), "GETRESULTS", 10);
    socket->send(request);

    // Connect to the datasocket (this is because of the setup of zeromq)
    void* dataContext = zmq_ctx_new();
    void* dataSocket = zmq_socket(dataContext, ZMQ_PAIR);
    stringstream sshost;
    string hostlocation = mConfiguration->dataHosts.front();
    LOG_DEBUG(hostlocation);
    zmq_connect(dataSocket, hostlocation.c_str());
    mConfiguration->dataHosts.pop_front();
    // Receive the data
    char buffer[1024];
    int num = zmq_recv(dataSocket, buffer, 1024, 0);
    // Check if we have received the data
    if (num > 0) {
      buffer[num] = '\0';
    } else {
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
  for (list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++) {
    zmq::socket_t* socket = *it;

    zmq::message_t request(command.length());
    memcpy((void*)request.data(), command.c_str(), command.length());
    socket->send(request);
  }
}

void SimulationController::getAllNodes(string command)
{
  // retrieve a reply string form all nodes
  for (list<zmq::socket_t*>::iterator it = commandSockets.begin(); it != commandSockets.end(); it++) {
    zmq::socket_t* socket = *it;
    zmq::message_t reply;
    socket->recv(&reply);
    string replyStr = string(static_cast<char*>(reply.data()), reply.size());

    cout << "Received reply from worker: " << replyStr << endl;

    if (replyStr.compare(command) != 0) {
      // Handle error
      LOG_DEBUG("WRONG REPLY");
    }
  }
}
