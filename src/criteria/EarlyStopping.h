/*
 * EarlyStopping.h
 *
 *  Created on: Nov 18, 2009
 *      Author: brandmaier
 */

#ifndef EARLYSTOPPING_H_
#define EARLYSTOPPING_H_

#include "../trainer/Trainer.h"
#include "../Network.h"
#include "Criterion.h"

/**
 * Early Stopping Criterion
 *
 *
 *
 */
struct EarlyStopping : Criterion
{

	vector<double> errors;
	unsigned int count;


	EarlyStopping()
	{
		count = 0;

	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{

		errors.push_back( network->test_sequence( trainer->validation_set ) );

		if (errors.size() >= 2) {
		 if (errors[errors.size()-1] < errors[errors.size()-2])
		 {
			 count = 0;
		 } else {
			 count++;
		 }
		}

		if (count > 1) {
			cout << "Early Stopping!" << endl;
			return true;
		} else {
			return false;
		}

	}
};

#endif /* EARLYSTOPPING_H_ */
