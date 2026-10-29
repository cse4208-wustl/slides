CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
DEMOS    := hash_demo array_demo

# Demos always rebuild, so edits made live in class are picked up.
.PHONY: all clean $(DEMOS)

all: $(DEMOS)

$(DEMOS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	./$@

clean:
	rm -rf $(DEMOS) *.dSYM
