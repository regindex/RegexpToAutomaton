# Regexp to NFA
This is a software to construct the Glushkov automaton of a regular expression.

# Usage

```
Usage: ./regToNFA <regexp> <verbose mode 0|1>
```
Supported operators: ()+?|* 

### Requirements

This tool requires:
* A modern C++17 compiler.

# Example

### Download and Compile

```console
git clone https://github.com/regindex/RegexpToNfa.git
cd RegexpToNfa
make
```

### Run on Example Data

```console
// Construct the NFA of a simple regexp
./regToNfa "(a*(ab))+((b*)|a*)*" 1 (verbose mode on)
```

### Implementation:

* [Davide Cenzato](https://github.com/davidecenzato)