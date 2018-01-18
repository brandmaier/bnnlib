/*
 * BackpropTrainer.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef BACKPROPTRAINER_H_
#define BACKPROPTRAINER_H_

#include "Trainer.h"

struct BackpropTrainer : Trainer
{


	BackpropTrainer(Network* network);
	~BackpropTrainer() {}

	void change_weight(Trainable* c);

	virtual string get_name()
	{
		return "BackPropagation";
	}
};

#endif /* BACKPROPTRAINER_H_ */
