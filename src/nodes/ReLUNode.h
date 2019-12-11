/*
 *  LinearNode.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 20.08.19.
 *  Copyright 2019 Andreas Brandmaier. All rights reserved.
 *
 */

#ifndef __RELU_H__
#define __RELU_H__

#include <string>
#include <sstream>
#include "../functions.h"
#include "Node.h"
using namespace std;



struct ReLUNode : Node
{

	ReLUNode();
	~ReLUNode() {};
	
	const int get_type() { return Node::RELU_NODE; }
	
	void activate(unsigned int timestep) ;
	weight_t get_activation_function_derivative(unsigned int time);
};

#endif
