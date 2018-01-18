/*
 * RPropTrainer.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef RPROPTRAINER_H_
#define RPROPTRAINER_H_

#include "Trainer.h"

struct RPropTrainer : Trainer
{
	RPropTrainer(Network* network);
	~RPropTrainer() {};

	void change_weight(Trainable* c);

	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;

	virtual string get_name()
	{
		return "ResilientProp";
	}
};

#endif /* RPROPTRAINER_H_ */
