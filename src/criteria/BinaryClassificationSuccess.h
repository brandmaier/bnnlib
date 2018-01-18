/*
 * BinaryClassificationSuccess.h
 *
 *  Created on: Nov 9, 2009
 *      Author: brandmaier
 */

#ifndef BINARYCLASSIFICATIONSUCCESS_H_
#define BINARYCLASSIFICATIONSUCCESS_H_

struct BinaryClassificationSuccess : Criterion
{
	SequenceSet* sequence;

	BinaryClassificationSuccess()
	{
		this->sequence = NULL;
	}

	BinaryClassificationSuccess(SequenceSet* sequence)
	{
		this->sequence = sequence;
	}

	bool finished(Network* network, Trainer* trainer, SequenceSet* sequence)
	{
		SequenceSet* testSet;
		if (this->sequence != NULL) testSet = this->sequence;
		else testSet = sequence;

		if (network->get_percent_correct_binary_decision(testSet, 0) >= 1)
		{
			return true;
		}

		return false;


	}
};


#endif /* BINARYCLASSIFICATIONSUCCESS_H_ */
