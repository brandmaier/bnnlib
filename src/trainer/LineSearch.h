/*
 * LineSearch.h
 *
 *  Created on: Dec 22, 2009
 *      Author: brandmaier
 */

#ifndef LINESEARCH_H_
#define LINESEARCH_H_

#include "Trainer.h"
#include "../Network.h"
#include "../SequenceSet.h"

class LineSearch : Trainer {
public:

	double best_alpha;

	LineSearch(Network* network, vector<weight_t>* direction);
	virtual ~LineSearch();

	void train(SequenceSet* sequenceset, unsigned int iterations);

	void change_weight(Trainable* conn);

	virtual string get_name()
	{
		return "LineSearch";
	}

private:
	vector<weight_t>* direction;
	Network* network;

	double evaluate(vector<weight_t>* weights, double alpha, SequenceSet* sequenceset);
};

#endif /* LINESEARCH_H_ */
