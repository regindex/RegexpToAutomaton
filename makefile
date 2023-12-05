# compilation flags
CCX_FLAGS=-std=c++17 -O3 -Wall -Wextra -pedantic
CCX=g++

# main executables 
EXECS = regToAutomaton randReg

# targets not producing a file declared phony
.PHONY: all clean

all: $(EXECS)

regToAutomaton: main.cpp
	$(CCX) $(CCX_FLAGS) -o $@ main.cpp

randReg: gen_regexp.cpp
	$(CCX) $(CCX_FLAGS) -o $@ gen_regexp.cpp

clean:
	rm -f $(EXECS)