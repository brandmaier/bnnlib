/*
 * ARPropTrainer.h - Asymmetric Resilient Propagation
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef ARPropTRAINER_H_
#define ARPropTRAINER_H_

#include "../functions.h"
#include "../Network.h"
#include "Trainer.h"
#include "../Trainable.h"

struct ARPropTrainer : public Trainer
{
	ARPropTrainer(Network* network);
	~ARPropTrainer();

	void change_weight(Trainable* c);

	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;

	unsigned int q = 0;
	
	weight_t forgetting_discount;
	weight_t output_discount;
	weight_t input_discount;
	
	bool phase = true;

	virtual string get_name()
	{
		return "Asymmetric RProp";
	}
	
	virtual void prepare_step();
};

#endif /* ARPropTRAINER_H_ */
