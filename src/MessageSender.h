#ifndef __MESSAGESENDER_H__
#define __MESSAGESENDER_H__

// C C++ includes
#include <string>
// External library: ZMQ
#include <zmq.hpp>
// External library: vmac
#include "vmac.h"

using namespace std;

// for VMAC
#define ALIGN(n) __attribute__((aligned(n)))

/**
 * Class responsible for all communication between the Java client of Riak and this C program
 */
class MessageSender {
public:
  MessageSender(string security);
  ~MessageSender();

  void send(string message);
  string receive();

  // hash of message, trick compiler into not optimising it away
  string sha1Hash;
  uint64_t vmacHash;

private:
  zmq::context_t* context;
  zmq::socket_t* socket;
  bool sha1Enabled;
  bool vmacEnabled;
  // vmac security
  ALIGN(16) vmac_ctx_t ctx;
  uint64_t tagl;
};

#endif
//__MESSAGESENDER_H__