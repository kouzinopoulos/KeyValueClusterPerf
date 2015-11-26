#include <map>
#include <string>
#include <iostream>
#include <string>

#include "request.pb.h"
#include "MessageSender.h"

#include <google/protobuf/stubs/common.h>

#include "logger.h"
#include "RiakJavaKeyValueDB.h"

RiakJavaKeyValueDB::RiakJavaKeyValueDB(Configuration* _config)
{
  // Check if there is a security setting in the database configuration file
  string securityValue = _config->databaseSecurity;

  cout << "Security value specified: " << securityValue << endl;

  // Check if there is an IP specified for the broker in the database configuration file
  std::string brokerIP = _config->databaseBroker;

  cout << "Broker IP specified: " << brokerIP << endl;

  // Create new message sender
  messageSender = new MessageSender(securityValue, brokerIP);
}

RiakJavaKeyValueDB::~RiakJavaKeyValueDB()
{
  delete messageSender;
  google::protobuf::ShutdownProtobufLibrary();
}

void RiakJavaKeyValueDB::putValue(string key, string* value)
{
  // Create PUT message
  messaging::RequestMessage* msg = new messaging::RequestMessage;
  msg->set_command("PUT");
  msg->set_key(key);
  msg->set_value(*value);

  // Serialize the message to a string and send it
  string msgString;
  msg->SerializeToString(&msgString);
  messageSender->send(msgString);

  // Receive message as a serialized string and de-serialize it
  string repString = messageSender->receive();
  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromString(repString);

  // Check the reply for an "OK" command
  if (msgReply->command().compare("OK") != 0) {
    LOG_DEBUG(msgReply->error());
  }

  delete msg;
  delete msgReply;
}

string RiakJavaKeyValueDB::getValue(string key)
{
  // Create GET message
  messaging::RequestMessage* msg = new messaging::RequestMessage;
  msg->set_command("GET");
  msg->set_key(key);

  // Serialize the message to a string
  string getmsg;
  msg->SerializeToString(&getmsg);

  // send the message
  messageSender->send(getmsg);

  // check reply for OK
  string repString = messageSender->receive();
  // convert to actual message
  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromString(repString);
  string replyValue = "";
  // check the replyCommand
  if (msgReply->command().compare("OK") != 0) {
    // There was an error
    LOG_DEBUG(msgReply->error());
  } else {
    // read out the returned value
    replyValue = msgReply->value();
  }

  delete msg;
  delete msgReply;

  return replyValue;
}

void RiakJavaKeyValueDB::deleteValue(string key)
{
  // no delete functionality yet
  LOG_DEBUG("Delete not implemented in RiakJava");
}

void RiakJavaKeyValueDB::initialise(map<string, string> keyValuePairs)
{
  // nothing to do
}
