CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread
INC = -Iinclude

all: server client test_cli

server: src/server.cpp src/libtslog.cpp
	$(CXX) $(CXXFLAGS) $(INC) -o server src/server.cpp src/libtslog.cpp

client: src/client.cpp src/libtslog.cpp
	$(CXX) $(CXXFLAGS) $(INC) -o client src/client.cpp src/libtslog.cpp

test_cli: tests/test_cli.cpp src/libtslog.cpp
	$(CXX) $(CXXFLAGS) $(INC) -o test_cli tests/test_cli.cpp src/libtslog.cpp

clean:
	rm -f server client test_cli *.log
