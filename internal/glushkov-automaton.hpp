#ifndef GLUSHKOV_AUTOMATON_HPP
#define GLUSHKOV_AUTOMATON_HPP

#include "common.hpp"
#include "nfa.hpp"

bool vmode = false;

void annotate_syntax_tree(stree_node* node, NFA* automaton, std::vector<char>& mapping);

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

void printBT(const std::string& prefix, const stree_node *node, bool isLeft)
{
    if( node != nullptr )
    {
        std::cout << prefix;

        std::cout << (isLeft ? "├──" : "└──" );

        // print the value of the node
        if(node->type != 0){

            std::string oper = "";
            oper += op_char(node->type);
        	
            if(node->unary == 1){ oper += "*"; }
            else if(node->unary == 2){ oper += "+"; }
            else if(node->unary == 0){ oper += "?"; }
            std::cout << oper << std::endl;
        }
        else{
            std::string oper = "";
            if(node->unary == 1){ oper += "*"; }
            else if(node->unary == 2){ oper += "+"; }
            else if(node->unary == 0){ oper += "?"; }
            std::cout << node->value + 1 << oper << std::endl;
    	}

        // enter the next tree level - left and right branch
        printBT( prefix + (isLeft ? "│   " : "    "), node->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), node->right, false);
    }
}

void printBT(const stree_node *root)
{
    printBT("", root, false);    
}

uint compute_postfix_format(std::string &regexp)
{
    std::size_t len = regexp.size();
    uint no_symbols = 0;

    std::vector<char> regsep;
    regsep.reserve(len*2);

    for(size_t i=0; i < (len-1); ++i)
    {
        regsep.push_back(regexp[i]);
        if( not (operators.count(regexp[i]) > 0) ) 
        {
            if( (not (operators.count(regexp[i+1]) > 0) ) or regexp[i + 1] == '(') regsep.push_back(sep);
            no_symbols++;
        }
        else
        {
            if( regexp[i] == ')' )
            {
                if( (regexp[i + 1] == '(') or (not (operators.count(regexp[i + 1]) > 0) ) ){ regsep.push_back(sep); }
            }

            else if( regexp[i] == '*' or regexp[i] == '+' or regexp[i] == '?' )
            {
                if( (regexp[i + 1] == '(') or (not (operators.count(regexp[i + 1]) > 0) ) ){ regsep.push_back(sep); }
            }
        }
    }

    regsep.push_back(regexp[len-1]);

    std::memset(&regexp[0], '.', len);
    size_t j = 0;
    std::stack<char> stack;
    for(char c : regsep) 
    {
        if( c == '*' or c == '+' or c == '?' or (not (operators.count(c) > 0) ) )
        {
            regexp[j++] = c;
        }
        else if( c == ')' )
        {
            while (stack.size() > 0 and stack.top() != '(')
            {
                regexp[j++] = stack.top();
                stack.pop();
            }
            stack.pop();
        }
        else if(c == '(')
        {
            stack.push(c);
        }
        else if( stack.size() == 0 or stack.top() == '(' or precedence(c, stack.top()) )
        {
            stack.push(c);
        }
        else
        {
            while( stack.size() > 0 and stack.top() != '(' and (not precedence(c, stack.top())) ){
                regexp[j++] = stack.top();
                stack.pop();
            }
            stack.push(c);
        }
    }

    while( stack.size() > 0 )
    {
        regexp[j++] = stack.top();
        stack.pop();
    }

    regexp.resize(j);

    if(vmode) std::cout << "No. symbols in regexp: " << no_symbols << "\n";
    return no_symbols;
}

void process_concat_oper(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{
    // recursive call
    annotate_syntax_tree(node->left, automaton, mapping);
    annotate_syntax_tree(node->right, automaton, mapping);

    for(size_t i=0;i<node->left->rmost.size();++i)
    {
        for(size_t j=0;j<node->right->lmost.size();++j)
        {
            if(vmode) std::cout << node->left->rmost[i]+1 << " - " << mapping[node->right->lmost[j]] << " -> " << node->right->lmost[j]+1 << "\n";
            automaton->add_edge(node->left->rmost[i]+1, node->right->lmost[j]+1, mapping[node->right->lmost[j]]);
        }
    }

    if( node->unary > 0 )
    {
        for(size_t i=0;i<node->right->rmost.size();++i)
        {
            for(size_t j=0;j<node->left->lmost.size();++j)
            {
                if(vmode) std::cout << node->right->rmost[i]+1 << " - " << mapping[node->left->lmost[j]] << " -> " << node->left->lmost[j]+1 << "\n";
                automaton->add_edge(node->right->rmost[i]+1, node->left->lmost[j]+1, mapping[node->left->lmost[j]]);
            }
        }
    }

    node->lmost = node->left->lmost;
    node->left->lmost.clear();
    node->left->rmost.clear();

    node->rmost = node->right->rmost;
    node->right->rmost.clear();
    node->right->lmost.clear();

    node->min = node->left->min or node->right->min;

    if( node->unary == 0 or node->unary == 1 )
    {
        node->min = false;
    }
}

void process_union_oper(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{
    // recursive call
    annotate_syntax_tree(node->left, automaton, mapping);
    annotate_syntax_tree(node->right, automaton, mapping);

    if( node->unary > 0 )
    {
        for(size_t i=0;i<node->right->rmost.size();++i)
        {
            for(size_t j=0;j<node->left->lmost.size();++j)
            {
                if(vmode) std::cout << node->right->rmost[i]+1 << " - " << mapping[node->left->lmost[j]] << " -> " << node->left->lmost[j]+1 << "\n";
                automaton->add_edge(node->right->rmost[i]+1 , node->left->lmost[j]+1, mapping[node->left->lmost[j]]);
            }

            if( node->right->unary < 1 )
            {
                for(size_t j=0;j<node->right->lmost.size();++j)
                {
                    if(vmode) std::cout << node->right->rmost[i]+1 << " - " << mapping[node->right->lmost[j]] << " -> " << node->right->lmost[j]+1 << "\n";
                    automaton->add_edge(node->right->rmost[i]+1 , node->right->lmost[j]+1, mapping[node->right->lmost[j]]);
                }
            }
        }

        for(size_t i=0;i<node->left->rmost.size();++i)
        {
            for(size_t j=0;j<node->right->lmost.size();++j)
            {
                if(vmode) std::cout << node->left->rmost[i]+1 << " - " << mapping[node->right->lmost[j]] << " -> " << node->right->lmost[j]+1 << "\n";
                automaton->add_edge(node->left->rmost[i]+1 , node->right->lmost[j]+1, mapping[node->right->lmost[j]]);
            }

            if( node->right->unary < 1 )
            {
                for(size_t j=0;j<node->left->lmost.size();++j)
                {
                    if(vmode) std::cout << node->left->rmost[i]+1 << " - " << mapping[node->left->lmost[j]] << " -> " << node->left->lmost[j]+1 << "\n";
                    automaton->add_edge(node->left->rmost[i]+1 , node->left->lmost[j]+1, mapping[node->left->lmost[j]]);
                }
            }
        }
    }

    node->lmost = node->left->lmost;
    node->lmost.insert( node->lmost.end(), node->right->lmost.begin(), node->right->lmost.end() );
    node->left->lmost.clear();
    node->left->rmost.clear();

    node->rmost = node->left->rmost;
    node->rmost.insert( node->rmost.end(), node->right->rmost.begin(), node->right->rmost.end() );
    node->right->rmost.clear();
    node->right->lmost.clear();

    node->min = node->left->min or node->right->min;

    // if ? or * operator then a source can be found in the right branch
    // this is not true for + 
    if( node->unary == 0 or node->unary == 1 )
    {
        node->min = false;
    }
}

void process_symbol_oper(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{
    node->lmost.push_back(node->value);
    node->rmost.push_back(node->value);

    // if case of a + or * operator create a self-loop
    if( node->unary > 0 )
    {
        if(vmode) std::cout << node->value+1 << " - " << mapping[node->value] << " -> " << node->value+1 << "\n";
        automaton->add_edge(node->value+1, node->value+1, mapping[node->value]);
    }

    node->min = true;

    if( node->unary == 0 or node->unary == 1 )
    {
        node->min = false;
    }
}

void annotate_syntax_tree(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{   

    if( op_char(node->type) == '_' )
    {
        process_concat_oper(node, automaton, mapping);  
    }
    else if( op_char(node->type) == '|' )
    {
        process_union_oper(node, automaton, mapping); 
    }
    else
    {
        process_symbol_oper(node, automaton, mapping);  
    }
}

void compute_source_states(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{   

    if( op_char(node->type) == 'u' )
    {
        if(vmode) std::cout << "0 - " << mapping[node->value] << " -> " << node->value+1 << "\n";
        automaton->add_edge(0, node->value+1 , mapping[node->value]);
    }
    else
    {
        compute_source_states(node->left, automaton, mapping);

        if( op_char(node->type) == '|' or (not node->left->min) )
        {
            compute_source_states(node->right, automaton, mapping);
        }
    }
}

void compute_final_states(stree_node* node, NFA* automaton)
{   

    if( op_char(node->type) == 'u' )
    {
        if(vmode) std::cout << node->value + 1 << "\n";
        automaton->add_final_state(node->value + 1);
    }
    else
    {
        compute_final_states(node->right, automaton);

        if( op_char(node->type) == '|' or (not node->right->min) )
        {
            compute_final_states(node->left, automaton);
        }
    }
}

NFA* compute_glushkov_automaton(std::string regexp, size_t sigma, bool verb)
{
    vmode = verb;
    NFA* glushkov_automaton = new NFA(compute_postfix_format(regexp)+1,sigma); 

    if(vmode) std::cout << "###### Postfix regexp: " << regexp << "\n";

    std::vector<char> mapping;
    stree_node *syntax_root = compute_syntax_tree(regexp,mapping);

    if(vmode){

    std::cout << "###### Syntax tree of: " << regexp << "\n";

    printBT(syntax_root);

    std::cout << "###### Computing the Glushkov automaton for: " << regexp << "\n";
    }

    annotate_syntax_tree(syntax_root, glushkov_automaton, mapping);

    compute_source_states(syntax_root, glushkov_automaton, mapping);

    mapping.clear();

    compute_final_states(syntax_root, glushkov_automaton);

    return glushkov_automaton;
}

#endif
