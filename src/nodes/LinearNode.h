/*
 *  LinearNode.h
 *  BNNlib
 *
 *  Created by Andreas Brandmaier on 20.08.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __LINEAR_H__
#define __LINEAR_H__

#include <string>
#include <sstream>
#include "../functions.h"
#include "Node.h"
using namespace std;



struct LinearNode : Node
{

	LinearNode();
	~LinearNode() {};
	
	const int get_type() { return Node::LINEAR_NODE; }
	
	void activate(unsigned int timestep) ;
	weight_t get_activation_function_derivative(unsigned int time);
};

#endif
