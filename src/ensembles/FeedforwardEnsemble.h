#ifndef FEEDFORWARDENSEMBLE_H_
#define FEEDFORWARDENSEMBLE_H_

#include "Ensemble.h"

struct FeedforwardEnsemble : Ensemble
{
	int node_type;
	
	FeedforwardEnsemble( int node_type, unsigned int size);
	FeedforwardEnsemble(vector<Node*>* nodes);
	~FeedforwardEnsemble() {};
	
	virtual const unsigned int get_type() { return FEEDFORWARD_ENSEMBLE; }
	
	string to_string();
	static FeedforwardEnsemble* from_string(string from_string);
	static FeedforwardEnsemble* from_string(string from_string, vector<Node*>* nodes);	
};

#endif /*FEEDFORWARDENSEMBLE_H_*/
