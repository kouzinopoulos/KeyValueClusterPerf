#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>

#include <time.h>
#include <unistd.h>

#include "boost/date_time/posix_time/posix_time.hpp"
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
#include "MQ.h"
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

/*
  // Initialise the keyValueDatabase
  if (_config->initialization) {
    LOG_DEBUG("Initialise kvdb");
    keyValueDB->initialise(accessPattern->getInitialisationKeyValuePairs());
    LOG_DEBUG("Finished init kvdb");
  }
  */

  // Read the OCDB directory
/*  boost::filesystem::path dataPath("/root/charis/OCDB/");
  boost::filesystem::directory_iterator endIterator;

  // Traverse the filesystem and load each root file found
  if (boost::filesystem::exists(dataPath) && boost::filesystem::is_directory(dataPath)) {
    for (boost::filesystem::directory_iterator directoryIterator(dataPath); directoryIterator != endIterator;
         ++directoryIterator) {
      if (boost::filesystem::is_regular_file(directoryIterator->status())) {

        // Trim directory structure
        std::size_t pos = directoryIterator->path().string().rfind("/");
        std::string str = directoryIterator->path().string().substr (pos + 1);

        // Trim file extension and obtain key
        std::size_t pos2 = str.rfind(".");
        std::string key = str.substr(0, pos2);
        cout << key << endl;

        // Load the data as value using the ROOT libraries
        TFile *f = new TFile(directoryIterator->path().c_str());

        cout << "Loaded root file " << key << " with a size of " << f->GetEND() << " bytes" << endl;
      }
    }
  } else {
    std::cerr << "Path " << dataPath.string() << "/ could not be found or does not contain any valid data files"
              << endl;
  }
*/

  // Initialize variables
  reads = 0;
  writes = 0;
}

DBConnector::DBConnector()
{
  accessPattern = NULL;
  keyValueDB = NULL;
  valueDistribution = NULL;
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

  google::protobuf::ShutdownProtobufLibrary();
}

std::vector<char> DBConnector::load(const char* path)
{
  TFile *f = new TFile(path);
  //TFile *f = new TFile("/root/charis/OCDB/Run145289_145289_v3_s0.root");

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

  return vector;
}

void DBConnector::create(const char* value, int size)
{
  // Debug:
  cout << "Creating Root object of size " << size << endl;

  // Create an incoming buffer
  TBufferFile *buf = new TBufferFile(TBuffer::kWrite);

  // Write the contents of the char pointer to the incoming buffer
  buf->WriteBuf((void*)value, size);

  // Change the buffer mode to read
  buf->SetReadMode();

  // Rewind the buffer pointer to the beginning (This works like a file pointer. In case of a TMessage the pointer should be rewinded to position 8 instead of 0)
  buf->SetBufferOffset(0);

  // Cast the object contained in the incoming buffer to an AliCDBEntry
  AliCDBEntry *en = (AliCDBEntry*)buf->ReadObject(AliCDBEntry::Class());

  // Debug
  en->Print();
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

  MQ mq;
  mq.openSocket(ZMQ_REQ);

  stringstream ss;
  ss << "tcp://cernvm14:5559";

  mq.connect(ss.str());
  mq.send((char *)serialBuffer, messageSize);

  // Receive message as a serialized string and de-serialize it
  std::string repString = mq.receive();

  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromString(repString);

  if (msgReply->command().compare("OK") != 0) {
    LOG_DEBUG(msgReply->error());
  }

  mq.closeSocket();
  mq.destroy();

  free(serialBuffer);

  delete message;
  delete msgReply;
}

void DBConnector::getValue(std::string key, const char*& value, int& size)
{
  messaging::RequestMessage* message = new messaging::RequestMessage;
  message->set_command("GET");
  message->set_key(key);

 // Serialize the message to a string
 std::string serialString;
 message->SerializeToString(&serialString);

  stringstream ss;
  ss << "tcp://cernvm14:5559";

  // Connect and send the command/key message
  MQ mq;
  mq.openSocket(ZMQ_REQ);

  mq.connect(ss.str());
  mq.send((char *)serialString.c_str(), serialString.length());

  // Receive the data payload
  std::vector<char> dataVectorIn;
  mq.receive(&dataVectorIn);

  // convert to actual message
  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromArray(reinterpret_cast<char*>(dataVectorIn.data()), dataVectorIn.size());

  // check the replyCommand
  if (msgReply->command().compare("OK") != 0) {
    // There was an error
    cerr << msgReply->error() << endl;
  } else {
    // read out the returned value
    //replyValue = msgReply->value();
    cout << "OK was received" << endl;
  }

  value = msgReply->value().c_str();
  size = msgReply->value().size();

  mq.closeSocket();
  mq.destroy();

  delete message;
  delete msgReply;
}

void DBConnector::run()
{
  boost::posix_time::ptime before = boost::posix_time::microsec_clock::local_time();

  boost::filesystem::path dataPath("/root/charis/OCDB/");
  boost::filesystem::directory_iterator endIterator;

  // Traverse the filesystem and load each root file found
  if (boost::filesystem::exists(dataPath) && boost::filesystem::is_directory(dataPath)) {
    for (boost::filesystem::directory_iterator directoryIterator(dataPath); directoryIterator != endIterator;
         ++directoryIterator) {
      if (boost::filesystem::is_regular_file(directoryIterator->status())) {

        // Trim the directory structure from the file name
        std::size_t pos = directoryIterator->path().string().rfind("/");
        std::string str = directoryIterator->path().string().substr (pos + 1);

        // Trim the file extension from the file name. The resulting string is the DB key
        std::size_t pos2 = str.rfind(".");
        std::string key = str.substr(0, pos2);
        cout << key << endl;

        // Load the data as value using the ROOT libraries
        std::vector<char> dataVectorOut = load(directoryIterator->path().c_str());

        putValue(key, dataVectorOut);

        int valueSize = 0;
        const char* value = NULL;

        getValue(key, value, valueSize);

        create(value, valueSize);
      }
    }
  } else {
    std::cerr << "Path " << dataPath.string() << "/ could not be found or does not contain any valid data files"
              << endl;
  }

  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

  cout << "Communication latency: "<< (now - before).total_microseconds() << "µs "
     << (now - before).total_milliseconds() << "ms " << (now - before).total_seconds()
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
  // Add number of read operations
  ss << reads;
  results["reads"] = ss.str();
  ss.str(string());
  // Add number of write operations
  ss << writes;
  results["writes"] = ss.str();
  ss.str(string());
  // Add total runtime of actual simulation
  ss << duration;
  results["duration"] = ss.str();
  ss.str(string());
  // Maybe also add timestamps later on
  return results;
}

void DBConnector::mergeResults(list<map<string, string>> results, string csvFilePath)
{
  ofstream csvFile(csvFilePath);
  if (csvFile.is_open()) {
    // Add header
    csvFile << "HostName"
            << ","
            << "#reads"
            << ","
            << "#writes"
            << ","
            << "total_duration" << endl;
    // Add results
    for (list<map<string, string>>::iterator it = results.begin(); it != results.end(); it++) {
      map<string, string> hostResults = *it;
      csvFile << hostResults["hostname"] << "," << hostResults["reads"] << "," << hostResults["writes"] << ","
              << hostResults["duration"] << endl;
    }
    csvFile.close();
  }
}
