#ifndef NFA_HPP_
#define NFA_HPP_

#include "common.hpp"

typedef uint32_t uint;

// class for directed unidirectional unlabeled NFA
class NFA{

public:
	// empty constructor
	NFA(){}
	// initialize the empty NFA
	NFA(uint no_states_, int sigma_)
	{
		// set number of nodes
		this->no_states = no_states_;
		this->sigma = sigma_;
		this->no_transitions = 0;
		// initialize DFA
		transitions.resize(no_states_);
		for(uint i=0;i<no_states_;++i)
		{
			transitions[i] = new std::unordered_map<char, std::vector<uint>>;
		}
	}

	uint no_nodes(){ return no_states; }
	uint no_edges(){ return no_transitions; }
	
	void add_edge(uint origin, uint dest, char label){

		// insert out edge
		auto it = transitions[origin]->find(label);
	    if (it == transitions[origin]->end())
	    {
	        transitions[origin]->insert(it, {label, std::vector<uint>{dest} });
	    }
	    else
	    {
	        it->second.push_back(dest);
	    }

		// increment edge number
		no_transitions++;
	}

	void add_final_state(uint id)
	{
		final.push_back(id);
	}
	

private:
	// number of states in the DFA
	uint no_states;
	// number of transitions in the DFA
	uint no_transitions;
	// vector containing all edges
	std::vector< std::unordered_map<char, std::vector<uint>>* > transitions;
	// final states
	std::vector<uint> final;
	// alphabet size
	int sigma;
};


#endif