/*
 * ScaledTanhNode.h
 *
 *  Created on: Nov 5, 2009
 *      Author: brandmaier
 */

#ifndef SCALEDTANHNODE_H_
#define SCALEDTANHNODE_H_

#include "Node.h"

struct ScaledTanhNode : Node
{
	weight_t a;

	ScaledTanhNode(double a) { this->a = a; };

	const nodetype get_type() { return Node::SCALED_TANH_NODE; }

	void activate(unsigned int timestep) {
		//cout << "ACTIVATE!" << endl;
		// actbuf[timestep] = activation_function(inbuf[timestep]);
		actbuf[timestep] =  a*tanh(inbuf[timestep]); //2.0 / (1.0 + exp(-2.0 *inbuf[timestep] )) - 1.0;
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return 1.0 - this->actbuf[time]*this->actbuf[time] ;
	}

	virtual string to_string()
	{
		stringstream ostr;
		ostr << a;
		return ostr.str();
	}

	static ScaledSigmoidNode* from_string(string str)
	{
		return new ScaledSigmoidNode( atof(str.c_str()) );
	}

};

#endif /* SCALEDTANHNODE_H_ */
