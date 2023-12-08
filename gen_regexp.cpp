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

void compute_random_tree(stree_node* root, uint type, uint unary, uint depth)
{
    // compute type
    if( type < 20 ){ type = 2; }
    else if( type >= 20 and type < 60 ){ type = 0; }
    else{ type = 1; }
    // compute unary operator
    if( unary < 10 ){ root->unary = 1; }
    else if( unary >= 10 and unary < 20 ){ root->unary = 2; }
    else if( unary >= 20 and unary < 30 ){ root->unary = 3; }
    else{ root->unary = 0; }
    // if max depth reached
    if( depth == 4 ){ type = 2; }
    else if( depth == 0 ){ type = random(0,1); }

    if( type==0 )
    {
        //std::cout << "type: " << type << "\n";
        root->type = op_code('|');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,random(0,100),random(0,100),depth+1);
        compute_random_tree(root->right,random(0,100),random(0,100),depth+1);
    }
    else if ( type==1 )
    {
        //std::cout << "Type: " << type << "\n";
        root->type = op_code('_');
        root->value = -1;
        root->left = new stree_node(0,0);
        root->right = new stree_node(0,0);
        // recurse
        compute_random_tree(root->left,random(0,100),random(0,100),depth+1);
        compute_random_tree(root->right,random(0,100),random(0,100),depth+1);
    }
    else
    {
        //root = new stree_node(0,random(97,122));
        root->type = 0;
        root->value = random(97,122);
    }
}

void tree_to_reg(stree_node* node)
{
    if( op_char(node->type)=='|' )
    {
        //std::cout << "(" << tree_to_reg(node->left) << "|" << tree_to_reg(node->right) << ")";
        std::cout << "(";
        tree_to_reg(node->left);
        std::cout << "|";
        tree_to_reg(node->right);
        std::cout << ")";
    }
    else if ( op_char(node->type)=='_' )
    {
        //std::cout << tree_to_reg(node->left) << tree_to_reg(node->right);
        tree_to_reg(node->left);
        tree_to_reg(node->right);
    }
    else
    {
        //return node->value;
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

//int main(int argc, char *argv[])
int main()
{

    stree_node* root = new stree_node(0,0);
    uint type = random(0,100);
    uint unary = random(0,100);
    // 0 is (+), 1 is (.), 2 is a random symbol
    compute_random_tree(root, type, unary, 0);
    std::cout << "t: " << (int)root->type << "\n";
    std::cout << root->right << "\n";
    std::cout << root->left << "\n";
    tree_to_reg(root);
    std::cout << "\n";

	return 0;
}