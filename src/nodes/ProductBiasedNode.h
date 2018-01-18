#ifndef __PRODUCTBIASEDNODE_H_
#define __PRODUCTBIASEDNODE_H_


#include "Node.h"

struct ProductBiasedNode : Node
{
	weight_t bias;
	
	ProductBiasedNode();
	
	void add_input(weight_t input, weight_t weight, unsigned int time);
	
	weight_t get_input_to_weight_derivative(Connection* connection, unsigned int time);
	weight_t get_input_to_activation_derivative(Connection* connection, unsigned int time);
	
	void grow_buffer(unsigned int size);
	
	void activate(unsigned int timestep);
	
	const nodetype get_type() { return Node::PRODUCTBIASED_NODE; }
};




#endif /*EPINODE_H_*/
