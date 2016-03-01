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

void DBConnector::run()
{ /*
  // Initialise timers
  struct timespec timespecStart;
  struct timespec timespecStop;

  boost::posix_time::ptime before = boost::posix_time::microsec_clock::local_time();
  clock_gettime(idMonotonicRaw, &timespecStart);
  // Do the actual simulation
  for (int i = 0; i < runs; i++) {
    SingleAccess singleAccess = accessPattern->getNext();
    if (singleAccess.read == true) {
      reads++;
      keyValueDB->getValue(singleAccess.key);
    } else {
      writes++;
      keyValueDB->putValue(singleAccess.key, singleAccess.value);
    }
  }

  // Get final clock reading and print out difference
  clock_gettime(idMonotonicRaw, &timespecStop);
  boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

  double microsecondsTotal = calculateDurationMicroseconds(timespecStart, timespecStop);
  double microsecondsPerOperation = microsecondsTotal / runs;
  duration = microsecondsTotal;
  stringstream ss;
  ss << microsecondsPerOperation;

  cout << "Time elapsed: " << microsecondsTotal << "μs. Time per run: " << microsecondsTotal / runs << "μs." << endl;
  cout << "Latency for " << runs << " runs: " << (now - before).total_microseconds() << "µs "
       << (now - before).total_milliseconds() << "ms " << (now - before).total_seconds()
       << "s. Latency per run: " << (now - before).total_microseconds() / runs << "µs "
       << (now - before).total_milliseconds() / runs << "ms " << (now - before).total_seconds() / runs << "s." << endl; */


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
        TFile *f = new TFile(directoryIterator->path().c_str());

        cout << "Loaded root file " << key << " with a size of " << f->GetEND() << " bytes" << endl;

        //TFile *f = new TFile("/root/charis/OCDB/Run144998_144998_v2_s0.root");

        // Get the AliCDBEntry from the root file
        AliCDBEntry *en = (AliCDBEntry*)f->Get("AliCDBEntry");

        // Create an outcoming buffer
        TBufferFile *buf = new TBufferFile(TBuffer::kWrite);

        // Stream and serialize the AliCDBEntry object to the buffer
        buf->WriteObject(en);

        // Obtain a pointer to the buffer
        char *my = buf->Buffer();

        // Create a *copy* of the data in an std::vector
        std::vector<char> dataVectorOut(my, my + buf->Length());

        for (int i = 0; i < 1024; i++) {
          cout << dataVectorOut[i];
        }
        cout << endl;

        std::cout << "Created a buffer of size " << buf->Length() << " bytes || " << dataVectorOut.size() << std::endl;

        //SERIALIZE TEST
        //Serialize buffer
        messaging::RequestMessage* msg = new messaging::RequestMessage;
        msg->set_command("PUT");
        msg->set_key(key);
        msg->set_value(&dataVectorOut[0], dataVectorOut.size());

        // Serialize the message to a binary buffer and send it
        int mysize = msg->ByteSize();

        cout << "Protocol buffers reported size: " << mysize << endl;

        void *serialBuffer = malloc(mysize);
        msg->SerializeToArray(serialBuffer, mysize);

        //!TEST

        //SEND TEST
        MQ mq;
        mq.openSocket(ZMQ_REQ);

        stringstream ss;
        ss << "tcp://cernvm14:5559";

        mq.connect(ss.str());
        mq.send((char *)serialBuffer, mysize);

        // Receive message as a serialized string and de-serialize it
        std::string repString = mq.receive();

        messaging::RequestMessage* msgReply = new messaging::RequestMessage;
        msgReply->ParseFromString(repString);

        if (msgReply->command().compare("OK") != 0) {
          LOG_DEBUG(msgReply->error());
        }

        mq.closeSocket();
        mq.destroy();

        delete msg;
        delete msgReply;

        //!TEST

        //RECEIVE TEST

        messaging::RequestMessage* msg2 = new messaging::RequestMessage;
        msg2->set_command("GET");
        msg2->set_key(key);

        // Serialize the message to a string
        std::string getString;
        msg2->SerializeToString(&getString);

        stringstream ss2;
        ss2 << "tcp://cernvm14:5559";

        // Connect and send the command/key message
        MQ mq2;
        mq2.openSocket(ZMQ_REQ);

        mq2.connect(ss.str());
        mq2.send((char *)getString.c_str(), getString.length());

        // Receive the data payload
        std::vector<char> dataVectorIn;
        mq2.receive(&dataVectorIn);

        cout << "New:" << endl;
        for (int i = 0; i < 1024; i++) {
          cout << dataVectorIn[i];
        }
        cout << endl;

        cout << "Received vector size: " << dataVectorIn.size() << endl;

        //!TEST

        //DE-SERIALIZE TEST

        // convert to actual message
        messaging::RequestMessage* msgReply2 = new messaging::RequestMessage;
        msgReply2->ParseFromArray(reinterpret_cast<char*>(dataVectorIn.data()), dataVectorIn.size());

        // check the replyCommand
        if (msgReply2->command().compare("OK") != 0) {
          // There was an error
          cerr << msgReply2->error() << endl;
        } else {
          // read out the returned value
          //replyValue = msgReply->value();
          cout << "OK was received" << endl;
        }

        cout << "Value payload size: " << msgReply2->value().size() << endl;

        mq2.closeSocket();
        mq2.destroy();

        //!TEST

        // Create an incoming buffer
        TBufferFile *buf2 = new TBufferFile(TBuffer::kWrite);

        // Write the contents of the char pointer to the incoming buffer
        buf2->WriteBuf((void*)msgReply2->value().c_str(), msgReply2->value().size());

        // Change the buffer mode to read
        buf2->SetReadMode();

        // Rewind the buffer pointer to the beginning (This works like a file pointer. In case of a TMessage the pointer should be rewinded to position 8 instead of 0)
        buf2->SetBufferOffset(0);

        // Cast the object contained in the incoming buffer to an AliCDBEntry
        AliCDBEntry *en2 = (AliCDBEntry*)buf2->ReadObject(AliCDBEntry::Class());

        // Debug
        en2->Print();

        delete msg2;
        delete msgReply2;

        break;
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

double DBConnector::calculateDurationMicroseconds(struct timespec start, struct timespec stop)
{
  uint64_t secondsDifference = stop.tv_sec - start.tv_sec;
  uint64_t nanosecondsDifference = stop.tv_nsec - start.tv_nsec;
  nanosecondsDifference += secondsDifference * 1000000000;
  return nanosecondsDifference / 1000.0;
}

void DBConnector::putValue(std::string key, std::vector<char> dataVectorOut, size_t size)
{
  // Create PUT message as a google protocol buffer message
  messaging::RequestMessage* msg = new messaging::RequestMessage;
  msg->set_command("PUT");
  msg->set_key(key);
  msg->set_value(&dataVectorOut[0], size);

  // Serialize the message to a binary buffer and send it
  int mysize = msg->ByteSize();

  cout << "Protocol buffers reported size: " << mysize << endl;

  void *serialBuffer = malloc(mysize);
  msg->SerializeToArray(serialBuffer, mysize);

  MQ mq;
  mq.openSocket(ZMQ_REQ);

  stringstream ss;
  ss << "tcp://cernvm14:5559";

  mq.connect(ss.str());
  mq.send((char *)serialBuffer, size);

  // Receive message as a serialized string and de-serialize it
  std::string repString = mq.receive();

  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromString(repString);

  if (msgReply->command().compare("OK") != 0) {
    LOG_DEBUG(msgReply->error());
  }

  mq.closeSocket();
  mq.destroy();

  delete msg;
  delete msgReply;
}

char* DBConnector::getValue(std::string key)
{
/*  // Create GET message
  messaging::RequestMessage* msg = new messaging::RequestMessage;
  msg->set_command("GET");
  msg->set_key(key);

  // Serialize the message to a string
  string getString;
  msg->SerializeToString(&getString);

  stringstream ss;
  ss << "tcp://cernvm14:5559";

  // Connect and send the message
  MQ mq;
  mq.openSocket(ZMQ_REQ);

  mq.connect(ss.str());
  mq.send((char *)getString.c_str(), getString.length());

  // check reply for OK
  char *msg;
  mq.receive(msg, 1024);

  // convert to actual message
  messaging::RequestMessage* msgReply = new messaging::RequestMessage;
  msgReply->ParseFromArray(buffer);
  void *buffer;

  // check the replyCommand
  if (msgReply->command().compare("OK") != 0) {
    // There was an error
    cerr << msgReply->error() << endl;
  } else {
    // read out the returned value
    replyValue = msgReply->value();
  }

  cout << "Strlen: " << strlen(msgReply->value().c_str()) << endl;

  delete msg;
  delete msgReply;

  return replyValue;*/
}
