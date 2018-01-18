#ifndef WINNERTAKESALLENSEMBLE_H_
#define WINNERTAKESALLENSEMBLE_H_

#include "FeedforwardEnsemble.h"
#include "../Sequence.h"

struct WinnerTakesAllEnsemble : FeedforwardEnsemble
{
	
	WinnerTakesAllEnsemble(int node_type, unsigned int size);
	WinnerTakesAllEnsemble(vector<Node*>* nodes);
	
	void _after_activation(unsigned int t);

	virtual const unsigned int get_type() { return WINNER_TAKES_ALL_ENSEMBLE; }

	string to_string();
	static WinnerTakesAllEnsemble* from_string(string from_string);
	static WinnerTakesAllEnsemble* from_string(string from_string, vector<Node*>* nodes);
	
};

#endif /*WINNERTAKESALLENSEMBLE_H_*/
