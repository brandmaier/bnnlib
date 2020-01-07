/*
 * RMSPropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "RMSPropTrainer.h"

weight_t RMSPropTrainer::beta = 0.9;
weight_t RMSPropTrainer::squared_gradient_moving_average = 0.0;

RMSPropTrainer::RMSPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
}

void RMSPropTrainer::change_weight(Trainable* c)
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
