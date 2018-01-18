/*
 * ClassificationCallback.h
 *
 * /!\ does not work properly with UNDEF targets
 *
 *  Created on: Mar 13, 2010
 *      Author: brandmaier
 */

#ifndef CLASSIFICATIONCALLBACK_H_
#define CLASSIFICATIONCALLBACK_H_

#include "../SequenceSet.h"
#include "../errorfunctions/WinnerTakesAllErrorFunction.h"

struct ClassificationCallback : CallbackHandler
{
	ErrorFunction* error_function;
	SequenceSet* sequence_set;

	bool mode;

	ClassificationCallback(SequenceSet* set)
	{
		init(set, false);
	}

	ClassificationCallback(SequenceSet* set, bool mode)
	{
		init(set, mode);
	}

	void init(SequenceSet* set, bool mode)
	{
		error_function = new WinnerTakesAllErrorFunction();
		sequence_set = set;
		this->mode = mode;
	}

	~ClassificationCallback()
	{
		delete error_function;
	}

	virtual void callback(Network* net,Trainer* trainer, SequenceSet* set)
	{
		double error = net->evaluate_error(this->sequence_set, error_function);

		unsigned int t_total = 0;

		if (mode) {
			t_total = this->sequence_set->size();
		} else {
		 for (unsigned int i=0; i < this->sequence_set->size();i++)
	 	 {
			t_total+=this->sequence_set->get(i)->size();
		 }
		}

		double error_percent = error / t_total * 100.0;

		cout << "Classification Error: " << error_percent << "% ("<<error<<","<<t_total <<","<<mode<<")" << endl;
	}

};

#endif /* CLASSIFICATIONCALLBACK_H_ */
