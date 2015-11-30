cmake_minimum_required (VERSION 2.8.11)
project (KeyValueClusterPerf)

IF(NOT DEFINED ENV{SIMPATH})
   MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{SIMPATH})

SET(SIMPATH $ENV{SIMPATH})

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}
${SIMPATH}/include
)

include_directories( ${INCLUDE_DIRECTORIES})

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

LINK_DIRECTORIES(${SIMPATH}/lib)

add_executable(keyvalueclusterperf
src/AccessPattern.cc
src/MessageSender.cc
src/sha1.cc
src/ConfigurationManager.cc
src/RamCloudKeyValueDB.cc
src/SimulationController.cc
src/ConstantValueDistribution.cc
src/RandomAccessPattern.cc
src/SimulationWorker.cc
src/DummyKeyValueDB.cc
src/ReadOnlyAccessPattern.cc
src/Simulator.cc
src/KeyValueClusterPerf.cc
src/request.pb.cc
src/WriteOnlyAccessPattern.cc
src/KeyValueDB.cc
src/RiakJavaKeyValueDB.cc
src/rijndael-alg-fst.c
src/vmac.c)

TARGET_LINK_LIBRARIES(keyvalueclusterperf zmq boost_system boost_program_options protobuf)
