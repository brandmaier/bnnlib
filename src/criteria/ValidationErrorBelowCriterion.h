/*
 * ZeroErrorCriterion.h
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */

#ifndef VALIDATIONERRORBELOWCRITERION_H_
#define VALIDATIONERRORBELOWCRITERION_H_

struct ValidationErrorBelowCriterion : Criterion
{
	ErrorFunction* error_function;
	double threshold;

	ValidationErrorBelowCriterion(ErrorFunction* error_function, double threshold)
	{
		this->error_function = error_function;
		this->threshold = threshold;
	}


	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		double error = network->evaluate_validation_error(sequence, error_function);
		cout << "Error condition: " << error << " <? " << threshold << endl;
		return(error <= threshold);

	}

};

#endif /* ZEROERRORCRITERION_H_ */
