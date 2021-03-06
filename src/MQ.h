#ifndef __MQ_H__
#define __MQ_H__

#include <iostream>
#include <list>
#include <string>
#include <vector>
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
  int send(char* buffer, int size);

  /*! Receives size bytes of buffer over socket */
  void receive(char*& buffer, int size);

  /*! Receives a char* buffer over socket */
  void receive(char*& buffer);

  /*! Receives an std::string over socket */
  std::string receive();

  /*! Receives an std::vector<char> over socket */
  void receive(std::vector<char>* vector);

  /*! Returns a pointer to the ZMQ context */
  void* getContext();

  /*! Opens a new ZMQ socket of the given type */
  void openSocket(int type);

  /*! Closes a ZMQ socket */
  void closeSocket();

  /*! Bind a ZMQ socket to a given address */
  bool bind(const string& address);

  /*! Connect a ZMQ socket to a given address */
  void connect(const std::string& address);

  /*! Destroy the ZMQ context associated with the MQ object */
  void destroy();

private:
  /*! The ZMQ context */
  void* mContext;

  /*! A ZMQ socket */
  void* mSocket;

  /*! List of ZMQ sockets */
  list<void*> mSockets;

};

#endif
//__MQ_H__
