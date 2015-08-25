// C C++ includes
#include <iostream>
#include <string>
// External Library: ZMQ
#include <zmq.hpp>
// External library: sha1
#include "sha1.h"
// External library: vmac 
#include "vmac.h"
// KeyValueClusterPerf includes
#include "MessageSender.h"
#include "logger.h"

MessageSender::MessageSender(string security)
{
	// store security settings (use two boolean values for rapid evaluation)
	sha1Enabled = false;
	vmacEnabled = false;
	if(security.compare("sha1")==0)
	{
		LOG_DEBUG("SHA1 enabled");
		sha1Enabled = true;
	}
	if(security.compare("vmac")==0)
	{
		LOG_DEBUG("VMAC ENABLED");
		ALIGN(4) unsigned char key[] = "abcdefghijklmnop";
		vmacEnabled = true;
		vmac_set_key(key, &ctx);
	}
	// start zeroMQ
	std::cout << "messenger started" << std::endl;
	context = new zmq::context_t(1);
	socket = new zmq::socket_t(*context, ZMQ_REQ);
	// Connect to the broker node cernvm22
	socket->connect("tcp://137.138.234.128:5559");
}

MessageSender::~MessageSender()
{
	delete socket;
	delete context;
}

void MessageSender::send(string message)
{
	if(sha1Enabled)
	{
		// calculate sha1 of message
		sha1Hash = sha1(message);
	}
	if(vmacEnabled)
	{
		// calculate vmac of message
		// could be even more efficient if string data is alligned in memory
		vmacHash = vmac((unsigned char*)message.c_str(), message.length(),(unsigned char*)"\0\0\0\0\0\0\0\0bcdefghi", &tagl, &ctx);
	}
	zmq::message_t request(message.length());
	memcpy((char*)request.data(), message.c_str(), message.length());
	socket->send(request);
}

string MessageSender::receive()
{
	zmq::message_t reply;
	socket->recv(&reply);
	string repstr((char*)reply.data(), reply.size());
	return repstr;
}