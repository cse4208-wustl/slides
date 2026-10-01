CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
TARGET   := demo
SRC      := demo.cpp

.PHONY: all run asan noelide leaks clean

# Default build: modern standard (C++17)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

# Turn off copy elision: see the move hidden in "return local;"
noelide: $(SRC)
	$(CXX) $(CXXFLAGS) -fno-elide-constructors -o $(TARGET)_noelide $(SRC)
	./$(TARGET)_noelide

# AddressSanitizer build: catches double delete (Rule of Three violations),
# use-after-free (and leaks on Linux)
asan: $(SRC)
	$(CXX) -std=c++17 -g -fsanitize=address -fno-omit-frame-pointer -o $(TARGET)_asan $(SRC)
	./$(TARGET)_asan

# macOS leak check (LeakSanitizer is not available with Apple clang)
leaks: $(TARGET)
	leaks --atExit -- ./$(TARGET)

clean:
	rm -rf $(TARGET) $(TARGET)_asan $(TARGET)_noelide *.dSYM
