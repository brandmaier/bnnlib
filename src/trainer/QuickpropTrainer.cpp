/*
 * QuickpropTrainer.cpp
 *
 *  Created on: Dec 17, 2009
 *      Author: brandmaier
 */

#include "QuickpropTrainer.h"

QuickpropTrainer::QuickpropTrainer(Network* network) {
	this->network = network;

}

QuickpropTrainer::~QuickpropTrainer() {

}

void QuickpropTrainer::change_weight(Trainable* conn)
{
			if (conn->freeze_weight == true){
				conn->previous_derivative = conn->derivative;
				conn->derivative = 0.0;
				conn->previous_weight_change = 0.0;
				return;
			}

			weight_t change = momentum*learning_rate*conn->derivative
					+ (1.0-momentum)*conn->previous_weight_change;

			weight_t divisor = (conn->previous_derivative-conn->derivative);

//			weight_t change = 0.0;
			if (divisor != 0.0) {
			 change = conn->previous_weight_change
					 *(conn->derivative/(conn->previous_derivative-conn->derivative))
			;
		//	 cout << "bruch:"<< (conn->derivative/(conn->previous_derivative-conn->derivative)) << ","<< conn->previous_weight_change << endl;
			}

		//	cout << "first guess" << endl;

			weight_t mu = 1.75;

			/*if (conn->derivative*conn->previous_derivative >= 0)
				change+= 0.00001*conn->derivative;
*/

			if (network->timestep > 0)
			if (abs(change) > abs(mu*conn->previous_weight_change))
			{
				change = abs(mu*conn->previous_weight_change)*(change>0?1:-1);
				//cout << "ABS" << endl;
			}



		//	cout << "change: " << change << "," << network->timestep << "," << total_iterations << endl;

			conn->weight += change;
			conn->previous_weight_change = change;
			conn->previous_derivative = conn->derivative;
			conn->derivative = 0.0;

}
