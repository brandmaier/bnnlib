#include "StochasticSigmoidNode.h"
#include <iostream>

/*
 * This is a sigmoid node with stochastic gain following the computational model
 * of Li, Lindenberger, & Frensch (2000) in Neurocomputing 32-33, p879--890
 */
	
StochasticSigmoidNode::StochasticSigmoidNode()
	{
  this->gain = 1;
  this->bias = 0;
  this->stochastic_range = 0;
      init();
	}


StochasticSigmoidNode::StochasticSigmoidNode(weight_t gain, weight_t bias, weight_t stochastic_range)
{
  
  this->gain = gain;
  this->bias = bias;
  this->stochastic_range = stochastic_range;
  init();
  
}

void StochasticSigmoidNode::init()
{
  std::stringstream namestream;
  namestream << "Stochastic Sigmoid" << id;
  name = namestream.str();
  
  activation_function = &sigmoid;
  derivative_activation_function = &dsigmoid;	
  
  std::random_device                  rand_dev; // seed
  // TODO: make this static
  this->generator = std::mt19937(rand_dev()); //Standard mersenne_twister_engine seeded with rd()
  
  distr = std::uniform_real_distribution<double>(-stochastic_range,+stochastic_range);
}

void StochasticSigmoidNode::activate(unsigned int timestep) {
  
  weight_t current_gain = gain;
  if (this->stochastic_range != 0.0) {

    double added_gain = distr(this->generator);
   // cout << "Added gain " << added_gain << "; ";
    current_gain += added_gain;
    current_gain = max(0.0, current_gain); // clip negative values
  } 
  
  actbuf[timestep] = 1.0/(1.0+safeexp(-current_gain*inbuf[timestep]+bias));
}

weight_t StochasticSigmoidNode::get_activation_function_derivative(unsigned int time)
{
  return this->actbuf[time] * (1.0-this->actbuf[time]);
}