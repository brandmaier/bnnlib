#ifndef __PRODUCTNODE_H_
#define __PRODUCTNODE_H_


#include "Node.h"

struct ProductNode : Node
{
	
	ProductNode();
	
	void add_input(weight_t input, weight_t weight, unsigned int time);
	
	weight_t get_input_to_weight_derivative(Connection* connection, unsigned int time);
	weight_t get_input_to_activation_derivative(Connection* connection, unsigned int time);
	
	void grow_buffer(unsigned int size);
	void reset_buffer(unsigned int timestep);
	
	void activate(unsigned int timestep);
	
	const nodetype get_type() { return Node::PRODUCT_NODE; }
};




#endif /*EPINODE_H_*/
