#ifndef SINUNIT_H_
#define SINUNIT_H_

#include "Node.h"
#include <cmath>
#include "../functions.h"

struct SinNode : Node
{

	Trainable freq;

	SinNode()
	{
		name = "Sin";
		freq.weight = 1.0;
	}
	
	~SinNode() {

	}

	void activate(unsigned int time) {
		this->actbuf[time] = sin(freq.weight*this->inbuf[time]) ;
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return cos(freq.weight*this->inbuf[time] ) * freq.weight;
	}


	const nodetype get_type() { return Node::SIN_NODE; }

};

#endif /*SINUNIT_H_*/
