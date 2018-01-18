/*
 * ImprovedRPropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "ImprovedRPropTrainer.h"

weight_t ImprovedRPropTrainer::eta_minus = 0.5;
weight_t ImprovedRPropTrainer::eta_plus = 1.2;
weight_t ImprovedRPropTrainer::gamma_min = 0 ;//10e-6;
weight_t ImprovedRPropTrainer::gamma_max = 50;

ImprovedRPropTrainer::ImprovedRPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
}

void ImprovedRPropTrainer::change_weight(Trainable* c)
{
	if (c->freeze_weight == true)
	{
		c->previous_derivative = c->derivative;
		c->derivative = 0.0;
		c->previous_weight_change = 0.0;
		return;
	}


	weight_t sign = c->derivative*c->previous_derivative;
	weight_t change = 0;

	if (isnan(sign)) {
		std::cout << "Derivative has become NaN" << endl;
		exit(-1);
	}

	if (sign > 0) {
		c->gamma = fmin( c->gamma * ImprovedRPropTrainer::eta_plus, ImprovedRPropTrainer::gamma_max );

		 change = c->gamma * (c->derivative>=0?+1:-1);
	} else if (sign < 0) {
		c->gamma = fmax( c->gamma * ImprovedRPropTrainer::eta_minus, ImprovedRPropTrainer::gamma_min );

		// iImprovedRProp+
		if (current_error > previous_error) {
			change = -c->previous_weight_change;
		}

		// iImprovedRProp-
		c->derivative = 0;
	} else {
		 change = c->gamma * (c->derivative>=0?+1:-1);
	}



	//std::cout << " Change : " << change << " und sign " << sign <<
	//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;

	c->weight += change;
	c->previous_weight_change = change;
	c->previous_derivative = c->derivative;
	c->derivative = 0.0;

//	if (fabs(nodes[i]->outgoing_connections[j]->weight) < 10e-5) {
//		std::cout << "SMALL WEIGHT!" << endl;
//	}
}


