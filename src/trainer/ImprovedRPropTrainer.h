/*
 * ImprovedRPropTrainer.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef IMPROVEDRPROPTRAINER_H_
#define IMPROVEDRPROPTRAINER_H_

#include "Trainer.h"
#include "../Trainable.h"

/**
 * Realization of iRprop+ training algorithm. For details see Igel & Hüsken,
 * "Improving the Rprop learning algorithm", 2000
 */
struct ImprovedRPropTrainer : Trainer
{
	ImprovedRPropTrainer(Network* network);
	~ImprovedRPropTrainer() {};

	void change_weight(Trainable* c);

	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;

	virtual string get_name()
	{
		return "IRProp-";
	}
};

#endif /* ImprovedRPropTRAINER_H_ */
