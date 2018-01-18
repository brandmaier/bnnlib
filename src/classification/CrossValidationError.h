/*
 * CrossValidationError.h
 *
 *  Created on: 15.04.2010
 *      Author: andreas
 */

#ifndef CROSSVALIDATIONERROR_H_
#define CROSSVALIDATIONERROR_H_

#include "../Network.h"
#include "../SequenceSet.h"
#include <vector>

struct CrossValidationError
{
	bool verbose;

	Network* network;
	SequenceSet* sequence_set;

	bool train_with_validation_set;

	CrossValidationError(Network* network, SequenceSet* sequence_set);
	vector<double>* evaluate(Trainer* trainer, ErrorFunction* err_func, unsigned int runs, unsigned int fold);

};

#endif /* CROSSVALIDATIONERROR_H_ */
