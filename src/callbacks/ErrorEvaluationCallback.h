/*
 * DummyCallback.h
 *
 *  Created on: Jan 27, 2009
 *      Author: brandmaier
 */

#ifndef ERROREVALUATIONCALLBACK_H_
#define ERROREVALUATIONCALLBACK_H_

struct ErrorEvaluationCallback : CallbackHandler
{
	ErrorFunction* error_function;
	SequenceSet* sequence_set;

	ErrorEvaluationCallback(ErrorFunction* error_function) {
		this->error_function = error_function;
		this->sequence_set = NULL;
	}

	ErrorEvaluationCallback(ErrorFunction* error_function, SequenceSet* set) {
		this->error_function = error_function;
		this->sequence_set = set;
	}

	void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		double error = -1;

		if (this->sequence_set != NULL) {
			error = net->evaluate_error(set, error_function);
		} else {
			error = net->evaluate_error(set);
		}

		cout << "Current Error on "<< set->name << " : " << error << endl;
	}


};


#endif
