/*
 * SteepSigmoidNode.h
 *
 *  Created on: Nov 4, 2009
 *      Author: brandmaier
 */

#ifndef SteepSigmoidNode_H_
#define SteepSigmoidNode_H_


#include "Node.h"
#include <sstream>
#include <iostream>

struct SteepSigmoidNode : Node
{
	double a;

	/**
	 *
	 * larger values than 1.0 will make the sigmoid steeper,
	 * smaller values than 1.0 will make it closer to a linear function
	 *
	 */
	SteepSigmoidNode(double a)
	{
		std::stringstream namestream;
		namestream << "SteepSig" << id;
		name = namestream.str();

		this->a = a;
	}

	const nodetype get_type() { return Node::STEEP_SIGMOID_NODE; }

	void activate(unsigned int timestep) {

		actbuf[timestep] = 1.0/(1.0+safeexp(-a*inbuf[timestep]));
		//cout << actbuf[timestep] << endl;
	}

	weight_t get_activation_function_derivative(unsigned int time)
	{
		return (a*this->actbuf[time] * (1.0-this->actbuf[time]));
	}

	virtual string to_string()
	{
		stringstream ostr;
		ostr << a;
		return ostr.str();
	}

	static SteepSigmoidNode* from_string(string str)
	{
		return new SteepSigmoidNode( atof(str.c_str()) );
	}

};



#endif /* SCALEDSigmoidNode_H_ */
