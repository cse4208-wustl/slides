CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
TARGET   := demo
SRC      := demo.cpp

.PHONY: all run asan leaks clean

# Default build: modern standard (C++17)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

# AddressSanitizer build: catches use-after-free, double delete,
# new[]/delete mismatch (and leaks on Linux)
asan: $(SRC)
	$(CXX) -std=c++17 -g -fsanitize=address -fno-omit-frame-pointer -o $(TARGET)_asan $(SRC)
	./$(TARGET)_asan

# macOS leak check (LeakSanitizer is not available with Apple clang)
leaks: $(TARGET)
	leaks --atExit -- ./$(TARGET)

clean:
	rm -rf $(TARGET) $(TARGET)_asan *.dSYM
