/*
 * GeneralizationLoss.h
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */

#ifndef GENERALIZATIONLOSS_H_
#define GENERALIZATIONLOSS_H_

/**
 * Generalization Loss Criterion according to Proben1
 *
 * a standard setting of GL_threshold could be 5. GL_threshold is given in percent,
 * so a value of 5 refers to 5%
 *
 */
struct GeneralizationLoss : Criterion
{
	double best_error;
	double GL_threshold;
	SequenceSet* validation_set;

	GeneralizationLoss(SequenceSet* validation_set, double GL_treshold)
	{
		best_error = 10e50;
		this->GL_threshold = GL_treshold;
		this->validation_set = validation_set;
	}

	GeneralizationLoss(SequenceSet* validation_set)
	{
		best_error = 10e50;
		this->GL_threshold = 5;
		this->validation_set = validation_set;
	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{

		// evaluate validation set

		double validation_error = network->evaluate_validation_error(validation_set);

		if (validation_error < best_error)
		{
			best_error = validation_error;
		}


		double GL = 100 * (validation_error/best_error - 1);

		cout << "current GL = " << GL << " ("<< validation_error << "/" << best_error << ")" << endl;

		return GL > GL_threshold;
	}

};

#endif /* GENERALIZATIONLOSS_H_ */
