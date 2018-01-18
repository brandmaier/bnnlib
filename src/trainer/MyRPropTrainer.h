/*
 * MyRPropTrainer.h
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#ifndef MyRPROPTRAINER_H_
#define MyRPROPTRAINER_H_

#include "Trainer.h"
#include "../Trainable.h"

struct MyRPropTrainer : Trainer
{
	MyRPropTrainer(Network* network);
	~MyRPropTrainer() {};

//	void change_weights();
	void change_weight(Trainable* c);

	// R-Prop constants
	static weight_t eta_minus;
	static weight_t eta_plus;
	static weight_t gamma_max;
	static weight_t gamma_min;
};

#endif /* MyRPropTRAINER_H_ */
