/*
 * DelayEnsemble.h
 *
 *  Created on: Jan 18, 2010
 *      Author: brandmaier
 */

#ifndef DELAYENSEMBLE_H_
#define DELAYENSEMBLE_H_

#include "Ensemble.h"
#include "../nodes/Node.h"

struct DelayEnsemble : Ensemble
{
	unsigned int delay;

	DelayEnsemble( Ensemble* from_ensemble, unsigned int delay );
	DelayEnsemble( vector<Node*>* nodes );
	~DelayEnsemble() {};

	virtual const unsigned int get_type() { return Ensemble::DELAY_ENSEMBLE; }

	string to_string();
//	static DelayEnsemble* from_string(string from_string);
	static DelayEnsemble* from_string(string from_string, vector<Node*>* nodes);


};

#endif /* DELAYENSEMBLE_H_ */
