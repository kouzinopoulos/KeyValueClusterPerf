# KeyValueClusterPerf

### Prerequisites

Before compilation of the performance framework, the schema of protocol buffers has to be created for request.proto that can be found in the RiakJavaC repository. The generated files should be present at the src/ subdirectory.

    protoc --cpp_out=../../KeyValueClusterPerf/src/ request.proto

The following was used to compile/link the executable:

    g++ -Wall -std=c++0x -c src/*.cc src/*.c -I/opt/alice/external/include
    g++ -o keyvalueclusterperf *.o -L/opt/alice/external/lib/ -lzmq -lboost_program_options -lprotobuf

And to execute:
    LD_LIBRARY_PATH=/opt/alice/external/lib/ ./keyvalueclusterperf

### Execution summary

KeyValueClusterPerf is a benchmarking framework that tests the efficiency of a Riak database when storing and retrieving key/value pairs. KeyValueClusterPerf consists of two main components, the Controllers and the Workers.
The Controllers are responsible to organize tasks among the Workers, while the Workers receive those tasks and perform the actual simulation.

First, the hwbroker must be started. The broker accepts connections from one or more frontend clients, receives commands (PUT, GET, ERROR, OK) and key/value pairs and forwards them to one or more riak java clients. By default, the broker listens on port 5559 of the current node for the frontend clients and port 5560 of the current node for the riak java clients. Both of these values are currently hardcoded. To execute the broker:

    java -cp /opt/alice/external/share/java/zmq.jar:/opt/aliceiakJavaC/hwbroker/dist/hwbroker.jar hwbroker.Hwbroker debug

Then, at least one riak java client must be started. The client accepts connections from the broker using the MessageListener class on port 5560 of the current node. Subsequently, the client connects to the Riak cluster nodes from the RiakConnection class using a hostname list provided in the Main.java file.

Additionally, the client parses user commands from the command line on a new thread. The available commands are "stop" and "delete". To start the riak java client:

    java -cp ./lib/*:RiakJavaClient-1.0-SNAPSHOT.jar com.cern.riakjavaclient.Main

After the riak java client has started, the KeyValueClusterPerf framework must be executed. First, the worker nodes must be fired up. This can be done manually via the command:

    ./keyvalueclusterperf --port 3000 --dport 4000

This specifies two ports. The two different types are due to the way ZeroMQ handles small and large messages. First a control port (3000) is opened. When the data has been retrieved, a data port (4000) is opened which allows bigger transfers. Multiple worker processes can be executed on the same node without impacting performance.

When the worker instances are running, the controller instance can be started:

    ./keyvalueclusterperf --controller --hostlimit $i --simiteration $simit

The hostlimit and simiteration arguments are optional. The hostlimit restricts the number of workers keyvalueclusterperf will use. This is useful when you don't want to rewrite the hostFile.cfg file every
time. The simiteration is just a number keyvalueclusterperf will use to identify the output. It will write a resultsX.csv file with the results of the benchmark. If the optional arguments are omitted, the follwoing command can be executed instead:

    ./keyvalueclusterperf --controller

Note that the controller needs a hostFile.cfg in its directory which indicates which hosts are available with running worker instances. In the example above this file would contain:

#hostname:firstCommandPort|lastCommandPort|firstDataPort
localhost:3000|3000|4000

The hostFile allows you to easily configure if there are multiple instances on the same node. For example you could write 'localhost:3000|3099|4000' and it would think there are 100 worker instances on localhost, with ports 3000-3099 and data ports 4000-4099.
