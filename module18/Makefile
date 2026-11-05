CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
DEMOS    := main

# Demos always rebuild, so edits made live in class are picked up.
.PHONY: all clean asan $(DEMOS)

all: $(DEMOS)

$(DEMOS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	./$@

# Same demo with AddressSanitizer: turns invalidated-iterator bugs into reports
asan: main.cpp
	$(CXX) $(CXXFLAGS) -fsanitize=address -o demo $<
	./demo

clean:
	rm -rf $(DEMOS) *.dSYM
