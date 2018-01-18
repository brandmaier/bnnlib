/*
 * CGTrainer.h
 *
 * Conjugate Gradients training can be seen as a Backpropagation method with an adaptive learning rate and
 * adaptive momentum term. The error surface is locally approximated via a quadratic function. For optimal
 * parameter settings, the Hessian is needed. In this implementation, the calculation of the Hessian is
 * replaced by approximations. Beta, the "momentum", is approximated via Fletcher-Reeves formula and alpha,
 * the "learning rate", is approximated by a line search.
 *
 *  Created on: 08.04.2010
 *      Author: andreas
 */

#ifndef CGTRAINER_H_
#define CGTRAINER_H_

#include "Trainer.h"
#include "../Network.h"

struct CGTrainer : Trainer
{
	double alpha,beta;	// "momentum" term
	SequenceSet* sequenceset;
	vector<weight_t>* original_weights;
	double tol;
	double initial_delta;
	unsigned int restart_cg_every;

	unsigned int _stat_num_bracket_runs;
	unsigned int _stat_num_gss_runs;

	CGTrainer(Network* network);
	~CGTrainer();

	void train(SequenceSet* sequenceset,SequenceSet* validationset,  unsigned int iterations);
	void change_weights();
	void change_weight(Trainable* c);

	double evaluate(vector<weight_t>* weights, double alpha);

	virtual string get_name()
	{
		return "Conjugate Gradient";
	}

};

#endif /* CGTRAINER_H_ */
