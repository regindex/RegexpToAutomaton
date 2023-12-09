#ifndef GLUSHKOV_AUTOMATON_HPP
#define GLUSHKOV_AUTOMATON_HPP

#include "common.hpp"
#include "nfa.hpp"

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
    uint no_symbols = 0, j = 0;

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

    if( not (operators.count(regexp[len-1]) > 0) ){ no_symbols++; }
    regsep.push_back(regexp[len-1]);

    // create postfix regexp
    regexp.resize(len*2);
    std::memset(&regexp[0], '.', len*2);
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
    // resize regexp according to the final length
    regexp.resize(j);

    if(vmode) std::cout << "No. symbols in regexp: " << no_symbols << "\n";
    return no_symbols;
}

void compute_mbe_stree(stree_node *root)
{
    if( op_char(root->type) == '_' )
    {
        compute_mbe_stree(root->left); compute_mbe_stree(root->right);
        if( root->unary == 0 or root->unary == 1 ){  root->mbe = true; }
        else
        {
            root->mbe = root->left->mbe and root->right->mbe;
        }
    }
    else if( op_char(root->type) == '|' )
    {
        compute_mbe_stree(root->left); compute_mbe_stree(root->right);
        if( root->unary == 0 or root->unary == 1 ){  root->mbe = true; }
        else
        {
            root->mbe = root->left->mbe or root->right->mbe;
        }
    }
    else
    {
        if( root->unary == 0 or root->unary == 1 ){  root->mbe = true; }
    }
}

void propagate_kleene(stree_node *root, bool propagate)
{
    if( propagate ){ root->unary = 1; }

    if( op_char(root->type) == '_' )
    {
        if( root->unary >= 1 )
        {
            if( root->right->mbe ){ propagate_kleene(root->left, true); }
            else{ propagate_kleene(root->left, false); }
            if( root->left->mbe ){ propagate_kleene(root->right, true); }
            else{ propagate_kleene(root->right, false); }
        }
        else{ propagate_kleene(root->left, false); propagate_kleene(root->right, false); }
    }
    else if( op_char(root->type) == '|' )
    {
        if( root->unary >= 1 )
        { propagate_kleene(root->right, true); propagate_kleene(root->left, true); }
        else{ propagate_kleene(root->right, false); propagate_kleene(root->left, false); }
    } 
}

void process_concat_oper(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{   
    // recursive call
    annotate_syntax_tree(node->left, automaton, mapping);
    annotate_syntax_tree(node->right, automaton, mapping);
    // 
    for(size_t i=0;i<node->left->rmost.size();++i)
    {
        for(size_t j=0;j<node->right->lmost.size();++j)
        {
            if(vmode) std::cout << node->left->rmost[i]+1 << " - " << mapping[node->right->lmost[j]] << " -> " << node->right->lmost[j]+1 << "\n";
            automaton->add_edge(node->left->rmost[i]+1, node->right->lmost[j]+1, mapping[node->right->lmost[j]]);
        }
    }
    // if we have a * or + node
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
    // dire se c'è ? o * prendere entrambi
    if( node->left->mbe )
    {
        node->lmost.insert( node->lmost.end(), node->right->lmost.begin(), node->right->lmost.end() );
    }

    node->rmost = node->right->rmost;
    if( node->right->mbe )
    {
        node->rmost.insert( node->rmost.end(), node->left->rmost.begin(), node->left->rmost.end() );
    }

    node->left->lmost.clear(); node->left->rmost.clear(); 
    node->right->rmost.clear(); node->right->lmost.clear();
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
        }

        for(size_t i=0;i<node->left->rmost.size();++i)
        {
            for(size_t j=0;j<node->right->lmost.size();++j)
            {
                if(vmode) std::cout << node->left->rmost[i]+1 << " - " << mapping[node->right->lmost[j]] << " -> " << node->right->lmost[j]+1 << "\n";
                automaton->add_edge(node->left->rmost[i]+1 , node->right->lmost[j]+1, mapping[node->right->lmost[j]]);
            }
        }
    }

    node->lmost = node->left->lmost;
    node->lmost.insert( node->lmost.end(), node->right->lmost.begin(), node->right->lmost.end() );
    node->left->lmost.clear(); node->right->lmost.clear();

    node->rmost = node->left->rmost;
    node->rmost.insert( node->rmost.end(), node->right->rmost.begin(), node->right->rmost.end() );
    node->right->rmost.clear(); node->left->rmost.clear();
}

void process_symbol_oper(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{
    // initialize lmost and rmost vectors
    node->lmost.push_back(node->value);
    node->rmost.push_back(node->value);

    // if case of a + or * operator create a self-loop
    if( node->unary > 0 )
    {
        if(vmode) std::cout << node->value+1 << " - " << mapping[node->value] << " -> " << node->value+1 << "\n";
        automaton->add_edge(node->value+1, node->value+1, mapping[node->value]);
    }
}

void annotate_syntax_tree(stree_node* node, NFA* automaton, std::vector<char>& mapping)
{   
    // check type of node
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

        if( op_char(node->type) == '|' or node->left->mbe )
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

        if( op_char(node->type) == '|' or node->right->mbe )
        {
            compute_final_states(node->left, automaton);
        }
    }
}

void check_0_final(stree_node* root, NFA* automaton)
{
    if( root->mbe )
    {
        automaton->add_final_state(0);
        if(vmode) std::cout << "0\n";
    }
}

/*
    MAIN FUNCTION CONSTRUCTING GLUSHKOV AUTOMATON
*/
NFA* compute_glushkov_automaton(std::string regexp, size_t sigma, bool verb, bool to_stdout)
{
    vmode = verb;
    NFA* glushkov_automaton = new NFA(compute_postfix_format(regexp)+1,sigma); 

    //if(vmode) std::cout << "###### Postfix regexp: " << regexp << "\n";

    std::vector<char> mapping;
    stree_node *syntax_root = compute_syntax_tree(regexp,mapping);
    compute_mbe_stree(syntax_root);

    if(vmode){

    std::cout << "###### Syntax tree:\n";

    printBT(syntax_root);

    std::cout << "###### Computing the Glushkov automaton:\n";
    }

    propagate_kleene(syntax_root,false);

    annotate_syntax_tree(syntax_root, glushkov_automaton, mapping);

    compute_source_states(syntax_root, glushkov_automaton, mapping);
    mapping.clear();

    compute_final_states(syntax_root, glushkov_automaton);
    check_0_final(syntax_root, glushkov_automaton);

    if( to_stdout )
    {
        if(vmode) std::cout << "###### Final automaton to stdout:" << std::endl;
        // print stats
        std::cout << glushkov_automaton->no_nodes() << " " << glushkov_automaton->no_edges() << " 0 " << glushkov_automaton->get_finals()->size() << "\n";
        // print transitions
        glushkov_automaton->print_transitions_NFA();
        // print finals
        for(uint i=0;i<glushkov_automaton->get_finals()->size();++i)
        {
            std::cout << (*glushkov_automaton->get_finals())[i] << "\n";
        }
    }

    return glushkov_automaton;
}

#endif