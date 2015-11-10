# KeyValueClusterPerf

### Prerequisites

Before compilation of the performance framework, the schema of protocol buffers has to be created for request.proto that can be found in the RiakJavaC repository. The generated files should be present at the src/ subdirectory.

    protoc --cpp_out=../../src/ request.proto

The following was used to compile/link the executable:

    g++ -Wall -std=c++0x -c src/*.cc src/*.c -I/opt/alice/external/include
    g++ -o keyvalueclusterperf *.o -L/opt/alice/external/lib/ -lzmq -lboost_program_options -lprotobuf

And to execute:
    LD_LIBRARY_PATH=/opt/alice/external/lib/ ./keyvalueclusterperf
