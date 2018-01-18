/*
 * Criterion.h
 *
 *  Created on: Nov 9, 2009
 *      Author: brandmaier
 */

#ifndef CRITERION_H_
#define CRITERION_H_

#include "../Network.h"
#include "../SequenceSet.h"
#include "../trainer/Trainer.h"

struct Trainer;
struct Network;

struct Criterion
{
	//Criterion() {};
	virtual ~Criterion() {};

	virtual bool finished(Network* network, Trainer* trainer, SequenceSet* sequence) = 0;

};

#endif /* CRITERION_H_ */
