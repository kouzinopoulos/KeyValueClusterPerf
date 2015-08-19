#ifndef __MESSAGESENDER_H__
#define __MESSAGESENDER_H__

// C C++ includes
#include <string>
// External library: ZMQ
#include <zmq.hpp>

using namespace std;

/**
 * Class responsible for all communication between the Java client of Riak and this C program
 */
class MessageSender
{
	public:
		MessageSender(bool enableSecurity);
		~MessageSender();

		void send(string message);
		string receive();

		string hash;

	private:
		zmq::context_t* context;
		zmq::socket_t* socket;
		bool securityEnabled;
};

#endif
//__MESSAGESENDER_H__