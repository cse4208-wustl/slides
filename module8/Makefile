CXX      ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -g
TARGET   := demo
SRC      := demo.cpp

.PHONY: all run cpp11 clean

# Default build: modern standard (C++17)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

# Build under strict C++11 rules -- useful for the Q1 discussion of whether
# the if/else lambda (once added) compiles under C++11 vs. later standards.
cpp11: $(SRC)
	$(CXX) -std=c++11 -Wall -Wextra -g -o $(TARGET)_cpp11 $(SRC)
	./$(TARGET)_cpp11

clean:
	rm -f $(TARGET) $(TARGET)_cpp11
