/*
 * ConfusionMatrix.h
 *
 *  Created on: Dec 15, 2009
 *      Author: brandmaier
 */

#ifndef CONFUSIONMATRIX_H_
#define CONFUSIONMATRIX_H_

#include "../Network.h"
#include "../SequenceSet.h"
#include <vector>

class ConfusionMatrix {

private:
	Network* network;
	SequenceSet* sequences;

	double weighted_acc;
	double acc;

public:
	ConfusionMatrix(Network* network,  SequenceSet* sequences);
	virtual ~ConfusionMatrix();

	void create();
	void create_with_entropythreshold(double entropy_threshold);

	double get_balanced_accuracy()
	{
		return weighted_acc;
	}

	double get_accuracy()
	{
		return acc;
	}

};

#endif /* CONFUSIONMATRIX_H_ */
