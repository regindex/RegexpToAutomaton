# Regexp to Automaton
This is a software to construct an automaton out of a regular expression.

# Usage

```
Usage: ./regToAutomaton [regexp] [options]
Tool to compute an automaton out of a regexp.

    -n, --NFA
        Compute the Glushkov automaton (NFA eps-free).
    -d, --DFA
        Compute the Glushkov DFA running the powerset construction algorithm.
    -m, --DFAmin
        Compute the minimized Glushkov DFA.
    -v, --verbise
        Activate the verbose mode.
```
Supported operators: ()+?|* 

### Requirements

This tool requires:
* A modern C++17 compiler.

# Example

### Download and Compile

```console
git clone -b develop https://github.com/regindex/RegexpToAutomaton.git
cd RegexpToAutomaton
make
```

### Run on Example Data

```console
// Construct the Glushkov NFA of a simple regexp
./regToNfa "(a*(ab))+((b*)|a*)*" --NFA --verbose
// Construct the Glushkov NFA and convert it into a DFA
./regToNfa "(a*(ab))+((b*)|a*)*" 1 --DFA --verbose
// Construct the Glushkov NFA and convert it into a minimum DFA
./regToNfa "(a*(ab))+((b*)|a*)*" 1 --DFAmin --verbose
```

### Implementation:

* [Davide Cenzato](https://github.com/davidecenzato)