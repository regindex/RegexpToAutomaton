#include "internal/glushkov-automaton.hpp"
#include "internal/powerset-construction.hpp"
#include "internal/dfa-minimization.hpp"

const std::vector<char> DNA_vec{'a','c','g','t'};

// function that prints the instructions for using the tool
void print_help(char** argv) { 
    std::cout << std::endl <<
        "Usage: " << argv[0] << " [options]" << std::endl 

        << "Tool to compute a random regexp." << std::endl << std::endl

        << "    -m, --maxDepth" << std::endl 
        << "        maximum depth of the regexp syntex tree. Default: 4." << std::endl 

        << "    -u, --unionP" << std::endl 
        << "        Probability of sampling a union operator (|). Default: 20%." << std::endl 

        << "    -c, --concatP" << std::endl 
        << "        Probability of sampling a concat operator. Default: 40%." << std::endl 

        << "    -k, --kleeneP" << std::endl
        << "        Probability of sampling a kleene star operator (*). Default: 10%." << std::endl 

        << "    -p, --plusP" << std::endl
        << "        Probability of sampling a plus operator (+). Default: 10%." << std::endl 

        << "    -q, --questionP" << std::endl
        << "        Probability of sampling a question operator (?). Default: 10%." << std::endl 

        << "    --DNA" << std::endl
        << "        Use characters from DNA alphabet Default: English alphabet." << std::endl 
        << std::endl;
}

// function for parsing the input arguments
void parseArgs(int argc, char** argv, Args_gen& arg) {

    // read and parse input parameters
    for(int i=1;i<argc;++i)
    {
        std::string param = argv[i];
        i++;

        if( param == "-m" or param == "--maxDepth" )
        {
            arg.max_depth = std::atoi( argv[i] );
        }
        else if( param == "-u" or param == "--unionP" )
        {
            arg.unionP = std::atoi( argv[i] );
        }
        else if( param == "-c" or param == "--concatP" )
        {
            arg.concatP = std::atoi( argv[i] );
        }
        else if( param == "-k" or param == "--kleeneP" )
        {
            arg.kleeneP = std::atoi( argv[i] );
        }
        else if( param == "-p" or param == "--plusP" )
        {
            arg.plusP = std::atoi( argv[i] );
        }
        else if( param == "-q" or param == "--questionP" )
        {
            arg.questionP = std::atoi( argv[i] );
        }
        else if( param == "--DNA" )
        {
            arg.alphabet = 1;
        }
        else if( param == "-h" or param == "--help" )
        {
            print_help(argv); exit(1);
        }
        else
        {
            std::cerr << "Unknown option. Use -h for help." << std::endl;
            exit(-1);
        }
    }
    // check mode
    uint counter = arg.unionP + arg.concatP;
    if(counter > 100)
    {
            std::cerr << "Union and concat joined prob. must be <= 100" << std::endl;
            exit(1);
    }
    counter = arg.kleeneP + arg.plusP + arg.questionP;
    if(counter > 100)
    {
            std::cerr << "Kleene, plus, question joined prob. must be <= 100" << std::endl;
            exit(1);
    }
}

void compute_random_tree(stree_node* root, uint depth, Args_gen& arg)
{
    // compute two random numbers
    uint type = random(0,100);
    uint unary = random(0,100);
    // compute type
    if( type < arg.unionP ){ type = 0; }
    else if( type >= arg.unionP and type < arg.unionP + arg.concatP ){ type = 1; }
    else{ type = 2; }
    // compute unary operator
    if( unary < arg.kleeneP ){ root->unary = 1; }
    else if( unary >= arg.kleeneP and unary < (arg.kleeneP + arg.plusP) ){ root->unary = 2; }
    else if( unary >= (arg.kleeneP + arg.plusP) and unary < (arg.kleeneP + arg.plusP + arg.questionP) ){ root->unary = 3; }
    else{ root->unary = 0; }
    // if max depth reached
    if( depth == arg.max_depth ){ type = 2; }
    else if( depth == 0 ){ type = random(0,1); }

    if( type==0 )
    {
        root->type = op_code('|');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,depth+1,arg);
        compute_random_tree(root->right,depth+1,arg);
    }
    else if ( type==1 )
    {
        root->type = op_code('_');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,depth+1,arg);
        compute_random_tree(root->right,depth+1,arg);
    }
    else
    {
        root->type = 0;
        if( arg.alphabet == 0 )
            root->value = random(97,122);
        else if( arg.alphabet == 1 )
            root->value = DNA_vec[random(0,3)];
    }
}

void tree_to_reg(stree_node* node)
{
    if( op_char(node->type)=='|' )
    {
        std::cout << "(";
        tree_to_reg(node->left);
        std::cout << "|";
        tree_to_reg(node->right);
        std::cout << ")";
    }
    else if ( op_char(node->type)=='_' )
    {
        std::cout << "(";
        tree_to_reg(node->left);
        tree_to_reg(node->right);
        std::cout << ")";
    }
    else
    {
        std::cout << (char)node->value;
    }
    // print unary op
    if( node->unary > 0 )
    {
        if( node->unary == 1 ){ std::cout << "*"; }
        if( node->unary == 2 ){ std::cout << "+"; }
        if( node->unary == 3 ){ std::cout << "?"; }
    }
}

int main(int argc, char *argv[])
{
     // read arguments
    Args_gen arg;
    parseArgs(argc, argv, arg);

    stree_node* root = new stree_node(0,0);
    compute_random_tree(root, 0, arg);
    tree_to_reg(root);
    
    std::cout << "\n";

	return 0;
}