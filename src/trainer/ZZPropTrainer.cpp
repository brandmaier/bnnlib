/*
 * ZZPropTrainer.cpp
 *
 *  Created on: Jan 06, 2020
 *      Author: brandmaier
 */

#include "ZZPropTrainer.h"

weight_t ZZPropTrainer::beta = 0.9;
weight_t ZZPropTrainer::squared_gradient_moving_average = 0.0;

ZZPropTrainer::ZZPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
}

void ZZPropTrainer::change_weight(Trainable* c)
{

			if (c->freeze_weight == true)
			{
				c->previous_derivative = c->derivative;
				c->derivative = 0.0;
				c->previous_weight_change = 0.0;
				return;
			}
			
			// update moving area
			squared_gradient_moving_average = beta * squared_gradient_moving_average +
			  (1-beta) * (c->derivative*c->derivative);

			// compute change
			weight_t change = learning_rate / sqrt(squared_gradient_moving_average) * c->derivative;

			//std::cout << " Change : " << change << " und sign " << sign <<
			//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;

			// update all information in connection
			c->weight += change;
			c->previous_weight_change = change;
			c->previous_derivative = c->derivative;
			c->derivative = 0.0;


}
