CC = g++
OPT = -O3
# LIBS = -static-libstdc++
LIBS = -std=c++17

all: main

main: main.cpp
	$(CC) -o ../bin/hw3 main.cpp ./Struct/Struct.cpp ./Floorplanner/Floorplanner.cpp ./Parser/Parser.cpp $(OPT) $(LIBS)

clean:
	rm -f ../bin/hw3