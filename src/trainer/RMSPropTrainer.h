/*
 * RMSPropTrainer.h
 *
 *  Created on: Jan 03, 2020
 *      Author: brandmaier
 */

#ifndef RMSPROPTRAINER_H_
#define RMSPROPTRAINER_H_

#include "Trainer.h"

struct RMSPropTrainer : Trainer
{
	RMSPropTrainer(Network* network);
	~RMSPropTrainer() {};

	void change_weight(Trainable* c);
	
	// RMS-Prop moving average
	static weight_t squared_gradient_moving_average;

	static weight_t beta;
	
	virtual string get_name()
	{
		return "RMSProp";
	}
};

#endif /* RMSPROPTRAINER_H_ */
