#include "internal/glushkov-automaton.hpp"
#include "internal/powerset-construction.hpp"
#include "internal/dfa-minimization.hpp"

#include <random>

template<typename T>
T random(T range_from, T range_to) {
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}

/*
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
*/

void compute_random_tree(stree_node* root, uint type, uint depth)
{
    if( depth == 4 ){ type = 2; }
    if( type==0 )
    {
        //std::cout << "type: " << type << "\n";
        root->type = op_code('|');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,random(0,2),depth+1);
        compute_random_tree(root->right,random(0,2),depth+1);
    }
    else if ( type==1 )
    {
        //std::cout << "Type: " << type << "\n";
        root->type = op_code('_');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,random(0,2),depth+1);
        compute_random_tree(root->right,random(0,2),depth+1);
    }
    else
    {
        //root = new stree_node(0,random(97,122));
        root->type = 0;
        root->value = random(97,122);
    }
}

char tree_to_reg(stree_node* node)
{
    if( op_char(node->type)=='|' )
    {
        std::cout << "(" << tree_to_reg(node->left) << "|" << tree_to_reg(node->right) << ")";
    }
    else if ( op_char(node->type)=='_' )
    {
        std::cout << tree_to_reg(node->left) << tree_to_reg(node->right);
    }
    else
    {
        return node->value;
    }
}

/*
void print_regexp()
{

}

stree_node* compute_syntax_tree(std::string& regexp,std::vector<char>& mapping)
{
    std::stack<stree_node*> stack;
    uint32_t id = 0; 

    for(char c : regexp)
    {
        if( c == '|' )
        {
            stree_node* z = new stree_node(op_code('|'),-1);
            z->right = stack.top();
            stack.pop();
            z->left = stack.top();
            stack.pop();
            stack.push(z);
        }
        else if( c == '_' )
        {
            stree_node* z = new stree_node(op_code('_'),-1);
            z->right = stack.top();
            stack.pop();
            z->left = stack.top();
            stack.pop();
            stack.push(z);
        }
        else if( c == '*' )
        {
            stack.top()->unary = 1;
        }
        else if( c == '+' )
        {
            stack.top()->unary = 2;
        }
        else if( c == '?' )
        {
            stack.top()->unary = 0;
        }
        else
        {
            mapping.push_back(c);
            stack.push( new stree_node(0, id++) );
        }
    }

    assert( stack.size() == 1 );
    return stack.top();
}
*/

int main(int argc, char *argv[])
{

    stree_node* root = new stree_node(0,0);
    uint type = random(0,1);
    // 0 is (+), 1 is (.), 2 is a random symbol
    compute_random_tree(root, type, 0);
    std::cout << "t: " << (int)root->type << "\n";
    std::cout << root->right << "\n";
    std::cout << root->left << "\n";
    tree_to_reg(root);
    std::cout << "\n";
    /*
    // read arguments
    Args arg;
    parseArgs(argc, argv, arg);

    //std::string regexp = std::string(argv[1]);
    //verbose = atoi(argv[2]);
  
    //regexp = "(a*(ab))+((b*)|a*)*";
    //std::cout << "regexp: " << regexp << "\n"; 

    if( arg.NFA )
    {
        compute_glushkov_automaton(arg.regexp,sigma,false,true);
    }
    else if( arg.DFA )
    {
        NFA* glushkov_automaton = compute_glushkov_automaton(arg.regexp,sigma,false,false);
        compute_powerset_construction(glushkov_automaton,false,true);
    }
    else if( arg.DFAmin )
    {
        NFA* glushkov_automaton = compute_glushkov_automaton(arg.regexp,sigma,false,false);
        NFA* glushkov_dfa       = compute_powerset_construction(glushkov_automaton,false,false);
        delete glushkov_automaton;
        minimize_DFA(glushkov_dfa);
    }
    */

	return 0;
}