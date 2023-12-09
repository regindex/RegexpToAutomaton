#ifndef POWERSET_CONSTRUCTION_HPP
#define POWERSET_CONSTRUCTION_HPP

#include "common.hpp"
#include "nfa.hpp"

// mask vector
const unsigned char mask[]={0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01}; 
// check if s[i] == 1
bool iget(const std::string* s, uint i)
{
    return ( ((*s)[(i)/8]&mask[(i)%8]) ? 1 : 0 );
}
// set s[i] = 1
void bset(std::string* s, uint i)
{
    (*s)[(i)/8]=(1) ? (mask[(i)%8]|(*s)[(i)/8]) : ((~mask[(i)%8])&(*s)[(i)/8]);
} 
// global variables
uint counter, nodes;
std::unordered_map<std::string, uint> M;
std::string* F;

void compute_closure(NFA* A, ntype* n, std::vector<uint>* S, std::vector<std::string*>& V)
{
    // iterate over all STARTING nodes in S
    for(uint i=0;i<S->size();++i)
    {
        if(A->get_node((*S)[i]) == nullptr){ continue; }
        // iterate over all labels of S[i]
        for (auto const& [l, edges] : *A->get_node((*S)[i]))
        {
            // initialize vector for l and visit vector
            if( n->find(l) == n->end() )
            { 
                n->insert({l,std::vector<uint>{}});
                V[l] = new std::string((nodes/8)+1, 0);
            }
            // iterate over all edges labeled with l
            for(uint j=0;j<edges.size();++j)
            {
                // check if the node has already been visited
                if( iget(V[l],edges[j]) == false )
                {
                    // mark the state as visited
                    bset(V[l],edges[j]);
                    // insert new edge
                    n->at(l).push_back(edges[j]);
                    // check if is final
                    if( iget(F,edges[j]) ){ bset(V[l],0); }
                }
            }
        }
    }
} 

void powerset_construction(NFA* A, NFA* dfa, std::vector<uint>* curr, uint b)
{
    // add a new state
    dfa->add_empty_state();
    // initialize new node
    ntype* node = new ntype();
    // initialize visit vectors
    std::vector<std::string*> visit(A->get_sigma(),nullptr);
    // compute closure for curr states
    compute_closure(A, node, curr, visit);
    // check new states
    for (auto const& [key, val] : *node)
    {
        //std::cout << key << " ";
        //std::cout << val.size() << "\n";
        if(M.find(*visit[key]) != M.end())
        {
            //std::cout << b << " - " << key << " -> " << M[*visit[key]] << "\n";
            dfa->add_edge(b,M[*visit[key]],key);
        }
        else
        {
            M[*visit[key]] = ++counter;
            dfa->add_edge(b,counter,key);
            if( iget(visit[key],0) ){ dfa->add_final_state(counter); } 
            //std::cout << b << " - " << key << " -> " << counter << "\n";
            powerset_construction(A,dfa,&node->at(key),counter);
        }
    }
}

void print_mapping(uint nodes)
{
    std::cout << "Mapping =\n";
    for (auto const& [key, val] : M)
    {
        std::cout << val << " -> {";
        for(uint i=0;i<nodes;++i)
        {
            if( iget(&key,i) ){ std::cout << i << ","; }
        }
        std::cout << "}" << std::endl;
    }
}

NFA* compute_powerset_construction(NFA* A, bool vmode = false, bool to_stdout = false)
{
    if(vmode) std::cout << "###### Computing the Glushkov DFA:\n";
    // initialize glushkov DFA
    NFA* glushkov_dfa = new NFA(A->get_sigma()); 
    nodes = A->no_nodes();
    counter = 0;
    std::vector<uint> curr{0};
    // final states bitvector
    F = new std::string((nodes/8)+1,0);
    for(uint i=0;i<A->get_finals()->size();++i)
    {
        bset(F,(*A->get_finals())[i]);
    }
    // run powerset construction starting from source state
    powerset_construction(A,glushkov_dfa,&curr,0);
    // check if the source is final
    if( iget(F,0) ) glushkov_dfa->add_final_state(0);

    if(vmode) print_mapping(nodes);

    if( to_stdout )
    {
        if(vmode) std::cout << "###### Final automaton to stdout:" << std::endl;
        // print stats
        std::cout << glushkov_dfa->no_nodes() << " " << glushkov_dfa->no_edges() << " 0 " << glushkov_dfa->get_finals()->size() << "\n";
        // print transitions
        glushkov_dfa->print_transitions();
        // print finals
        for(uint i=0;i<glushkov_dfa->get_finals()->size();++i)
        {
            std::cout << (*glushkov_dfa->get_finals())[i] << "\n";
        }
    }

    return glushkov_dfa;
}

#endif