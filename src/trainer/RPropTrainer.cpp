/*
 * RPropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "RPropTrainer.h"

weight_t RPropTrainer::eta_minus = 0.5;
weight_t RPropTrainer::eta_plus = 1.2;
weight_t RPropTrainer::gamma_min = 10e-6;
weight_t RPropTrainer::gamma_max = 5.0;

RPropTrainer::RPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
}

void RPropTrainer::change_weight(Trainable* c)
{

			if (c->freeze_weight == true)
			{
				c->previous_derivative = c->derivative;
				c->derivative = 0.0;
				c->previous_weight_change = 0.0;
				return;
			}


			weight_t sign = c->derivative*c->previous_derivative;

			if (isnan(sign)) {
				std::cout << "Derivative has become NaN" << endl;
				exit(-1);
			}

			if (sign > 0) {
				c->gamma = fmin( c->gamma * RPropTrainer::eta_plus, RPropTrainer::gamma_max );
			} else if (sign < 0) {
				c->gamma = fmax( c->gamma * RPropTrainer::eta_minus, RPropTrainer::gamma_min );
			}

			weight_t change = c->gamma * (c->derivative>=0?+1:-1);

			//std::cout << " Change : " << change << " und sign " << sign <<
			//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;

			c->weight += change;
			c->previous_weight_change = change;
			c->previous_derivative = c->derivative;
			c->derivative = 0.0;


}
