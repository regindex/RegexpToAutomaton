#ifndef COMMON_HPP
#define COMMON_HPP

// LIBRARIES #########################
#include <iostream>
#include <string>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <random>
// GLOBAL VARIABLES ###################
const char sep = '_';
const std::set<char> operators({'+', '*', '.', '|', '(', ')', '_', '?', '+'});
bool vmode = false;
typedef uint32_t uint;
int sigma = 128;
// RANDOM NUMBER GENERATOR ###################
template<typename T>
T random(T range_from, T range_to) {
    std::random_device                  rand_dev;
    std::mt19937                        generator(rand_dev());
    std::uniform_int_distribution<T>    distr(range_from, range_to);
    return distr(generator);
}
// ARGS STRUCT ###################
struct Args
{
  std::string regexp;
  bool NFA = false, DFA = false, DFAmin = false;
  bool verbose = false; 
};
// ARGS REGEXP GEN ###################
struct Args_gen
{
  uint max_depth = 4;
  uint unionP = 20, concatP = 40;
  uint kleeneP = 10, plusP = 10, questionP = 10;
  uint alphabet = 1;
};
// SYNTAX TREE NODE####################
struct stree_node
{
	char type, unary; 
    uint value;
	stree_node *left, *right;

    std::vector<uint> lmost, rmost;
    // maybe be empty bool
    bool mbe;

	stree_node(char type_, uint value_)
	{
		this->type = type_;
		this->value = value_;
        this->unary = -1;
        this->mbe = false;
		this->right = nullptr;
		this->left = nullptr;
	}
};
// PRECEDENCE CODES####################
int op_code_prec(char a)
{
	switch (a) {
	    case '|':
	        return 1;
	        break;
	    case '_':
	        return 2;
	        break;
	    case '*':
	    	return 3;
	    	break;
        case '?':
            return 3;
            break;
        case '+':
            return 3;
            break;
	    default:
	        return 0;
    }
}
// CHAR CODES##########################
int op_code(char a)
{
    switch (a) {
        case '|':
            return 1;
            break;
        case '_':
            return 2;
            break;
        case '*':
            return 3;
            break;
        case '?':
            return 4;
            break;
        case '+':
            return 5;
            break;
        default:
            return 0;
    }
}
// CODE CHARACTERS####################
char op_char(int a)
{
    switch (a) {
        case 1:
            return '|';
            break;
        case 2:
            return '_';
            break;
        case 3:
            return '*';
            break;
        case 4:
            return '?';
            break;
        case 5:
            return '+';
            break;
        default:
            return 'u';
    }
}
// COMPUTE CHAR PRECEDENCE
inline char precedence(char a, char b)
{
    return op_code_prec(a) > op_code_prec(b);
}

#endif