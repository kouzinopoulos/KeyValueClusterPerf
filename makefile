CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=src/*
OBJECTS=$(SOURCES:.cxx=.o)
EXECUTABLE=keyvalueclusterperf

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cxx.o:
	$(CC) $(CFLAGS) $< -o $@