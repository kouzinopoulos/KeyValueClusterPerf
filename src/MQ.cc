#include "MQ.h"

MQ::MQ()
{
}

MQ::~MQ()
{
}

std::string MQ::receive(void* socket)
{
  zmq_msg_t message;
  zmq_msg_init(&message);

  int nbytes = zmq_msg_recv(&message, socket, 0);

  if (nbytes < 0) {
    cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
  }

  std::string messageString((char*)zmq_msg_data(&message), zmq_msg_size(&message));

  return messageString;
}

int MQ::send(void* socket, char* buffer, int size)
{
  zmq_msg_t message;
  zmq_msg_init_size(&message, size);
  memcpy((char*)zmq_msg_data(&message), buffer, size);

  int nbytes = zmq_msg_send(&message, socket, 0);

  if (nbytes < 0) {
    cout << "Failed sending on socket, reason: " << zmq_strerror(errno);
  }

  return nbytes;
}
