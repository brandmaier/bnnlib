/*
 * SARropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "SARPropTrainer.h"
#include <math.h>

weight_t SARPropTrainer::eta_minus = 0.5;
weight_t SARPropTrainer::eta_plus = 1.2;
weight_t SARPropTrainer::gamma_min = 10e-6;
weight_t SARPropTrainer::gamma_max = 5.0;

weight_t SARPropTrainer::k1 = 0.01;
weight_t SARPropTrainer::k2 = 0.1;
weight_t SARPropTrainer::k3 = 3;

SARPropTrainer::SARPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
	this->temperature = 0.01;
}

void SARPropTrainer::change_weight(Trainable* c)
{
		double error = this->get_normalized_root_mean_sequence_error();

			if (c->freeze_weight == true)
			{
				c->previous_derivative = c->derivative;
				c->derivative = 0.0;
				c->previous_weight_change = 0.0;
				return;
			}

			weight_t change = 0.0;

			// SAR: modify derivative with weight_decay   TODO: make more efficient!
			double exponent = temperature*total_iterations;
			//if (exponent <= 50) {
				c->derivative -= k1 * c->weight * pow(2.0, -exponent);
			//}

			weight_t sign = c->derivative*c->previous_derivative;

			if (isnan(sign)) {
				std::cout << "Derivative has become NaN" << endl;
				exit(-1);
			}

			if (sign > 0) {
				c->gamma = fmin( c->gamma * SARPropTrainer::eta_plus, SARPropTrainer::gamma_max );
				change = c->gamma * (c->derivative>=0?+1:-1);
			} else if (sign < 0) {

					c->gamma = fmax( c->gamma * SARPropTrainer::eta_minus, SARPropTrainer::gamma_min );
					if (c->gamma < error*error*k2) {
						c->gamma += k3* rand_uniform_01()*pow(2.0, -temperature*total_iterations);
					}
					c->derivative = 0;

			} else {
				change = c->gamma * (c->derivative>=0?+1:-1);
			}

			//weight_t change = c->gamma * (c->derivative>=0?+1:-1);

			//std::cout << " Change : " << change << " und sign " << sign <<
			//		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;

			c->weight += change;
			c->previous_weight_change = change;
			c->previous_derivative = c->derivative;
			c->derivative = 0.0;


}
