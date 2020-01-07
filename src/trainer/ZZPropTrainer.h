/*
 * ZZPropTrainer.h
 *
 *  Created on: Jan 06, 2020
 *      Author: brandmaier
 */

#ifndef ZZPROPTRAINER_H_
#define ZZPROPTRAINER_H_

#include "Trainer.h"

struct ZZPropTrainer : Trainer
{
	ZZPropTrainer(Network* network);
	~ZZPropTrainer() {};
	
	void change_weight(Trainable* c);
	
	// RMS-Prop moving average
	static weight_t squared_gradient_moving_average;

	static weight_t beta;
	
	virtual string get_name()
	{
		return "ZZProp";
	}
};

#endif /* ZZPROPTRAINER_H_ */
