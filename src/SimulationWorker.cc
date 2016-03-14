#include <map>
#include <sstream>
#include <string>

#include "logger.h"
#include "ConfigurationManager.h"
#include "MQ.h"
#include "SimulationWorker.h"
#include "Simulator.h"

SimulationWorker::SimulationWorker(Configuration* _config)
{
  state = START;

  mCommandPortNumber = _config->commandPort;
  mDataPortNumber = _config->dataPort;
}

SimulationWorker::~SimulationWorker()
{
}

// This method can be called multiple times from the SimulationWorker::listen
// method, when the GETRESULTS command is received
void SimulationWorker::sendDataToController(char* buffer)
{
  MQ mq;
  mq.openSocket(ZMQ_PAIR);

  // Open up a socket for the controller to connect to
  stringstream ss;
  ss << "tcp://*:" << mDataPortNumber;

  mq.bind(ss.str());

  cout << "Data socket bound on " << ss.str() << endl;

  mq.send(buffer, 1024);

  cout << "Sent data reply to controller" << endl;

  // Close the open data connection
  mq.closeSocket();
  mq.destroy();
}

void SimulationWorker::run(Configuration* _config)
{
  // Open command connection to controller
  MQ mq;

  mq.openSocket(ZMQ_PAIR);

  stringstream ss;
  ss << "tcp://*:" << mCommandPortNumber;

  mq.bind(ss.str());

  cout << "Command socket bound on " << ss.str() << endl;

  // Listen for commands from the controller
  while ((state != EXIT) && (state != ERROR)) {
    std::string requestString = mq.receive();

    cout << "Received request: " << requestString << endl;

    std::string replyString;

    if (requestString.compare("INIT") == 0) {

      cout << "Initializing DB connector" << endl;

      connector = new DBConnector(_config);

      replyString = "INITDONE";

    } else if (requestString.compare("GO") == 0) {

      connector->run();

      state = RESULTS;

      replyString = "RESULTSREADY";

    } else if (requestString.compare("GETRESULTS") == 0) {
      // Report back the results from the database
      cout << "Reporting results" << endl;

      map<string, string> results = connector->getResults();

      // Buffer to store data in
      char buffer[1024];

      // Generate the data to send
      ConfigurationManager cm;
      string resultsData = cm.writeString(results);

      strcpy(buffer, resultsData.c_str());
      buffer[sizeof(buffer) - 1] = 0;

      // Send the data over the socket
      sendDataToController(buffer);

      // Deinitialise the simulator
      delete connector;

      // Reply that we are done sending results
      replyString = "DONERESULTS";

    } else if (requestString.compare("RESTART") == 0) {
      // Restarting the simulator state
      cout << "Restart simulator state" << endl;
      state = START;

      replyString = "RESTARTING";

    } else if (requestString.compare("EXIT") == 0) {
      cout << "Exiting" << endl;
      state = EXIT;

      replyString = "EXITING";
    }

    mq.send((char*)replyString.c_str(), replyString.length());

    cout << "Sent command reply to controller" << endl;
  }

  if (state == ERROR) {
    LOG_DEBUG("Exited due to error");
  }

  // Close the open command connection
  mq.closeSocket();
  mq.destroy();
}
