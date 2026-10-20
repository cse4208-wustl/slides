CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -g
DEMOS    := q1_demo q2_demo q3_demo q4_demo q5_demo move_demo

# Build and run one demo:   make q1_demo   (or q2_demo, ..., move_demo)
# Build and run all of them: make
# Demos always rebuild, so edits made live in class are picked up.
.PHONY: all clean $(DEMOS)

all: $(DEMOS)

$(DEMOS): %: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<
	./$@

clean:
	rm -rf $(DEMOS) *.dSYM demo_output.txt
