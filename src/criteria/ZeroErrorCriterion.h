/*
 * ZeroErrorCriterion.h
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */

#ifndef ZEROERRORCRITERION_H_
#define ZEROERRORCRITERION_H_

struct ZeroErrorCriterion : Criterion
{
	ErrorFunction* error_function;

	ZeroErrorCriterion(ErrorFunction* error_function)
	{
		this->error_function = error_function;
	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		double error = network->evaluate_error(sequence, error_function);
		//cout << "Error condition: " << error << endl;
		return(error == 0);

	}

};

#endif /* ZEROERRORCRITERION_H_ */
