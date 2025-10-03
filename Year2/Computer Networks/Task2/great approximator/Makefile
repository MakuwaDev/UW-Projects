CXX     = g++
CXXFLAGS = -Wall -Wextra -O2 -pedantic -std=c++20
LFLAGS =

.PHONY: all clean

TARGETS = approx-client approx-server

CLIENT_SRC = approx-client.cpp detail.cpp err.cpp protocol.cpp
SERVER_SRC = approx-server.cpp detail.cpp err.cpp protocol.cpp

CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

all: $(TARGETS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

approx-client: $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

approx-server: $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o $(TARGETS) *~