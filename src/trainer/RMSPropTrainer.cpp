/*
 * RMSPropTrainer.cpp
 * 
 * Root-Mean-Squared Propagation Trainer
 * 
 * uses Trainable's gamma to store moving average of 
 * squared gradients
 *
 *  Created on: Jan 03, 2019
 *      Author: brandmaier
 */

#include "RMSPropTrainer.h"

weight_t RMSPropTrainer::beta = 0.9;
//weight_t RMSPropTrainer::x->gamma = 0.0;

RMSPropTrainer::RMSPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = false;

	for (unsigned int i=0; i < this->network->get_num_nodes(); i++)
	  (this->network->nodes[i])->rms = 0.0;
	
//	this->learning_rate = 0.01;
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
			
	//		std::cout << "Previous MA " << c->rms <<"Derivative is" << c->derivative<<" updated with "<<(c->derivative*c->derivative)<<endl;
			
			// update moving area
			c->rms = beta * c->rms +
			  (1-beta) * (c->derivative*c->derivative);
			
			// epsilon constant is to safe-guard against explosion when gamma -> 0
			static const weight_t epsilon = 1e-13;

			// compute change
			weight_t change = learning_rate / sqrt(c->rms+epsilon) * c->derivative;

	//		std::cout <<  "Change : " << change << " moving average " << c->rms <<" Scaling is" << sqrt(c->rms+epsilon) << endl<<"*"<<endl;
			//std::cout << " Change : " << change << " und sign " << sign <<
			//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->rms << endl;

			// update all information in connection
			c->weight += change;
			c->previous_weight_change = change;
			c->previous_derivative = c->derivative;
			c->derivative = 0.0;


}
