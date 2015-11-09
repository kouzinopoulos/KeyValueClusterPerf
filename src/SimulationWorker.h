#ifndef __SIMULATIONWORKER_H__
#define __SIMULATIONWORKER_H__

// C C++ includes
#include <string>
// KeyValueClusterPerf includes
#include "Simulator.h"
// External library: ZMQ
#include <zmq.hpp>

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
  SimulationWorker(string databaseCfgPath, string accessPatternCfgPath, string valueDistributionCfgPath, bool skipInit);
  ~SimulationWorker();

  /*! Opens a port so that the controller can connect to it */
  void openConnection(int portNum, int dataportNum);
  /*! Close the connection to the controller */
  void closeConnection();
  /*! Listen for commands from the controller */
  void listen();

  /*! Create a simulationobject */
  void initialiseSimulator();
  /*! Delete simulation object, only call when all hosts finished */
  void deinitialiseSimulator();

private:
  SimulatorState state;
  /*! Path to the configuration file for the database */
  string databaseCfgPath;
  /*! Path to the configuration file for the access pattern */
  string accessPatternCfgPath;
  /*! Path to the configuration file for the value distribution */
  string valueDistributionCfgPath;
  /*! Indicate if a connection is open */
  bool connectionOpen;

  /*! ZMQ Context to send commands over */
  zmq::context_t* commandContext;
  /*! ZMQ Socket to send commands over */
  zmq::socket_t* commandSocket;
  /*! Port to receive commands through */
  int portNumber;
  /*! Port to send data through */
  int dataportNumber;

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