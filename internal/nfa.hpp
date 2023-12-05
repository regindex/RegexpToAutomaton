#ifndef NFA_HPP_
#define NFA_HPP_

#include "common.hpp"
typedef std::unordered_map<char, std::vector<uint>> ntype;

// class for directed unidirectional unlabeled NFA
class NFA{

public:
	// empty constructor
	NFA(){}
	// empty constructor
	NFA(int sigma_)
	{
		this->no_states = 0;
		this->sigma = sigma_;
		this->no_transitions = 0;
	}
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
	int get_sigma(){ return sigma; }
	std::unordered_map<char, std::vector<uint>>* get_node(uint i)
	{
		return transitions[i];
	}
	
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

	void add_empty_state()
	{
		transitions.push_back(new ntype());
		no_states++;
	}

	uint add_state(ntype* node, uint tran){

		// insert node
		transitions.push_back(node);

		// increment edge number
		no_transitions += tran;
		// increment states number
		no_states++;

		return transitions.size();
	}

	void add_final_state(uint id)
	{
		final.push_back(id);
	}

	std::vector<uint>* get_finals()
	{
		return &final;
	}

	void print_transitions()
	{
		for(uint i=0;i<transitions.size();++i)
		{
			for (auto const& [l, edges] : *transitions[i] )
	        {
	        	std::cout << i << " " << (int)l << " " << edges[0] << "\n";
			}
		}
	}

	void print_transitions_NFA()
	{
		for(uint i=0;i<transitions.size();++i)
		{
			for (auto const& [l, edges] : *transitions[i] )
	        {
	        	for(uint j=0;j<edges.size();++j)
					std::cout << i << " " << (int)l << " " << edges[j] << "\n";
			}
		}
	}
	

private:
	// number of states in the DFA
	uint no_states;
	// number of transitions in the DFA
	uint no_transitions;
	// vector containing all edges
	std::vector< ntype* > transitions;
	// final states
	std::vector<uint> final;
	// alphabet size
	int sigma;
};


#endif