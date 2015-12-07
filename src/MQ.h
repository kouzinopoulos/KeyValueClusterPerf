#ifndef __MQ_H__
#define __MQ_H__

#include <iostream>
#include <string>
#include <zmq.hpp>

using namespace std;

/**
 * Represents a value distribution with a fixed lenght
 */
class MQ {
public:
  MQ();
  ~MQ();

  /*! Sends size bytes of buffer over socket */
  int send(void *socket, char *buffer, int size);

  /*! Receives an std::string over socket */
  std::string receive(void *socket);

};

#endif
//__MQ_H__
