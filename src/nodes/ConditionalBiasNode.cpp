/*
 * ConditionalBias.cpp
 *
 *  Created on: Oct 26, 2009
 *      Author: brandmaier
 */

#include "ConditionalBiasNode.h"
#include <cstdlib>

ConditionalBiasNode::ConditionalBiasNode(bool type) {
		name = "CBias  ";
		activation_function = &linear;
		derivative_activation_function = &dlinear;
		this->type = type;
}

void ConditionalBiasNode::activate(unsigned int timestep)
{
	if (type) {
		if (timestep == 0) 	this->actbuf[timestep] = 1.0;
		else 	this->actbuf[timestep] = 0.0;
	} else {
		if (timestep == 0) 	this->actbuf[timestep] = 0.0;
		else 	this->actbuf[timestep] = 1.0;
	}

}

ConditionalBiasNode* ConditionalBiasNode::from_string(string str)
{
	if (atoi(str.c_str()) == 1)
		return new ConditionalBiasNode(true);
	else
		return new ConditionalBiasNode(false);
}

string ConditionalBiasNode::to_string(){
	return type?"1":"0";
}
