// C C++ includes
#include <iostream>
#include <string>
// External Library: ZMQ
#include <zmq.hpp>
// External library: sha1
#include "sha1.h"
// KeyValueClusterPerf includes
#include "MessageSender.h"
#include "logger.h"

MessageSender::MessageSender(bool enableSecurity)
{
	// store security settings
	securityEnabled = enableSecurity;
	if(securityEnabled)
	{
		LOG_DEBUG("Security turned on");
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
	if(securityEnabled)
	{
		hash = sha1(message);
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