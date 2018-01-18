#ifndef __BIASNODE_H_
#define __BIASNODE_H_

#include <iostream>
#include "Node.h"


struct BiasNode : Node
{
	BiasNode();
	~BiasNode() {};	
	void activate(unsigned int timestep);

	
	const int get_type() { return Node::BIAS_NODE; }	

	void  add_input(weight_t input, weight_t weight, unsigned int time);
	
};

#endif  /*BIASNODE_H_*/
