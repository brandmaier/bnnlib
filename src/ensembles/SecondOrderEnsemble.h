/*
 * SecondOrderEnsemble.h
 *
 *  Created on: Dec 7, 2009
 *      Author: brandmaier
 */

#ifndef SECONDORDERENSEMBLE_H_
#define SECONDORDERENSEMBLE_H_

#include "Ensemble.h"
#include "../nodes/Node.h"
#include "../nodes/TanhNode.h"
#include "../nodes/PiNode.h"
#include "../Network.h"
#include <vector>

/**
 * takes a vector of input nodes and an vector of output nodes.
 * The Ensemble will connect all input nodes to all output nodes
 * and, in addition, will create an intermediate layer of
 * multiplicative nodes of all combinations of two nodes of the input
 * layer
 *
 */
struct SecondOrderEnsemble : Ensemble
{
//	std::vector<Node*> outputs;
//	std::vector<Node*> inputs;
	bool include_additional_quadratic_terms;

//	SecondOrderEnsemble(unsigned int n);
	SecondOrderEnsemble(vector<Node*>* inputs, vector<Node*>* outputs);
	SecondOrderEnsemble(vector<Node*>* inputs, vector<Node*>* outputs,	bool include_additional_quadratic_terms );

	SecondOrderEnsemble(vector<Node*>* nodes);


	void init(vector<Node*>* inputs, vector<Node*>* outputs,	bool include_additional_quadratic_terms);

//	virtual std::vector<Node*>* get_inputs();
//	virtual std::vector<Node*>* get_outputs();

	virtual const unsigned int get_type() { return SECONDORDER_ENSEMBLE; }

	virtual string to_string();
//	static SecondOrderEnsemble* from_string(string from_string);
	static SecondOrderEnsemble* from_string(string from_string, vector<Node*>* nodes);


};

#endif /* SECONDORDERENSEMBLE_H_ */
