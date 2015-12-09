#include "MQ.h"

MQ::MQ()
{
  mContext = zmq_ctx_new();
  if (mContext == NULL) {
    cerr << "failed creating context, reason: " << zmq_strerror(errno);
    exit(1);
  }
}

MQ::~MQ()
{
  if (mContext == NULL) {
    return;
  }

  if (zmq_ctx_destroy(mContext) != 0) {
    if (errno == EINTR) {
      cerr << " failed closing context, reason: " << zmq_strerror(errno);
    } else {
      mContext = NULL;
      return;
    }
  }
}

void* MQ::getContext() { return mContext; }

void MQ::openSocket(int type)
{
  mSocket = zmq_socket(mContext, type);

  if (mSocket == NULL) {
    cerr << "Failed creating socket, reason: " << zmq_strerror(errno);
    exit(1);
  }
}

void MQ::closeSocket()
{
  if (mSocket == NULL) {
    return;
  }

  if (zmq_close(mSocket) != 0) {
    cerr << "Failed closing socket, reason: " << zmq_strerror(errno);
  }

  mSocket = NULL;
}

bool MQ::bind(const string& address)
{
  if (zmq_bind(mSocket, address.c_str()) != 0) {
    if (errno == EADDRINUSE) {
      // do not print error in this case, this is handled by FairMQDevice in case no connection could be established
      // after trying a number of random ports from a range.
      return false;
    }
    cerr << "Failed binding socket, reason: " << zmq_strerror(errno);
    return false;
  }
  return true;
}

void MQ::connect(const string& address)
{
  if (zmq_connect(mSocket, address.c_str()) != 0) {
    cerr << "Failed connecting socket, reason: " << zmq_strerror(errno);
    // error here means incorrect configuration. exit if it happens.
    exit(1);
  }
}

int MQ::send(char* buffer, int size)
{
  zmq_msg_t message;
  zmq_msg_init_size(&message, size);
  memcpy((char*)zmq_msg_data(&message), buffer, size);

  int nbytes = zmq_msg_send(&message, mSocket, 0);

  if (nbytes < 0) {
    cout << "Failed sending on socket, reason: " << zmq_strerror(errno);
  }

  return nbytes;
}

std::string MQ::receive()
{
  zmq_msg_t message;
  zmq_msg_init(&message);

  int nbytes = zmq_msg_recv(&message, mSocket, 0);

  if (nbytes < 0) {
    cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
  }

  std::string messageString((char*)zmq_msg_data(&message), zmq_msg_size(&message));

  return messageString;
}

void MQ::receive(char*& buffer, int size)
{
  zmq_msg_t message;
  zmq_msg_init(&message);

  int nbytes = zmq_msg_recv(&message, mSocket, 0);

  if (nbytes < 0) {
    cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
  }

  memcpy(buffer, (char*)zmq_msg_data(&message), size);
}
