#ifndef STOCHASTICFEEDFORWARDENSEMBLE_H_
#define STOCHASTICFEEDFORWARDENSEMBLE_H_

#include "Ensemble.h"

struct StochasticFeedforwardEnsemble : Ensemble
{
	int node_type;
	
	StochasticFeedforwardEnsemble( unsigned int size );
	StochasticFeedforwardEnsemble( unsigned int size, weight_t gain, weight_t bias, weight_t stochastic_range);
	StochasticFeedforwardEnsemble(vector<Node*>* nodes);
	~StochasticFeedforwardEnsemble() {};
	
	virtual const unsigned int get_type() { return STOCHASTIC_FEEDFORWARD_ENSEMBLE; }
	
	string to_string();
	static StochasticFeedforwardEnsemble* from_string(string from_string);
	static StochasticFeedforwardEnsemble* from_string(string from_string, vector<Node*>* nodes);	
	
	void set_stochastic_range(weight_t x);
};

#endif /*STOCHASTICFEEDFORWARDENSEMBLE_H_*/
