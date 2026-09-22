CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
TARGET   := demo
SRC      := demo.cpp

.PHONY: all run cpp11 clean

# Default build: modern standard (C++17)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET)_cpp11
