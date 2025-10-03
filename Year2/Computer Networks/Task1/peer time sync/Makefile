CXX     = g++
CXXFLAGS = -Wall -Wextra -O2 -pedantic -std=c++20
LFLAGS =

.PHONY: all clean

TARGET = peer-time-sync

# All source files
SRC = peer-time-sync.cpp protocol.cpp detail.cpp err.cpp
OBJ = $(SRC:.cpp=.o)

# Header files
DEPS = protocol.h err.h detail.h

# Default target
all: $(TARGET)

# Compilation step
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linking step
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean rule
clean:
	rm -f $(OBJ) $(TARGET) *~