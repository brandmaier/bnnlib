/*
 * ZeroErrorCriterion.h
 *
 *  Created on: Nov 19, 2009
 *      Author: brandmaier
 */

#ifndef ERRORBELOWCRITERION_H_
#define ERRORBELOWCRITERION_H_

struct ErrorBelowCriterion : Criterion
{
	ErrorFunction* error_function;
	double threshold;

	ErrorBelowCriterion(ErrorFunction* error_function, double threshold)
	{
		this->error_function = error_function;
		this->threshold = threshold;
	}


	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		double error = network->evaluate_error(sequence, error_function);
	//	cout << "Error condition: " << error << endl;
		return(error <= threshold);

	}

};

#endif /* ZEROERRORCRITERION_H_ */
