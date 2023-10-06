# compilation flags
CCX_FLAGS=-std=c++17 -O3 -Wall -Wextra -pedantic
CCX=g++

# main executables 
EXECS = regToNfa

# targets not producing a file declared phony
.PHONY: all clean

all: $(EXECS)

regToNfa: main.cpp internal/glushkov-automaton.hpp internal/nfa.hpp internal/common.hpp
	$(CCX) $(CCX_FLAGS) -o $@ main.cpp internal/glushkov-automaton.hpp internal/nfa.hpp internal/common.hpp

clean:
	rm -f $(EXECS)