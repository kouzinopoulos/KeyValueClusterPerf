#ifndef __SIMULATIONWORKER_H__
#define __SIMULATIONWORKER_H__

#include <string>

#include <zmq.hpp>

#include "ConfigurationManager.h"
#include "Simulator.h"

using namespace std;

/**
 * Enum representing the state of the current simulation Node
 */
enum SimulatorState { START, RESULTS, EXIT, ERROR, DONE };

/**
 * Worker node performing the tasks according to a fixed pattern determined by the controller node
 */
class SimulationWorker {
public:
  SimulationWorker(Configuration* _config);
  ~SimulationWorker();

  /*! Listen for commands from the controller */
  void run(Configuration* _config);

private:
  SimulatorState state;

  /*! Opens a data port and sends the results buffer to the controller node */
  void sendDataToController(char* buffer);

  /*! Port to receive commands through */
  int mCommandPortNumber;
  /*! Port to send data through */
  int mDataPortNumber;

  /*! Simulator to perform simulation */
  Simulator* simulator;
};

#endif
//__SIMULATIONWORKER_H__
