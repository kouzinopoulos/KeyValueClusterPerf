CC=g++
CFLAGS=-c -Wall
LDFLAGS=-std=c++0x -lzmq -LRAMCloud/obj.master -lramcloud -IRAMCloud/gtest/include -IRAMCloud/src -IRAMCloud/obj.master
SOURCES=src/*
OBJECTS=$(SOURCES:.cxx=.o)
EXECUTABLE=keyvalueclusterperf

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cxx.o:
	$(CC) $(CFLAGS) $< -o $@