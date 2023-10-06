#include "internal/glushkov-automaton.hpp"

int main(int argc, char *argv[])
{
    int sigma = 128;
    bool verbose = true;

    if( argc != 3 ){ std::cout << "Wrong format! type [executable] [regexp] [verbose mode on(1)/off(0)]\n"; }

    std::string regexp = std::string(argv[1]);
    verbose = atoi(argv[2]);

    //regexp = "(a*(ab))+((b*)|a*)*";
    std::cout << "regexp: " << regexp << "\n";

    NFA* glushkov_automaton = compute_glushkov_automaton(regexp,sigma,verbose);

	return 0;
}