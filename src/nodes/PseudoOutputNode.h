/*
 * PseudoOutputNode.h
 *
 *  Created on: Jan 25, 2010
 *      Author: brandmaier
 */

#ifndef PSEUDOOUTPUTNODE_H_
#define PSEUDOOUTPUTNODE_H_

#include "Node.h"

/**
 * A Pseudo-Output node is treated as a hidden node in the Network
 * but injects its own delta-terms in the backpropagation pass.
 * Therefore the network has to generate its own delta-terms during
 * the forward pass. While this procedure is a dirty workaround, it
 * offers interesting sideeffects to exploit.
 *
 */
struct PseudoOutputNode : Node
{
	PseudoOutputNode()
	{
		name = "PseudoOutput";
	}

	void activate(unsigned int time)
	{
		this->actbuf[time] = this->inbuf[time];
		if (time == 0)
			this->delta[time] = 0;
		else
			this->delta[time] += this->inbuf[time];
		/* actually this is: (this->inbuf[time-0]) */
	}

	weight_t  get_activation_function_derivative(unsigned int time)
	{
		return 1.0;
	}

	const int get_type() { return Node::PSEUDO_OUTPUT_NODE; }
};

#endif /* PSEUDOOUTPUTNODE_H_ */
