/*
 * ADAMTrainer.h
 *
 *  Created on: Jan 03, 2020
 *      Author: brandmaier
 */

#ifndef ADAMTRAINER_H_
#define ADAMTRAINER_H_

#include "Trainer.h"

struct ADAMTrainer : Trainer
{
	ADAMTrainer(Network* network);
	~ADAMTrainer() {};

	void change_weight(Trainable* c);
	

	static weight_t beta1;
	static weight_t beta2;
	
	virtual string get_name()
	{
		return "ADAM";
	}
};

#endif /* ADAMTRAINER_H_ */
