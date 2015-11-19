#include <map>
#include <sstream>
#include <string>

#include <zmq.hpp>

#include "logger.h"
#include "ConfigurationManager.h"
#include "SimulationWorker.h"
#include "Simulator.h"

SimulationWorker::SimulationWorker(string db, string ap, string vd, bool skipInit, int commandPortNumber, int dataPortNumber)
{
  // databaseCfgPath = db;
  // accessPatternCfgPath = ap;
  databaseCfgPath = "database.cfg";
  accessPatternCfgPath = "accessPattern.cfg";
  valueDistributionCfgPath = "valueDistribution.cfg";
  state = START;
  skipInitialisation = skipInit;

  mCommandPortNumber = (commandPortNumber == -1 ? 5555 : commandPortNumber);
  mDataPortNumber = (dataPortNumber == -1 ? 5554 : dataPortNumber);
}

SimulationWorker::~SimulationWorker()
{
  // Close the open command connection
  if (mCommandSocket != NULL) {

    if (zmq_close(mCommandSocket) != 0) {
      cout << "Failed closing socket, reason: " << zmq_strerror(errno);
    }

    mCommandSocket = NULL;
  }

  if (zmq_ctx_destroy(mCommandContext) != 0) {
    cout << "Failed terminating context, reason: " << zmq_strerror(errno);
  }
}

// This method is persistent and called only once, during the initialization of the class
void SimulationWorker::openCommandConnection()
{
  // Open a command socket
  mCommandContext = zmq_ctx_new();

  if (mCommandContext == NULL) {
    cout << "failed creating context, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  mCommandSocket = zmq_socket(mCommandContext, ZMQ_PAIR);

  if (mCommandSocket == NULL) {
    cout << "Failed creating socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  // Open up a socket for the controller to connect to
  stringstream ss;
  ss << "tcp://*:" << mCommandPortNumber;

  if (zmq_bind(mCommandSocket, ss.str().c_str()) != 0) {
    cout << "Failed connecting socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  cout << "Command socket bound on " << ss.str() << endl;
}

// This method can be called again and again from the SimulationWorker::listen method, when the GETRESULTS command is
// received
void SimulationWorker::sendData(char* buffer)
{
  // Open a command socket
  mDataContext = zmq_ctx_new();

  if (mDataContext == NULL) {
    cout << "failed creating context, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  mDataSocket = zmq_socket(mDataContext, ZMQ_PAIR);

  if (mDataSocket == NULL) {
    cout << "Failed creating socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  // Open up a socket for the controller to connect to
  stringstream ss;
  ss << "tcp://*:" << mDataPortNumber;

  if (zmq_bind(mDataSocket, ss.str().c_str()) != 0) {
    cout << "Failed connecting socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  cout << "Data socket bound on " << ss.str() << endl;

  // Send out the data message
  zmq_msg_t reply;
  zmq_msg_init_size(&reply, 1024);
  memcpy((char*)zmq_msg_data(&reply), buffer, 1024);

  int nbytes = zmq_msg_send(&reply, mDataSocket, 0);

  if (nbytes < 0) {
    cout << "Failed sending on socket, reason: " << zmq_strerror(errno);
  }

  cout << "Sent data reply to controller" << endl;

  // Close the open data connection
  if (mDataSocket != NULL) {

    if (zmq_close(mDataSocket) != 0) {
      cout << "Failed closing socket, reason: " << zmq_strerror(errno);
    }

    mDataSocket = NULL;
  }

  if (zmq_ctx_destroy(mDataContext) != 0) {
    cout << "Failed terminating context, reason: " << zmq_strerror(errno);
  }
}

void SimulationWorker::listen()
{
  // Open command connection to controller
  openCommandConnection();

  // Listen for commands form the controller
  while ((state != EXIT) && (state != ERROR)) {
    zmq_msg_t request;
    zmq_msg_init(&request);

    int nbytes = zmq_msg_recv(&request, mCommandSocket, 0);

    if (nbytes < 0) {
      cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
    }

    std::string requestStr((char*)zmq_msg_data(&request), zmq_msg_size(&request));

    cout << "Received request: " << requestStr << endl;

    std::string replyStr;

    if (requestStr.compare("INIT") == 0) {
      // Initialize simulator
      cout << "Initializing simulator" << endl;

      initialiseSimulator();
      replyStr = "INITDONE";

    } else if (requestStr.compare("GO") == 0) {
      // Request to start simulation received

      // Burn in
      cout << "Burning in" << endl;
      simulator->burnInOut(75);

      // Run simulation
      cout << "Simulating" << endl;
      runSimulation();

      // Burn out
      cout << "Burning out" << endl;
      simulator->burnInOut(75);

      // Simulation finished, change state to result
      state = RESULTS;

      replyStr = "RESULTSREADY";

    } else if (requestStr.compare("GETRESULTS") == 0) {
      // Report back the results from the simulator
      cout << "Reporting results" << endl;

      map<string, string> results = simulator->getResults();

      // Buffer to store data in
      char buffer[1024];

      // Generate the data to send
      ConfigurationManager cm;
      string resultsData = cm.writeString(results);
      strcpy(buffer, resultsData.c_str());
      buffer[sizeof(buffer) - 1] = 0;

      // Send the data over the socket
      sendData(buffer);

      // Deinitialise the simulator
      deinitialiseSimulator();

      // Reply that we are done sending results
      replyStr = "DONERESULTS";

    } else if (requestStr.compare("RESTART") == 0) {
      // Restarting the simulator state
      cout << "Restart simulator state" << endl;
      state = START;

      replyStr = "RESTARTING";

    } else if (requestStr.compare("EXIT") == 0) {
      cout << "Exiting" << endl;
      state = EXIT;

      replyStr = "EXITING";
    }

    // Send out the reply message
    zmq_msg_t reply;
    zmq_msg_init_size(&reply, replyStr.length());
    memcpy((char*)zmq_msg_data(&reply), replyStr.c_str(), replyStr.length());

    nbytes = zmq_msg_send(&reply, mCommandSocket, 0);

    if (nbytes < 0) {
      cout << "Failed sending on socket, reason: " << zmq_strerror(errno);
    }

    cout << "Sent command reply to controller" << endl;
  }

  if (state == ERROR) {
    LOG_DEBUG("Exited due to error");
  }
}

void SimulationWorker::initialiseSimulator()
{
  ConfigurationManager cm;
  // Create database configuration
  map<string, string> databaseConfiguration = cm.readFile(databaseCfgPath);
  // Create accessPattern configuration
  map<string, string> accessPatternConfiguration = cm.readFile(accessPatternCfgPath);
  // Create valueDistribution configuration
  map<string, string> valueDistribution = cm.readFile(valueDistributionCfgPath);
  // Create the simulator
  simulator = new Simulator(databaseConfiguration, accessPatternConfiguration, valueDistribution, skipInitialisation);
}

void SimulationWorker::deinitialiseSimulator() { delete simulator; }

void SimulationWorker::runSimulation() { simulator->simulate(250); }
