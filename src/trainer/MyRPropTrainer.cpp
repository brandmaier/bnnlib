/*
 * MyRPropTrainer.cpp
 *
 *  Created on: Dec 15, 2009
 *      Author: brandmaier
 */

#include "MyRPropTrainer.h"

weight_t MyRPropTrainer::eta_minus = 0.5;
weight_t MyRPropTrainer::eta_plus = 1.2;
weight_t MyRPropTrainer::gamma_min = 10e-10;
weight_t MyRPropTrainer::gamma_max = 5.0;

MyRPropTrainer::MyRPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;
}

void MyRPropTrainer::change_weight(Trainable* c)
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

	/*if (current_error < previous_error) {
		//cout << "*"<< current_error << " " << previous_error << endl;
		MyRPropTrainer::eta_plus = 1.3;
	}else {
		MyRPropTrainer::eta_plus = 1.1;

	}*/


	if (sign > 0) {
		c->gamma = fmin( c->gamma * MyRPropTrainer::eta_plus, MyRPropTrainer::gamma_max );

		if (abs(c->derivative)>abs(c->previous_derivative))
		{

			c->gamma = fmin( c->gamma * MyRPropTrainer::eta_plus, MyRPropTrainer::gamma_max );

		}

	//	if (dynamic_cast<Connection*>(c) != NULL) {

	//	}
		//double momentum = 0.9;
//		change = momentum*(c->gamma * (c->derivative>=0?+1:-1))+(1-momentum)*c->previous_weight_change;
		change = (c->gamma * (c->derivative>=0?+1:-1));
		//MyRPropTrainer::eta_plus+=0.001;
	//	if ((c->derivative*c->previous_derivative) > 0)
	//		change+=(1-momentum)*c->previous_weight_change;

	/*	Connection* conn = static_cast<Connection*>(c);
		if (conn->to->name == "forgetgate")
		{
			//change += 0.001;
			change += c->gamma/10.0;
			//cout << "FORGE!" << endl;
		}
		if (conn->to->name == "outgate")
		{
			//change += 0.001;
			change -= c->gamma/10.0;
			//cout << "FORGE!" << endl;
		}
*/
	} else if (sign < 0) {
		c->gamma = fmax( c->gamma * MyRPropTrainer::eta_minus, MyRPropTrainer::gamma_min );
		//MyRPropTrainer::eta_plus-=0.001;
		if (abs(c->derivative)<abs(c->previous_derivative))
		{
			c->gamma = fmax( c->gamma * MyRPropTrainer::eta_minus, MyRPropTrainer::gamma_min );

		}

		// iMyRProp+
		if (current_error > previous_error) {
			change = -c->previous_weight_change;
		}

		// iMyRProp-
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

