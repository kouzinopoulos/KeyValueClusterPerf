cmake_minimum_required (VERSION 2.8.11)
project (KeyValueClusterPerf)

IF(NOT DEFINED ENV{SIMPATH})
   MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is needed to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{SIMPATH})

SET(SIMPATH $ENV{SIMPATH})

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}
${SIMPATH}/include
)

include_directories( ${INCLUDE_DIRECTORIES})

set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

LINK_DIRECTORIES(${SIMPATH}/lib)

set(SRC_LIST src/AccessPattern.cc
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
src/MQ.cc
src/request.pb.cc
src/WriteOnlyAccessPattern.cc
src/KeyValueDB.cc
src/RiakJavaKeyValueDB.cc
src/rijndael-alg-fst.c
src/vmac.c)

set(CONFIG_LIST controllerConfiguration.xml
workerConfiguration.xml)

add_executable(keyvalueclusterperf ${SRC_LIST} ${CONFIG_LIST})

TARGET_LINK_LIBRARIES(keyvalueclusterperf zmq boost_system boost_program_options protobuf)

install(TARGETS keyvalueclusterperf DESTINATION ${CMAKE_SOURCE_DIR}/bin COMPONENT binaries)
install(FILES controllerConfiguration.xml workerConfiguration.xml DESTINATION ${CMAKE_SOURCE_DIR}/bin COMPONENT config)
