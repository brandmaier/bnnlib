/*
 * GaussianNode.cpp
 *
 *  Created on: Oct 8, 2009
 *      Author: brandmaier
 */

#include "GaussianNode.h"


void GaussianNode::activate(unsigned int time) {
	this->actbuf[time] = exp(-this->inbuf[time]*this->inbuf[time]) ;
}

weight_t  GaussianNode::get_activation_function_derivative(unsigned int time)
{
	return -2.0* this->inbuf[time] * this->actbuf[time];
}
