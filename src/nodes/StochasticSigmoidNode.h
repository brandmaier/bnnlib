#ifndef STOCHASTICSIGMOIDNODE_H_
#define STOCHASTICSIGMOIDNODE_H_

#include "Node.h"
#include <sstream>
#include <random>

struct StochasticSigmoidNode : Node
{
	StochasticSigmoidNode();
  StochasticSigmoidNode(weight_t gain, weight_t bias, weight_t stochastic_range);
  
  weight_t gain = 1.0;
  weight_t bias = 0.0;
  weight_t stochastic_range = 0.0;
  

  std::uniform_real_distribution<double>  distr;
  std::mt19937 generator;
    
	const nodetype get_type() { return Node::SIGMOID_NODE; }

	void init();
	
	void activate(unsigned int timestep);

	weight_t get_activation_function_derivative(unsigned int time);
	
};

#endif /*STOCHASTICSIGMOIDNODE_H_*/
