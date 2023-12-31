/* Code taken from https://github.com/WalkerCodeRanger/dfaMinimizationComparison*/

#ifndef DFA_MINIMIZATION_HPP
#define DFA_MINIMIZATION_HPP

#include <iostream>
#include <algorithm>
#include "nfa.hpp"

/* Refinable partition */
// These two structures are shared by both instances of partition, saving memory
int* marked; // Marked[s] the number of marked elements in set s
int* touched; // touched (i.e. contain marked) sets
int touchedCount = 0; // temporary worksets
struct Partition
{
	int setCount; // z - the number of sets
	int* elements;
	int* location;
	int* setOf;
	int* first;
	int* past;

	void init(int n)
	{
		setCount = bool(n);
		elements = new int[n];
		location = new int[n];
		setOf = new int[n];
		first = new int[n];
		past = new int[n];
		for (int i = 0; i < n; ++i)
		{
			elements[i] = location[i] = i;
			setOf[i] = 0;
		}
		if (setCount)
		{
			first[0] = 0;
			past[0] = n;
		}
	}

	void mark(int element)
	{
		// swap element with the first unmarked
		int set = setOf[element];
		int i = location[element];
		int j = first[set] + marked[set];
		// warning, function here relies on the fact that you won't call mark on an element that is already marked
		elements[i] = elements[j];
		location[elements[i]] = i;
		elements[j] = element;
		location[element] = j;
		// increment the number of marked and add to touched if needed
		if (!marked[set]++)
		{
			touched[touchedCount++] = set;
		}
	}

	void split()
	{
		while (touchedCount)
		{
			int set = touched[--touchedCount];
			int firstUnmarked = first[set] + marked[set];
			if (firstUnmarked == past[set])
			{
				marked[set] = 0;
				continue;
			}

			// Make the smaller half a new set
			// If same size, then make a new set out of unmarked
			if (marked[set] <= past[set] - firstUnmarked)
			{
				first[setCount] = first[set];
				past[setCount] = first[set] = firstUnmarked;
			}
			else
			{
				past[setCount] = past[set];
				first[setCount] = past[set] = firstUnmarked;
			}
			for (int i = first[setCount]; i < past[setCount]; ++i)
			{
				setOf[elements[i]] = setCount;
			}
			marked[setCount++] = 0;
			marked[set] = 0;
		}
	}
};

Partition blocks; // blocks (consist of states)
Partition cords; // cords (consist of transitions)
int stateCount; // number of states
int transitionCount; // number of transitions
int finalStatesCount; // number of final states
int initialState; // initial state
int* transitionTail; // tails of transitions (i.e. to state)
int* transitionLabel; // labels of transitions (i.e. on what input)
int* transitionHead; // heads of transitions (i.e. from state)
bool cmp(int i, int j)
{
	return transitionLabel[i] < transitionLabel[j];
}

/* Adjacent transitions */
int* adjacent;
int* offset;
void make_adjacent(int states[])
{
	for (int state = 0; state <= stateCount; ++state)
		offset[state] = 0;

	for (int transition = 0; transition < transitionCount; ++transition)
		++offset[states[transition]];

	for (int state = 0; state < stateCount; ++state)
		offset[state + 1] += offset[state];

	for (int transition = transitionCount; transition--; )
		adjacent[--offset[states[transition]]] = transition;
}

/* Removal of irrelevant parts */
int reachableCount = 0; // number of reached states
inline void reach(int state)
{
	int i = blocks.location[state];
	if (i >= reachableCount)
	{
		blocks.elements[i] = blocks.elements[reachableCount];
		blocks.location[blocks.elements[i]] = i;
		blocks.elements[reachableCount] = state;
		blocks.location[state] = reachableCount++;
	}
}

void rem_unreachable(int tail[], int head[])
{
	make_adjacent(tail);
	// walk the DFA graph marking reachable states
	for (int i = 0; i < reachableCount; ++i)
	{
		for (int j = offset[blocks.elements[i]]; j < offset[blocks.elements[i] + 1]; ++j)
			reach(head[adjacent[j]]);
	}
	// remove unreachable states and transitions
	int j = 0;
	for (int t = 0; t < transitionCount; ++t)
	{
		if (blocks.location[tail[t]] < reachableCount)
		{
			head[j] = head[t];
			transitionLabel[j] = transitionLabel[t];
			tail[j] = tail[t];
			++j;
		}
	}
	transitionCount = j;
	blocks.past[0] = reachableCount;
	reachableCount = 0;
}

/* Main program */
void minimize_DFA(NFA* A)
{
	stateCount = A->no_nodes();
	transitionCount = A->no_edges();
	initialState = 0;
	finalStatesCount = A->get_finals()->size();
	/* Read sizes and reserve most memory */
	transitionTail = new int[transitionCount];
	transitionLabel = new int[transitionCount];
	transitionHead = new int[transitionCount];
	blocks.init(stateCount);
	adjacent = new int[transitionCount];
	offset = new int[stateCount + 1];
	/* Read transitions */
	int x = 0;
	for (int t = 0; t < stateCount; ++t)
	{
		ntype* node = A->get_node(t);
	    for (auto const& [l, edges] : *node)
	    {
	    	transitionTail[x] = t;
	    	transitionLabel[x] = l;
	    	transitionHead[x++] = edges[0];
	    }
	}
	/* Remove states that cannot be reached
	from the initial state, and from which
	final states cannot be reached */
	reach(initialState);
	rem_unreachable(transitionTail, transitionHead);
	for (int i = 0; i < finalStatesCount; ++i)
	{
		int q;
		q = (*A->get_finals())[i];
		if (blocks.location[q] < blocks.past[0])
		{
			reach(q);
		}
	}
	finalStatesCount = reachableCount;
	rem_unreachable(transitionHead, transitionTail);
	/* Make initial partition */
	touched = new int[transitionCount + 1];
	marked = new int[transitionCount + 1];
	marked[0] = finalStatesCount;
	if (finalStatesCount)
	{
		touched[touchedCount++] = 0;
		blocks.split();
	}
	/* Make transition partition */
	cords.init(transitionCount);
	if (transitionCount)
	{
		std::sort(cords.elements, cords.elements + transitionCount, cmp);
		cords.setCount = marked[0] = 0;
		// this code relies on the fact that cords.first[0] == 0 at this point for the first set to be correct
		int currentLabel = transitionLabel[cords.elements[0]];
		for (int i = 0; i < transitionCount; ++i)
		{
			int t = cords.elements[i];
			if (transitionLabel[t] != currentLabel)
			{
				currentLabel = transitionLabel[t];
				cords.past[cords.setCount++] = i;
				cords.first[cords.setCount] = i;
				marked[cords.setCount] = 0;
			}
			cords.setOf[t] = cords.setCount;
			cords.location[t] = i;
		}
		cords.past[cords.setCount++] = transitionCount;
	}

	/* Split blocks and cords */
	make_adjacent(transitionHead);
	int b = 1, c = 0, i, j;
	while (c < cords.setCount)
	{
		for (i = cords.first[c]; i < cords.past[c]; ++i)
		{
			blocks.mark(transitionTail[cords.elements[i]]);
		}
		blocks.split(); ++c;
		while (b < blocks.setCount)
		{
			for (i = blocks.first[b]; i < blocks.past[b]; ++i)
			{
				for (j = offset[blocks.elements[i]]; j < offset[blocks.elements[i] + 1]; ++j)
				{
					cords.mark(adjacent[j]);
				}
			}
			cords.split(); ++b;
		}
	}
	/* Count the numbers of transitions
	and final states in the result */
	int mo = 0, fo = 0;
	for (int t = 0; t < transitionCount; ++t)
	{
		if (blocks.location[transitionTail[t]] == blocks.first[blocks.setOf[transitionTail[t]]])
		{
			++mo;
		}
	}
	for (int b = 0; b < blocks.setCount; ++b)
	{
		if (blocks.first[b] < finalStatesCount)
		{
			++fo;
		}
	}
	/* Print the result */
	std::cout << blocks.setCount << ' ' << mo << ' ' << blocks.setOf[initialState] << ' ' << fo << '\n';
	for (int t = 0; t < transitionCount; ++t)
	{
		if (blocks.location[transitionTail[t]] == blocks.first[blocks.setOf[transitionTail[t]]])
		{
			std::cout << blocks.setOf[transitionTail[t]] << ' ' << transitionLabel[t] << ' ' << blocks.setOf[transitionHead[t]] << '\n';
		}
	}
	for (int b = 0; b < blocks.setCount; ++b)
	{
		if (blocks.first[b] < finalStatesCount)
		{
			std::cout << b << '\n';
		}
	}
}

#endif