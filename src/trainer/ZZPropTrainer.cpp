/*
 * ZZPropTrainer.cpp
 *
 *  Created on: Jan 06, 2020
 *      Author: brandmaier
 */

#include "ZZPropTrainer.h"

weight_t ZZPropTrainer::beta = 0.9;
weight_t ZZPropTrainer::squared_gradient_moving_average = 0.0;

weight_t ZZPropTrainer::eta_minus = 0.5;
weight_t ZZPropTrainer::eta_plus = 1.2;
weight_t ZZPropTrainer::gamma_min = 0 ;//10e-6;
weight_t ZZPropTrainer::gamma_max = 50;

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
  
  
  weight_t sign = c->derivative*c->previous_derivative;
  weight_t change = 0;
  
  if (isnan(sign)) {
    std::cout << "Derivative has become NaN" << endl;
    exit(-1);
  }
  
  // same direction
  if (sign > 0) {
    c->gamma = c->gamma + 1 ;
    
    change = pow(2,1+c->gamma) * sign;
    
  // sign switch
  } else if (sign < 0) {
    
    if (c->gamma > 1)
      c->gamma = c->gamma / 2;
    
    // iImprovedRProp+
    if (current_error > previous_error) {
      change = -c->previous_weight_change;
    }
    
    // iImprovedRProp-
    c->derivative = 0;
  } else {
    change = pow(2,1+c->gamma) * sign;
  }
  
//  change = c->derivative * this->learning_rate;
  
 // std::cout << " Change : " << change << " und sign " << sign <<
//  		 " derivs:" << c->derivative << "," << c->previous_derivative << " gamma: " << c->gamma << endl;
  
  c->weight += change;
  c->previous_weight_change = change;
  c->previous_derivative = c->derivative;
  c->derivative = 0.0;
  
  //	if (fabs(nodes[i]->outgoing_connections[j]->weight) < 10e-5) {
  //		std::cout << "SMALL WEIGHT!" << endl;
  //	}
}
