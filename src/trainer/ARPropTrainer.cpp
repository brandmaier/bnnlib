/*
 * ARPropTrainer.cpp
 *
 *  Created on: Oct 19, 2009
 *      Author: brandmaier
 */

#include "ARPropTrainer.h"
#include "../functions.h"
#include "../Connection.h"
#include "../Trainable.h"
#include <cmath>

weight_t ARPropTrainer::eta_minus = 0.5;
weight_t ARPropTrainer::eta_plus = 1.2;
weight_t ARPropTrainer::gamma_min = 0;
weight_t ARPropTrainer::gamma_max = 50.0;


ARPropTrainer::~ARPropTrainer() {}


ARPropTrainer::ARPropTrainer(Network* network) : Trainer()
{
	this->network = network;
	this->batch_learning = true;

	// find all relevant connections in the Network
	// i.e. all Connections to LSTM gates
	for (unsigned int i=0; i < network->nodes.size(); i++)
	{
		for (unsigned int j=0; j < network->nodes[i]->outgoing_connections.size(); j++)
		{
			Connection* connection = network->nodes[i]->outgoing_connections[j];
			if ((connection->to->functional_type == Node::LSTM_INPUT_GATE) ||
				(connection->to->functional_type == Node::LSTM_FORGET_GATE) ||
				(connection->to->functional_type == Node::LSTM_OUTPUT_GATE))
			{
				connection->tag = true;
			} else {
				connection->tag = false;
			}
		}
	}

	q = 1;
}

void ARPropTrainer::prepare_step()
{
  phase = !phase;
  cout << "Setting Phase to " << phase << endl;
  
  return;
}


void ARPropTrainer::change_weight(Trainable* c)
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


/*	if (dynamic_cast<Connection*>(c) != NULL) {
		Connection* conn = dynamic_cast<Connection*>(c);
		cout << conn->from << " to " << conn->to << endl;
	}
*/



	if (isnan(sign)) {
		std::cout << "Derivative has become NaN" << endl;
		exit(-1);
	}

	if (sign > 0) {
		c->gamma = fmin( c->gamma * ARPropTrainer::eta_plus, ARPropTrainer::gamma_max );

		 change = c->gamma * (c->derivative>=0?+1:-1);
	} else if (sign < 0) {
		c->gamma = fmax( c->gamma * ARPropTrainer::eta_minus, ARPropTrainer::gamma_min );

		// iARProp+
		if (current_error > previous_error) {
			change = -c->previous_weight_change;
		}

		// iImprovedRProp-
		c->derivative = 0;
	} else {
		 change = c->gamma * (c->derivative>=0?+1:-1);
	}

	//cout << "Change: " << change << " sign: " << sign << endl;
	
	/*if (c->tag) {
		cout << "Change: " << change << " sign: " << sign << endl;
	}*/

	

	
	//Connection* conn = dynamic_cast<Connection*>(c);
	/*if (conn != NULL) {
		if( (conn->to->functional_type == Node::LSTM_INPUT_GATE) || (conn->to->functional_type == Node::LSTM_OUTPUT_GATE)) {
			if (sign < 0) {
				double corr = 1+(rand_uniform_01()*(1.0/exp2f(total_iterations*0.01)));
				//cout << "correction:" << corr <<","<< total_iterations <<","<< (1.0/exp2f(total_iterations*0.1)) <<endl;
				change = change * corr;
			}
		}
	}*/


//	std::cout << " Change : " << change << " und sign " << sign <<
//			 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;

	// phase
	if (phase) {
	  if (c->tag) {
	    change=0.1*change;
	  } 
	} else {
	  if (!c->tag) {
	    change=0.1*change;
	  }
	}
	
	c->weight += change;
	c->previous_weight_change = change;
	c->previous_derivative = c->derivative;
	c->derivative = 0.0;

//	if (fabs(nodes[i]->outgoing_connections[j]->weight) < 10e-5) {
//		std::cout << "SMALL WEIGHT!" << endl;
//	}
}
