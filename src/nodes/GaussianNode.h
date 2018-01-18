/*
 * GaussianNode.h
 *
 *  Created on: Oct 8, 2009
 *      Author: brandmaier
 */

#ifndef GAUSSIANNODE_H_
#define GAUSSIANNODE_H_

#include "Node.h"

struct GaussianNode : Node
{
	GaussianNode();
	~GaussianNode();

	weight_t get_activation_function_derivative(unsigned int time);
	void activate(unsigned int time);

};

#endif /* GAUSSIANNODE_H_ */
