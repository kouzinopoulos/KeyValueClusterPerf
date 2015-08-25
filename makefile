CC=g++
CFLAGS=-Wall
#LDFLAGS=-std=c++0x -lzmq -lriak_c_client-0.5 -LRAMCloud/obj.master -lramcloud -IRAMCloud/gtest/include -IRAMCloud/src -IRAMCloud/obj.master
LDFLAGS=-std=c++0x -lprotobuf -lzmq -lboost_program_options
SOURCES=src/*
OBJECTS=$(SOURCES:.cxx=.o)
EXECUTABLE=keyvalueclusterperf

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.cxx.o:
	$(CC) $(CFLAGS) $< -o $@