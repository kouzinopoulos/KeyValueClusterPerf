#include <iostream>
#include <string>

#include <zmq.hpp>

#include "sha1.h"

#include "vmac.h"

#include "MessageSender.h"
#include "logger.h"

MessageSender::MessageSender(string security)
{
  // store security settings (use two boolean values for rapid evaluation)
  sha1Enabled = false;
  vmacEnabled = false;

  if (security.compare("sha1") == 0) {
    LOG_DEBUG("SHA1 enabled");
    sha1Enabled = true;
  }

  if (security.compare("vmac") == 0) {
    LOG_DEBUG("VMAC ENABLED");
    ALIGN(4) unsigned char key[] = "abcdefghijklmnop";
    vmacEnabled = true;
    vmac_set_key(key, &ctx);
  }

  // start zeroMQ
  std::cout << "messenger started" << std::endl;

  mContext = zmq_ctx_new();

  if (mContext == NULL) {
    cout << "failed creating context, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  mSocket = zmq_socket(mContext, ZMQ_REQ);

  if (mSocket == NULL) {
    cout << "Failed creating socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }

  // Connect to the broker node cernvm22
  if (zmq_connect(mSocket, "tcp://137.138.234.128:5559") != 0) {
    cout << "Failed connecting socket, reason: " << zmq_strerror(errno);
    exit(-1);
  }
}

MessageSender::~MessageSender()
{
  if (mSocket != NULL) {

    if (zmq_close(mSocket) != 0) {
      cout << "Failed closing socket, reason: " << zmq_strerror(errno);
    }

    mSocket = NULL;
  }

  if (zmq_ctx_destroy(mContext) != 0) {
    cout << "Failed terminating context, reason: " << zmq_strerror(errno);
  }
}

void MessageSender::send(string message)
{
  // Calculate sha1 of message
  if (sha1Enabled) {
    sha1Hash = sha1(message);
  }

  // calculate vmac of message
  if (vmacEnabled) {
    // could be even more efficient if string data is alligned in memory
    vmacHash =
      vmac((unsigned char*)message.c_str(), message.length(), (unsigned char*)"\0\0\0\0\0\0\0\0bcdefghi", &tagl, &ctx);
  }

  zmq_msg_t request;
  zmq_msg_init_size(&request, message.length());
  memcpy((char*)zmq_msg_data(&request), message.c_str(), message.length());

  int nbytes = zmq_msg_send(&request, mSocket, 0);

  if (nbytes < 0) {
    cout << "Failed sending on socket, reason: " << zmq_strerror(errno);
  }
}

string MessageSender::receive()
{
  zmq_msg_t reply;
  zmq_msg_init(&reply);

  int nbytes = zmq_msg_recv(&reply, mSocket, 0);

  if (nbytes < 0) {
    cout << "Failed receiving on socket, reason: " << zmq_strerror(errno);
  }

  string repstr((char*)zmq_msg_data(&reply), zmq_msg_size(&reply));

  return repstr;
}
