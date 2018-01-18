#ifndef POLYNOMIALNODE_H_
#define POLYNOMIALNODE_H_

#include "../functions.h"
#include "Node.h"

struct PolynomialNode : Node
{
	unsigned int degree;
	
	PolynomialNode();
	~PolynomialNode() {};
	
//	virtual void add_input(weight_t input, weight_t weight, unsigned int time);
//	virtual void activate(unsigned int timestep);	
	
};

#endif /*POLYNOMIALNODE_H_*/
