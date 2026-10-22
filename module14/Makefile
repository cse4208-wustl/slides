CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
DEMOS    := q2_demo

# Build and run one demo:   make q2_demo
# Demos always rebuild, so edits made live in class are picked up.
.PHONY: all clean $(DEMOS)

all: $(DEMOS)

$(DEMOS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	./$@

clean:
	rm -rf $(DEMOS) *.dSYM
