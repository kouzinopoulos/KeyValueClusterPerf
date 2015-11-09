# KeyValueClusterPerf

### Prerequisites

Before compilation of the performance framework, the schema of protocol buffers has to be created for request.proto that can be found in the RiakJavaC repository. The generated files should be present at the src/ subdirectory.

    protoc --cpp_out=../../src/ request.proto
