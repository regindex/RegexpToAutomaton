# compilation flags
CCX_FLAGS=-std=c++17 -O3 -Wall -Wextra -pedantic
CCX=g++

# main executables 
EXECS = regToNfa

# targets not producing a file declared phony
.PHONY: all clean

all: $(EXECS)

regToNfa: main.cpp
	$(CCX) $(CCX_FLAGS) -o $@ main.cpp

clean:
	rm -f $(EXECS)