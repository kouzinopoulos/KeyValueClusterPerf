#ifndef __SIMULATIONCONTROLLER_H__
#define __SIMULATIONCONTROLLER_H__

#include <list>
#include <string>

#include <zmq.hpp>

using namespace std;

/**
 * Class handling the control of the entire simulation,
 * instructing the worker nodes on the next task and merging the results.
 */
class SimulationController {
public:
  SimulationController(Configuration* _config);
  ~SimulationController();

  /*! Connect to all hosts specified in the host file */
  void connect();
  /*! Start the simulation by instructing the worker nodes */
  void execute();

private:
  /*! value indicating if there are active connections */
  bool connected;

  /*! Opens a data port and receives the results buffer from a worker node */
  void receiveDataFromWorker(char*& buffer);

  /*! send a command to all connected host nodes */
  void sendAllNodes(string command);
  /*! receive an expected reply from all host nodes */
  void getAllNodes(string command);

  /*! ZMQ context to send commands over */
  void* mCommandContext;
  /*! ZMQ sockets to send commands over */
  list<void*> mCommandSockets;
    /*! ZMQ Context to receive data over */
  void* mDataContext;
  /*! ZMQ Socket to receive data over */
  void* mDataSocket;

  /*! Pointer to the configuration */
  Configuration* mConfiguration;

  /*! Current iteration of the simulation */
  int simulationIteration;
};

#endif
//__SIMULATIONCONTROLLER_H__
