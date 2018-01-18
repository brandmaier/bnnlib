#ifndef SIGMOIDNODE_H_
#define SIGMOIDNODE_H_

#include "Node.h"
#include <sstream>

struct SigmoidNode : Node
{
	SigmoidNode();
	
	const nodetype get_type() { return Node::SIGMOID_NODE; }

	void activate(unsigned int timestep) {

		actbuf[timestep] = 1.0/(1.0+safeexp(-inbuf[timestep]));
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return this->actbuf[time] * (1.0-this->actbuf[time]);
	}
	
};

#endif /*SIGMOIDNODE_H_*/
