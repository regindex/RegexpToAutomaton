#include "internal/glushkov-automaton.hpp"
#include "internal/powerset-construction.hpp"
#include "internal/dfa-minimization.hpp"

// function that prints the instructions for using the tool
void print_help(char** argv) { 
    std::cout << std::endl <<
        "Usage: " << argv[0] << " [regexp] [options]" << std::endl 

        << "Tool to compute an automaton out of a regexp." << std::endl << std::endl

        << "    -n, --NFA" << std::endl 
        << "        Compute the Glushkov automaton (NFA eps-free)." << std::endl 

        << "    -d, --DFA" << std::endl 
        << "        Compute the Glushkov DFA running the powerset construction algorithm." << std::endl 

        << "    -m, --DFAmin" << std::endl
        << "        Compute the minimized Glushkov DFA." << std::endl 

        << "    -v, --verbise" << std::endl
        << "        Activate the verbose mode." << std::endl 
        << std::endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args& arg) {

    if(argc < 3){ print_help(argv); exit(1); }

    // read regexp
    arg.regexp = std::string(argv[1]);

    // read and parse input parameters
    for(int i=2;i<argc;++i)
    {
        std::string param = argv[i];

        if( param == "-n" or param == "--NFA" )
        {
            arg.NFA = true;
        }
        else if( param == "-d" or param == "--DFA" )
        {
            arg.DFA = true;
        }
        else if( param == "-m" or param == "--DFAmin" )
        {
            arg.DFAmin = true;
        }
        else if( param == "-v" or param == "--verbose" )
        {
            arg.verbose = true;
        }
        else
        {
            std::cerr << "Unknown option. Use -h for help." << std::endl;
            exit(-1);
        }
    }
    // check mode
    uint32_t counter = (int)arg.NFA + (int)arg.DFA + (int)arg.DFAmin;
    if(counter != 1)
    {
            std::cerr << "Please select one option out of NFA|DFA|DFAmin" << std::endl;
            exit(1);
    }
}

int main(int argc, char *argv[])
{
    // read arguments
    Args arg;
    parseArgs(argc, argv, arg);

    if( arg.NFA )
    {
        compute_glushkov_automaton(arg.regexp,sigma,arg.verbose,true);
    }
    else if( arg.DFA )
    {
        NFA* glushkov_automaton = compute_glushkov_automaton(arg.regexp,sigma,arg.verbose,false);
        compute_powerset_construction(glushkov_automaton,arg.verbose,true);
    }
    else if( arg.DFAmin )
    {
        NFA* glushkov_automaton = compute_glushkov_automaton(arg.regexp,sigma,arg.verbose,false);
        NFA* glushkov_dfa       = compute_powerset_construction(glushkov_automaton,arg.verbose,false);
        delete glushkov_automaton;
        if(arg.verbose) std::cout << "###### Minimized DFA to stdout:" << std::endl;
        minimize_DFA(glushkov_dfa);
    }
    
	return 0;
}