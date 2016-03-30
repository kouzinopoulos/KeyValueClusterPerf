#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include <time.h>
#include <unistd.h>

#include "boost/filesystem.hpp"

#include "AliCDBEntry.h"
#include "TBuffer.h"
#include "TBufferFile.h"
#include "TClass.h"
#include "TFile.h"

#include "request.pb.h"
#include <google/protobuf/stubs/common.h>

#include "ConstantValueDistribution.h"
#include "DBConnector.h"
#include "DummyKeyValueDB.h"
#include "logger.h"
#include "OCDBAccessPattern.h"
#include "RamCloudKeyValueDB.h"
#include "RandomAccessPattern.h"
#include "ReadOnlyAccessPattern.h"
#include "RiakJavaKeyValueDB.h"
#include "ValueDistribution.h"
#include "WriteOnlyAccessPattern.h"

DBConnector::DBConnector(Configuration* _config)
{
  keyValueDB = new RiakJavaKeyValueDB(_config);
  accessPattern = new OCDBAccessPattern(_config);
  valueDistribution = new ConstantValueDistribution(_config);

  accessPattern->setValueDistribution(valueDistribution);

  mBytesLimit = _config->bytesLimit;

  // Connect to broker
  mq.openSocket(ZMQ_REQ);

  stringstream ss;
  ss << _config->databaseBroker;

  mq.connect(ss.str());

  // Metrics initialization
  mLoadedObjects = 0;
  mTotalDataSize = 0;
}

DBConnector::~DBConnector()
{
  if (accessPattern != NULL) {
    delete accessPattern;
  }
  if (keyValueDB != NULL) {
    delete keyValueDB;
  }
  if (valueDistribution != NULL) {
    delete valueDistribution;
  }

  mq.closeSocket();
  mq.destroy();

  google::protobuf::ShutdownProtobufLibrary();
}

std::vector<char> DBConnector::load(const char* path)
{
  TFile *f = new TFile(path);

  // Get the AliCDBEntry from the root file
  AliCDBEntry *en = (AliCDBEntry*)f->Get("AliCDBEntry");

  // Create an outcoming buffer
  TBufferFile *buf = new TBufferFile(TBuffer::kWrite);

  // Stream and serialize the AliCDBEntry object to the buffer
  buf->WriteObject(en);

  // Obtain a pointer to the buffer
  char *my = buf->Buffer();

  // Create and return a *copy* of the data in an std::vector
  std::vector<char> vector(my, my + buf->Length());

  mLoadedObjects++;
  mTotalDataSize += buf->Length();

  // Release the open file
  delete f;

  return vector;
}

void DBConnector::create(std::vector<char> value)
{
  // Create an incoming buffer
  TBufferFile *buf = new TBufferFile(TBuffer::kWrite);

  // Write the contents of the char pointer to the incoming buffer
  buf->WriteBuf(reinterpret_cast<char*> (&value[0]), value.size());

  // Change the buffer mode to read
  buf->SetReadMode();

  // Rewind the buffer pointer to the beginning (This works like a file pointer. In case of a TMessage the pointer should be rewinded to position 8 instead of 0)
  buf->SetBufferOffset(0);

  // Cast the object contained in the incoming buffer to an AliCDBEntry
  AliCDBEntry *en = (AliCDBEntry*)buf->ReadObject(AliCDBEntry::Class());

  // Debug
  //en->Print();
}

void DBConnector::putValue(std::string key, std::vector<char> value)
{
  //Serialize buffer
  messaging::RequestMessage* message = new messaging::RequestMessage;
  message->set_command("PUT");
  message->set_key(key);
  message->set_value(&value[0], value.size());

  // Serialize the message to a binary buffer and send it
  int messageSize = message->ByteSize();

  void *serialBuffer = malloc(messageSize);
  message->SerializeToArray(serialBuffer, messageSize);

  mq.send((char *)serialBuffer, messageSize);

  // Receive message as a serialized string and de-serialize it
  std::string repString = mq.receive();

  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromString(repString);

  if (msgReply->command().compare("OK") != 0) {
    LOG_DEBUG(msgReply->error());
  }

  free(serialBuffer);

  delete message;
  delete msgReply;
}

std::vector<char> DBConnector::getValue(std::string key)
{
  messaging::RequestMessage* message = new messaging::RequestMessage;
  message->set_command("GET");
  message->set_key(key);

  // Serialize the message to a string
  std::string serialString;
  message->SerializeToString(&serialString);

  // Send the command/key message
  mq.send((char *)serialString.c_str(), serialString.length());

  // Receive the data payload
  std::vector<char> dataVectorIn;
  mq.receive(&dataVectorIn);

  // convert to actual message
  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromArray(reinterpret_cast<char*>(dataVectorIn.data()), dataVectorIn.size());

  // check the replyCommand
  if (msgReply->command().compare("OK") != 0) {
    cerr << msgReply->error() << endl;
  }

  // Create and return a *copy* of the data in an std::vector
  std::vector<char> vector (msgReply->value().c_str(), msgReply->value().c_str() + msgReply->value().size());

  delete message;
  delete msgReply;

  return vector;
}

void DBConnector::run()
{
  boost::filesystem::path dataPath("/root/charis/OCDB/");
  boost::filesystem::recursive_directory_iterator endIterator;

  // Traverse the filesystem and load each root file found
  if (boost::filesystem::exists(dataPath) && boost::filesystem::is_directory(dataPath)) {
    for (boost::filesystem::recursive_directory_iterator directoryIterator(dataPath); directoryIterator != endIterator;
         ++directoryIterator) {
      if (boost::filesystem::is_regular_file(directoryIterator->status())) {

        // Trim the directory structure from the file name
        std::size_t pos = directoryIterator->path().string().rfind("/");
        std::string str = directoryIterator->path().string().substr (pos + 1);

        // Trim the file extension from the file name. The resulting string is the DB key
        std::size_t pos2 = str.rfind(".");
        std::string key = str.substr(0, pos2);
        //cout << key << endl;

        // Load the data as value using the ROOT libraries
        std::vector<char> dataVectorOut = load(directoryIterator->path().c_str());

        // Measure the time of the put/get cycle
        boost::posix_time::ptime before = boost::posix_time::microsec_clock::local_time();

        putValue(key, dataVectorOut);

        std::vector<char> value = getValue(key);

        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

        mDuration += (now - before);

        create(value);

        // Stop execution if we transmitted more bytes than the user specified limit
        if (mBytesLimit != -1 && mTotalDataSize > mBytesLimit) {
          break;
        }
      }
    }
  } else {
    std::cerr << "Path " << dataPath.string() << "/ could not be found or does not contain any valid data files"
              << endl;
  }

  cout << "Objects loaded: " << mLoadedObjects << " Total size: " << mTotalDataSize << " B " << mTotalDataSize/(1024) << " KB " << mTotalDataSize/(1024*1024) << " MB " << endl;

  cout << "Communication latency: "<< mDuration.total_microseconds() << "µs "
      << mDuration.total_milliseconds() << "ms " << mDuration.total_seconds()
     << "s." << endl;
}

// some code duplication to prevent simulation function from being slowed down
void DBConnector::burnInOut(int runs)
{
  for (int i = 0; i < runs; i++) {
    SingleAccess singleAccess = accessPattern->getNext();
    if (singleAccess.read == true) {
      keyValueDB->getValue(singleAccess.key);
    } else {
      keyValueDB->putValue(singleAccess.key, singleAccess.value);
    }
  }
}

map<string, string> DBConnector::getResults()
{
  stringstream ss;
  map<string, string> results;

  // Add hostname so that origin can be identified
  char* hostname = new char[256];
  gethostname(hostname, 256);
  results["hostname"] = string(hostname);
  delete[] hostname;

  // Add number of loaded objects
  ss << mLoadedObjects;
  results["objects"] = ss.str();
  ss.str(string());

  // Add total size of loaded objects
  ss << mTotalDataSize;
  results["size"] = ss.str();
  ss.str(string());

  // Add total runtime of actual simulation
  ss << mDuration;
  results["duration"] = ss.str();
  ss.str(string());

  return results;
}
