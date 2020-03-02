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
	
	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;
	
	
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
