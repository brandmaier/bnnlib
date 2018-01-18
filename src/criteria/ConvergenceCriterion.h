/*
 * ConvergenceCriterion.h
 *
 *  Created on: Nov 13, 2009
 *      Author: brandmaier
 */

#ifndef CONVERGENCECRITERION_H_
#define CONVERGENCECRITERION_H_

#include "Criterion.h"
#include "../trainer/Trainer.h"

struct ConvergenceCriterion : Criterion
{
	double threshold;

	ConvergenceCriterion(void)
	{
		threshold = 10e-9;
	}

	ConvergenceCriterion(double threshold)
	{
		this->threshold = threshold;
	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		//cout << trainer->error_train.size() << " " << trainer->error_train[0] << endl;
		if (trainer->error_train.size() > 2) {
		 double diff = trainer->error_train[trainer->error_train.size()-1]
		               -trainer->error_train[trainer->error_train.size()-2];
		 //std::cout << trainer->error_train.size()<<".:"<< "Diff" << diff << endl;
		 if (abs(diff) < threshold) {
			std::cout << "Training converged!  diff < " << diff << endl;
			return true;
		 }
		}

		return false;
	}


};

#endif /* CONVERGENCECRITERION_H_ */
