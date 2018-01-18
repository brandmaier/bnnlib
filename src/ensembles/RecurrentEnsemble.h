#ifndef RECURRENTENSEMBLE_H_
#define RECURRENTENSEMBLE_H_

#include "Ensemble.h"

struct RecurrentEnsemble : Ensemble
{
	int node_type;
	
	RecurrentEnsemble( int node_type, unsigned int size);
	RecurrentEnsemble(vector<Node*>* nodes);
	~RecurrentEnsemble() {};

	virtual const unsigned int get_type() { return RECURRENT_ENSEMBLE; }

	virtual string to_string();
	static RecurrentEnsemble* from_string(string from_string);
	static RecurrentEnsemble* from_string(string from_string, vector<Node*>* nodes);

};

#endif /*RECURRENTENSEMBLE_H_*/
