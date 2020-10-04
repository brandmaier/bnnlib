/*
 * ADAMTrainer.cpp
 * 
 * ADAM - Adaptive Moment Optimization
 *
 *  Created on: Jan 09, 2019
 *      Author: brandmaier
 */

#include "ADAMTrainer.h"

weight_t ADAMTrainer::beta1 = 0.9;
weight_t ADAMTrainer::beta2 = 0.999;
//weight_t ADAMTrainer::x->gamma = 0.0;

ADAMTrainer::ADAMTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = false;

	for (unsigned int i=0; i < this->network->get_num_nodes(); i++)
	{
	  (this->network->nodes[i])->rms = 0.0;
	  (this->network->nodes[i])->rms_sq = 0.0;
	}
	
	this->learning_rate = 0.001; // from Tensorflow
}

void ADAMTrainer::change_weight(Trainable* c)
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
			c->rms_sq = beta2 * c->rms_sq +
			  (1-beta2) * (c->derivative*c->derivative);
			
			c->rms = beta1*c->rms + 
			  (1-beta1) * (c->derivative);
			
			// epsilon constant is to safe-guard against explosion when gamma -> 0
			static const weight_t epsilon = 1e-8;
			

			// compute change
			weight_t change = learning_rate * (c->rms) / (sqrt(c->rms_sq)+epsilon);

	//		std::cout <<  "Change : " << change << " moving average " << c->rms <<" Scaling is" << sqrt(c->rms+epsilon) << endl<<"*"<<endl;
			//std::cout << " Change : " << change << " und sign " << sign <<
			//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->rms << endl;

			// update all information in connection
			c->weight += change;
			c->previous_weight_change = change;
			c->previous_derivative = c->derivative;
			c->derivative = 0.0;


}
