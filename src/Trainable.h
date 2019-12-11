/*
 * Trainable.h
 *
 *  Created on: Nov 26, 2009
 *      Author: Andreas Brandmaier
 */

#ifndef TRAINABLE_H_
#define TRAINABLE_H_

#include "functions.h" // defines weight_t

struct Trainable
{

	weight_t derivative;
	weight_t previous_derivative;
	weight_t gamma;
	weight_t previous_weight_change;
	weight_t weight;

	bool tag;	// bool tag for various purposes

	bool freeze_weight;

	Trainable()
	{
		init();
	}

	virtual ~Trainable() {};

	virtual void reset_derivative()
	{
		this->derivative = 0.0;
	}

	void init()
	{
		this->derivative = 0.0;
		this->previous_derivative = 0.0;
		this->previous_weight_change = 0.0;
		this->weight = rand_uniform_01()*0.4-0.2;
		this->gamma = 0.1;
		this->freeze_weight = false;
		this->tag = false;
	}

};

#endif /* TRAINABLE_H_ */
