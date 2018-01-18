/*
 * ZeroErrorCriterion.h
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */

#ifndef ZEROVALIDATIONERRORCRITERION_H_
#define ZEROVALIDATIONERRORCRITERION_H_

struct ZeroValidationErrorCriterion : Criterion
{
	ErrorFunction* error_function;

	ZeroValidationErrorCriterion(ErrorFunction* error_function)
	{
		this->error_function = error_function;
	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		double error = network->evaluate_validation_error(sequence,  error_function);
		cout << "Error condition: " << error << endl;
		return(error == 0);

	}

};

#endif /* ZEROERRORCRITERION_H_ */
