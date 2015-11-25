#ifndef __SIMULATIONWORKER_H__
#define __SIMULATIONWORKER_H__

#include <string>

#include <zmq.hpp>

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
  SimulationWorker(bool skipInit, int commandPortNumber, int dataPortNumber);
  ~SimulationWorker();

  /*! Listen for commands from the controller */
  void listen();

  /*! Create a simulationobject */
  void initialiseSimulator();
  /*! Delete simulation object, only call when all hosts finished */
  void deinitialiseSimulator();

private:
  SimulatorState state;

  /*! Opens a port so that the controller can connect to it */
  void openCommandConnection();

  /*! Opens a data port and sends the results buffer to the controlelr node */
  void sendData(char* buffer);

  /*! ZMQ Context to send commands over */
  void* mCommandContext;
  /*! ZMQ Socket to send commands over */
  void* mCommandSocket;
  /*! ZMQ Context to send data over */
  void* mDataContext;
  /*! ZMQ Socket to send data over */
  void* mDataSocket;
  /*! Port to receive commands through */
  int mCommandPortNumber;
  /*! Port to send data through */
  int mDataPortNumber;

  /*! indicates wether to do the initialisation step (performance increase), at least one worker should do
   * initialisation */
  bool skipInitialisation;
  /*! Simulator to perform simulation */
  Simulator* simulator;
  /*! Used by the node to start a simulation */
  void runSimulation();
};

#endif
//__SIMULATIONWORKER_H__
