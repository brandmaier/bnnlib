/*
 * SARPropTrainer.h
 *  A Simulated Annealing Enhancement To Resilient Back Propagation
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef SARPROPTRAINER_H_
#define SARPROPTRAINER_H_

#include "Trainer.h"

struct SARPropTrainer : Trainer
{
	SARPropTrainer(Network* network);
	~SARPropTrainer() {};

	void change_weight(Trainable* c);

	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;

	static weight_t k1,k2,k3;

	double temperature;

	virtual string get_name()
	{
		return "SARProp";
	}

};

#endif /* SARPROPTRAINER_H_ */
