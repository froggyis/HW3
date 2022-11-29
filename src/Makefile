CC =g++
CFLAGS = -std=c++14
LIBS = -lm -lsystemc
SOURCES = main.cpp
EXECUTABLE = ../bin/hw3

all:
	$(CC) $(CFLAGS) $(SOURCES) parser.cpp -o $(EXECUTABLE) -O3

clean:
	rm -rf  $(EXECUTABLE)
