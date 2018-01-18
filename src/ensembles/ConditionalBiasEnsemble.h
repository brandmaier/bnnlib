/*
 * ConditionalBiasEnsemble.h
 *
 *  Created on: Oct 26, 2009
 *      Author: brandmaier
 */

#ifndef CONDITIONALBIASENSEMBLE_H_
#define CONDITIONALBIASENSEMBLE_H_

#include "Ensemble.h"
#include "../nodes/Node.h"

struct ConditionalBiasEnsemble : Ensemble
{
	ConditionalBiasEnsemble();
	ConditionalBiasEnsemble(vector<Node*>* nodes);
	~ConditionalBiasEnsemble();
	Node* bias0;
	Node* biasx;

	virtual const unsigned int get_type() { return CONDITIONAL_BIAS_ENSEMBLE; }

	string to_string();
	static ConditionalBiasEnsemble* from_string(string from_string);
	static ConditionalBiasEnsemble* from_string(string from_string, vector<Node*>* nodes);

};

#endif /* CONDITIONALBIASENSEMBLE_H_ */
