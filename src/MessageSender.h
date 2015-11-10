#ifndef __MESSAGESENDER_H__
#define __MESSAGESENDER_H__

#include <string>

#include <zmq.hpp>

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
  void* mContext;
  void* mSocket;

  bool sha1Enabled;
  bool vmacEnabled;
  // vmac security
  ALIGN(16) vmac_ctx_t ctx;
  uint64_t tagl;
};

#endif
//__MESSAGESENDER_H__
